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

#include <nemesi/rtcp.h>
#include <nemesi/version.h>
#include <pwd.h>

int rtcp_build_sdes(rtp_session * rtp_sess, rtcp_pkt * pkt, int left)
{
    struct passwd *pwitem = getpwuid(getuid());
    rtcp_sdes_item_t *item;
    char str[MAX_SDES_LEN] = "";
    int len, pad;
    char addr[128];        /* Unix domain is largest */

    memset(str, 0, MAX_SDES_LEN);

    /* SDES CNAME: username@ipaddress */
    // if ( sock_ntop_host(rtp_sess->transport.dstaddr.addr, rtp_sess->transport.dstaddr.addr_len, addr, sizeof(addr)) ) {
    if (nms_addr_ntop(&rtp_sess->transport.RTP.u.udp.dstaddr, addr, sizeof(addr))) {
        strcpy(str, pwitem->pw_name);
        strcat(str, "@");
        strcat(str, addr);
    }
    if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
          sizeof(rtcp_common_t) + 1) >> 2) > (unsigned int) left)
        /* No space left in UDP pkt */
        return 0;

    len =
        (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
         sizeof(rtcp_common_t)) >> 2;
    pkt->common.ver = RTP_VERSION;
    pkt->common.pad = 0;
    pkt->common.count = 1;
    pkt->common.pt = RTCP_SDES;
    pkt->r.sdes.src = htonl(rtp_sess->local_ssrc);

    item = pkt->r.sdes.item;

    item->type = RTCP_SDES_CNAME;
    item->len = strlen(str);
    strcpy((char *) item->data, str);

    item = (rtcp_sdes_item_t *) ((char *) item + strlen((char *) item));

    /* SDES NAME: real name, if it exists */
    if (strlen(strcpy(str, pwitem->pw_gecos))) {
        if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
              sizeof(rtcp_common_t) + 1) >> 2) > (unsigned int) left) {
            /* No space left in UDP pkt */
            pad = 4 - len % 4;
            len += pad / 4;
            while (pad--)
                *((char *) item++) = 0;
            pkt->common.len = htons(len);
            return len;
        }

        len +=
            (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
             sizeof(rtcp_common_t) + 1) >> 2;

        item->type = RTCP_SDES_NAME;
        item->len = strlen(str);
        strcpy((char *) item->data, str);

        item =
            (rtcp_sdes_item_t *) ((char *) item +
                      strlen((char *) item));
    }

    /* SDES TOOL */
    sprintf(str, "%s - %s", PROG_NAME, PROG_DESCR);
    if (((strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
          sizeof(rtcp_common_t)) >> 2) > (unsigned int) left) {
        /* No space left in UDP pkt */
        pad = 4 - len % 4;
        len += pad / 4;
        while (pad--)
            // *(((char *)item)++)=0;
            *((char *) item++) = 0;
        pkt->common.len = htons(len);
        return len;
    }

    len +=
        (strlen(str) + sizeof(rtcp_sdes_item_t) - 1 +
         sizeof(rtcp_common_t) + 1) >> 2;

    item->type = RTCP_SDES_TOOL;
    item->len = strlen(str);
    strcpy((char *) item->data, str);

    item = (rtcp_sdes_item_t *) ((char *) item + strlen((char *) item));

    pad = 4 - len % 4;
    len += pad / 4;
    while (pad--)
        // *(((char *)item)++)=0;
        *((char *) item++) = 0;
    pkt->common.len = htons(len);

    return len;
}
