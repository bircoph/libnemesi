/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rtpparser.h"

static rtpparser_info mpv_served = {
    32,
    {"MPV", NULL}
};

#define DEFAULT_MPV_DATA_FRAME 65535

typedef struct {
//      mpa_data mpa_info;
    char *data;
    uint32_t data_size;
} rtp_mpv;

typedef struct {
#ifdef WORDS_BIGENDIAN
    uint32_t mbz:5;
    uint32_t t:1;
    uint32_t tr:10;
    uint32_t an:1;
    uint32_t n:1;
    uint32_t s:1;
    uint32_t b:1;
    uint32_t e:1;
    uint32_t p:3;
    uint32_t fbv:1;
    uint32_t bfc:3;
    uint32_t ffv:1;
    uint32_t ffc:3;
#else
    uint32_t tr_h:2;
    uint32_t t:1;
    uint32_t mbz:5;
    uint32_t tr_l:8;
    uint32_t p:3;
    uint32_t e:1;
    uint32_t b:1;
    uint32_t s:1;
    uint32_t n:1;
    uint32_t an:1;
    uint32_t ffc:3;
    uint32_t ffv:1;
    uint32_t bfc:3;
    uint32_t fbv:1;
#endif
    union {
        uint8_t data[1];
        struct {
            uint32_t ext_hdr;
            uint8_t data[1];
        } mpeg2;
    } pt;
} rtp_mpv_pkt;

#define RTP_MPV_PKT(pkt)        ((rtp_mpv_pkt *)(RTP_PKT_DATA(pkt)))
#define RTP_MPV_DATA(pkt)        (RTP_MPV_PKT(pkt)->t ? RTP_MPV_PKT(pkt)->pt.mpeg2.data : RTP_MPV_PKT(pkt)->pt.data)
#define RTP_MPV_DATA_LEN(pkt, pkt_size)    (RTP_MPV_PKT(pkt)->t ? RTP_PAYLOAD_SIZE(pkt, pkt_size)-8 : RTP_PAYLOAD_SIZE(pkt, pkt_size)-4)

#ifdef WORDS_BIGENDIAN
#define RTP_MPV_TR(pkt)            (RTP_MPV_PKT(pkt)->tr)
#else
#define RTP_MPV_TR(pkt)            (RTP_MPV_PKT(pkt)->tr_h << 8 | RTP_MPV_PKT(pkt)->tr_l)
#endif

static int mpv_parse(rtp_ssrc * stm_src, rtp_frame * fr, rtp_buff * config)
{
    rtp_mpv *mpv_priv = stm_src->privs[fr->pt];
    rtp_pkt *pkt;
    size_t pkt_len;
    uint32_t tot_pkts = 0;

    if (!(pkt = rtp_get_pkt(stm_src, &pkt_len)))
        return RTP_BUFF_EMPTY;

    // pkt_pt = RTP_PKT_PT(pkt);
    // fr->timestamp = RTP_PKT_TS(pkt);


    nms_printf(NMSML_DBG3, "\n[MPV]: header: mbz:%u t:%u tr:%u an:%u n:%u s:%u b:%u e:%u p:%u fbv:%u bfc:%u ffv:%u ffc:%u\n", RTP_MPV_PKT(pkt)->mbz, RTP_MPV_PKT(pkt)->t,    /*ntohs( */
           RTP_MPV_TR(pkt) /*) */ ,
           RTP_MPV_PKT(pkt)->an, RTP_MPV_PKT(pkt)->n,
           RTP_MPV_PKT(pkt)->s, RTP_MPV_PKT(pkt)->b,
           RTP_MPV_PKT(pkt)->e, RTP_MPV_PKT(pkt)->p,
           RTP_MPV_PKT(pkt)->fbv, RTP_MPV_PKT(pkt)->bfc,
           RTP_MPV_PKT(pkt)->ffv, RTP_MPV_PKT(pkt)->ffc);
#if 1
    // discard pkt if it's fragmented and the first fragment was lost
    while (!RTP_MPV_PKT(pkt)->b) {
        rtp_rm_pkt(stm_src);
        if (!(pkt = rtp_get_pkt(stm_src, &pkt_len)))
            return RTP_BUFF_EMPTY;
        else if (RTP_PKT_PT(pkt) != fr->pt)
            return RTP_PARSE_ERROR;
    }
#endif

    /* XXX if the frame is not fragmented we could use directly the data contained in bufferpool 
     * instead of memcpy the frame in a newly allocated space */
    // init private struct if this is the first time we're called
    if (!mpv_priv) {
        nms_printf(NMSML_DBG3,
               "[rtp_mpv] allocating new private struct...");
        if (!
            (stm_src->privs[fr->pt] = mpv_priv =
             malloc(sizeof(rtp_mpv))))
            return RTP_ERRALLOC;
        mpv_priv->data_size = max(DEFAULT_MPV_DATA_FRAME, pkt_len);
        if (!(fr->data = mpv_priv->data = malloc(mpv_priv->data_size)))
            return RTP_ERRALLOC;
        nms_printf(NMSML_DBG3, "done\n");
    } else
        fr->data = mpv_priv->data;

    do {
        pkt_len = RTP_MPV_DATA_LEN(pkt, pkt_len);
        if (mpv_priv->data_size < tot_pkts + pkt_len) {
            nms_printf(NMSML_DBG3,
                   "[rtp_mpv] reallocating data...");
            if ((fr->data = mpv_priv->data =
                 realloc(mpv_priv->data, tot_pkts + pkt_len)))
                return RTP_ERRALLOC;
            nms_printf(NMSML_DBG3, "done\n");
        }
        memcpy(fr->data + tot_pkts, RTP_MPV_DATA(pkt), pkt_len);
        tot_pkts += pkt_len;
        rtp_rm_pkt(stm_src);
    } while (!RTP_PKT_MARK(pkt) && (pkt = rtp_get_pkt(stm_src, &pkt_len))
         && (RTP_PKT_TS(pkt) == fr->timestamp)
         && (RTP_PKT_PT(pkt) == fr->pt));

    fr->len = tot_pkts;
    nms_printf(NMSML_DBG3, "fr->len: %d\n", fr->len);

    return RTP_FILL_OK;
}

RTP_PARSER(mpv);
