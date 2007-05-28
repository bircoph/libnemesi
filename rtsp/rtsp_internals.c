/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2007 by team@streaming.polito.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <nemesi/rtspinternals.h>
#include <nemesi/methods.h>
#include <nemesi/utils.h>

int body_exists(char *in_buffer)
{
    int body_len = 0;
    char *con_len;

    if ((con_len = strstrcase(in_buffer, "Content-Length")) != NULL) {
        con_len += 14;
        while ((*(con_len) == ' ') || (*(con_len) == ':'))
            con_len++;
        sscanf(con_len, "%d", &body_len);
    }
    return body_len;
}

int check_response(rtsp_thread * rtsp_th)
{
    int wait_res;
    uint64 wait_s_id;
    int wait_cseq;
    char *str_pos, *content;
    int CSeq;
    uint64 Session_ID = 0;
    int opcode = 0;

    if ((content = strchr((rtsp_th->in_buffer).data, '\n')) == NULL) {
        nms_printf(NMSML_ERR,
               "ERROR: CANNOT find end of line in server response.\n");
        return -1;
    } 
    sscanf(rtsp_th->waiting_for, "%d", &wait_res);
    /* cerco il numero di sequenza del pacchetto arrivato */
    if ((str_pos = strstrcase(content, "CSeq")) == NULL) {
        nms_printf(NMSML_ERR,
               "ERROR: CANNOT find CSeq number in server response.\n");
        return -1;
    }
    str_pos += 5;
    while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
        str_pos++;
    sscanf(str_pos, "%d", &CSeq);
    switch (wait_res) {
    case RTSP_GET_RESPONSE:
        if (CSeq == 1)    /* aspettavo la risposta alla DESCRIBE */
            opcode = RTSP_GET_RESPONSE;
        break;
    case RTSP_SETUP_RESPONSE:
        sscanf(rtsp_th->waiting_for, "%*d.%d", &wait_cseq);
        if (CSeq == wait_cseq)
            opcode = RTSP_SETUP_RESPONSE;
        break;
    default:
        sscanf(rtsp_th->waiting_for, "%*d:%"SCNu64".%d", &wait_s_id,
               &wait_cseq);
        if ((str_pos =
             strstrcase(content,
                "Session:")) != NULL) {
            str_pos += 8;
            while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
                str_pos++;
            sscanf(str_pos, "%"SCNu64, &Session_ID);
            if (Session_ID != wait_s_id) {
                nms_printf(NMSML_ERR, "Unexpected SessionID\n");
                break;
                }
        }
        if (CSeq == wait_cseq)
            opcode = wait_res;
        break;
    }
    nms_printf(NMSML_DBG2, "Opcode Set to %d\n", opcode);
    return opcode;
}

/*!
 * \brief scan status code of an RTSP reply
 *
 * \param status_line the status line in the reply
 * \return reply status code or -1 on error
 */
int check_status(char *status_line, rtsp_thread * rtsp_th)
{
    char ver[32];
    unsigned short res_state;
    char *reason_phrase;
    char *location = NULL;
    // string tokenizers
    char *tkn, *prev_tkn;

    if (sscanf(status_line, "%s %hu ", ver, &res_state) < 2) {
        nms_printf(NMSML_ERR,
               "invalid Status-Line in DESCRIBE Response\n");
        return -1;
    }
    reason_phrase = strchr(strchr(status_line, ' ') + 1, ' ') + 1;

    if (RTSP_IS_SUCCESS(res_state))
        return res_state;
    else if (RTSP_IS_REDIRECT(res_state)) { // (res_state>=300) && (res_state<400)
        nms_printf(NMSML_NORM,
               "WARNING: Redirection. reply was: %hu %s\n",
               res_state, reason_phrase);
        switch (res_state) {
        case RTSP_FOUND:
            if ((prev_tkn =
                 strtok((rtsp_th->in_buffer).data +
                    strlen(status_line) + 1, "\n")) == NULL) {
                nms_printf(NMSML_ERR,
                       "Could not find \"Location\" so... were I'll redirect you?\n");
                return -1;
            }
            while (((tkn = strtok(NULL, "\n")) != NULL)
                   && ((tkn - prev_tkn) > 1)) {
                if (((tkn - prev_tkn) == 2)
                    && (*prev_tkn == '\r'))
                    break;
                if (!strncmpcase(prev_tkn, "Location", 8)) {
                    prev_tkn += 8;
                    while ((*(prev_tkn) == ' ')
                           || (*(prev_tkn) == ':'))
                        prev_tkn++;
                    location = strdup(prev_tkn);
                    // sscanf(prev_tkn,"%d",&location);
                }
                prev_tkn = tkn;
            }
            if (location) {
                nms_printf(NMSML_NORM, "Redirecting to %s\n",
                       location);
                // XXX:proving
                rtsp_open((rtsp_ctrl*)rtsp_th, location);
                ///// XXX: end proving
            } else
                return -nms_printf(NMSML_ERR,
                           "No location string\n");
            // rtsp_th->status=INIT;
        }
    } else if (RTSP_IS_CLIENT_ERROR(res_state)) // (res_state>=400) && (res_state<500)
        nms_printf(NMSML_ERR, "Client error. Reply was: %hu %s\n",
               res_state, reason_phrase);
    else if (RTSP_IS_SERVER_ERROR(res_state)) // res_state>=500
        nms_printf(NMSML_ERR, "Server error. Reply was: %hu %s\n",
               res_state, reason_phrase);
    return -1;
}

int full_msg_rcvd(rtsp_thread * rtsp_th)
{
    struct rtsp_buffer *in_buffer = &(rtsp_th->in_buffer);
    char *back_n;        /* pointer to newline */
    char *head_end;        /* pointer to header end */
    size_t body_len;    

    // is there an interleaved RTP/RTCP packet?
    if ((rtsp_th->transport.type == TCP && rtsp_th->interleaved) && 
        in_buffer->size > 4 && in_buffer->data[0] == '$') {

        if ((body_len = ntohs(*((uint16 *) &(in_buffer->data[2]))) + 4)
            <= in_buffer->size) {
            in_buffer->first_pkt_size = body_len;
            return 1;
        } else {
            return 0;
        }

    }

    if ((head_end = strchr(in_buffer->data, '\n')) == NULL)
        return 0;
    do {
        back_n = head_end;
        if ((head_end = strchr(head_end + 1, '\n')) == NULL)
            return 0;    /* header is not complete */
        if (((head_end - back_n) == 2) && (*(back_n + 1) == '\r'))
            break;
    } while ((head_end - back_n) > 1);    /* here is the end of header */
    while ((*(++head_end) == '\n') || (*head_end == '\r'));    /* seek for first 
                                   valid char after
                                   the empty line */
    if ((body_len = body_exists(in_buffer->data)) == 0) {
        in_buffer->first_pkt_size = head_end - in_buffer->data;
        return 1;    /* header received (no payload) */
    }

    if (strlen(head_end) < body_len)
        return 0;    /* body incomplete */

    in_buffer->first_pkt_size = head_end - in_buffer->data + body_len;
    return 1;        /* full message received */
}

void rtsp_unbusy(rtsp_thread * rtsp_th)
{
    pthread_mutex_lock(&(rtsp_th->comm_mutex));

    rtsp_th->busy = RTSP_READY;
    pthread_cond_signal(&(rtsp_th->cond_busy));

    pthread_mutex_unlock(&(rtsp_th->comm_mutex));
}

int remove_pkt(rtsp_thread * rtsp_th)
{

    char *buff = NULL;
    size_t new_size;

    if ((new_size = rtsp_th->in_buffer.size - rtsp_th->in_buffer.first_pkt_size)) {
        if ((buff =
             (char *) malloc(new_size)) ==
            NULL)
            return nms_printf(NMSML_FATAL,
                      "remove_pkt: Cannot allocate memory! (%d bytes)\n", new_size);

        memcpy(buff,
               rtsp_th->in_buffer.data +
               rtsp_th->in_buffer.first_pkt_size,
               rtsp_th->in_buffer.size -
               rtsp_th->in_buffer.first_pkt_size);
    } else
        buff = NULL;
    free(rtsp_th->in_buffer.data);
    rtsp_th->in_buffer.data = buff;
    rtsp_th->in_buffer.size -= rtsp_th->in_buffer.first_pkt_size;
    rtsp_th->in_buffer.first_pkt_size = 0;
    return 0;
}

int rtsp_recv(rtsp_thread * rtsp_th)
{
    int n = -1, m = 0;
    char buffer[RTSP_BUFFERSIZE];
    nms_rtsp_interleaved *p;
#ifdef HAVE_LIBSCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    memset(buffer, '\0', RTSP_BUFFERSIZE);

    switch(rtsp_th->transport.type) {
    case TCP:
        n = nmst_read(&rtsp_th->transport, buffer, RTSP_BUFFERSIZE, NULL);
        break;
#ifdef HAVE_LIBSCTP
    case SCTP:
        memset(&sinfo, 0, sizeof(sinfo));
        n = nmst_read(&rtsp_th->transport, buffer, RTSP_BUFFERSIZE, &sinfo);
        m = sinfo.sinfo_stream;
        break;
#endif
    default:
        break;
    }
    if (n == 0) {
        return 0;
    }
    if (n < 0) {
        nms_printf(NMSML_ERR, "Could not read from RTSP socket\n");
        return n;
    }
    if (rtsp_th->transport.type == TCP || (rtsp_th->transport.type == SCTP && m==0)) {
        if (((rtsp_th->in_buffer).size) == 0) {
            if (((rtsp_th->in_buffer).data = 
                 (char *) calloc(1, n + 1)) == NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy((rtsp_th->in_buffer).data, buffer, n);
        } else {
            if (((rtsp_th->in_buffer).data =
                 (char *) realloc((rtsp_th->in_buffer).data,
                n + (rtsp_th->in_buffer).size + 1)) ==
                NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy((rtsp_th->in_buffer).data + (rtsp_th->in_buffer).size, buffer, n);
    }
    (rtsp_th->in_buffer).size += n;
    (rtsp_th->in_buffer).data[(rtsp_th->in_buffer).size] = '\0';
    } else /* if (rtsp_th->transport.type == SCTP && m!=0) */ {
#ifdef HAVE_LIBSCTP
        for (p = rtsp_th->interleaved; p && !((p->proto.sctp.rtp_st == m)
            || (p->proto.sctp.rtcp_st == m)); p = p->next);
        if (p) {
            if (p->proto.sctp.rtp_st == m) {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtp_fd);
                send(p->rtp_fd, buffer, n, MSG_EOR);
            } else {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTCP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtcp_fd);
                send(p->rtcp_fd, buffer, n, MSG_EOR);
            }
        }
#endif
    }
    return n;
}

int seturlname(rtsp_thread * rtsp_th, char *urlname)
{
    char *server = NULL, *port = NULL, *path = NULL;

    if (urltokenize(urlname, &server, &port, &path) > 0)
        return 1;
    if (port == NULL) {
        if ((port = (char *) malloc(6)))
            snprintf(port, 6, "%d", RTSP_DEFAULT_PORT);
        else
            return 1;
    }
    nms_printf(NMSML_DBG1, "server %s port %s\n", server, port);

    if ((rtsp_th->urlname = malloc(strlen("rtsp://") + strlen(server) + 1 +
                            strlen(path) + 1)) == NULL)
        return 1;
    strcpy(rtsp_th->urlname, "rtsp://");
    strcat(rtsp_th->urlname, server);
    strcat(rtsp_th->urlname, "/");
    strcat(rtsp_th->urlname, path);
#if 0                // port is already an allocated space => we use this without duplicating string;
    if ((rtsp_th->server_port = (char *) malloc(strlen(port) + 1)) == NULL)
        return 1;
    strcpy(rtsp_th->server_port, port);
#endif
    rtsp_th->server_port = port;

    // free no more useful memory
    free(server);
    free(path);

    return 0;
}

int set_rtsp_media(rtsp_thread * rtsp_th)
{

    rtsp_session *curr_rtsp_s = rtsp_th->rtsp_queue;
    rtsp_medium *curr_rtsp_m = NULL;
    sdp_medium_info *sdp_m;
    sdp_attr *sdp_attr;
    char *tkn, *ch;
    uint8 pt;

    switch (rtsp_th->descr_fmt) {
    case DESCRIPTION_SDP_FORMAT:
        for (sdp_m = curr_rtsp_s->info->media_info_queue; sdp_m;
             sdp_m = sdp_m->next) {
            if (curr_rtsp_m == NULL) {
                /* first medium */
                if ((curr_rtsp_s->media_queue = curr_rtsp_m =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
            } else if (rtsp_th->type == CONTAINER) {
                /* media in the same session */
                if ((curr_rtsp_m->next =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_m->next->rtp_sess;
                curr_rtsp_m = curr_rtsp_m->next;
            } else if (rtsp_th->type == M_ON_DEMAND) {
                /* one medium for each session */
                if ((curr_rtsp_s->next =
                     rtsp_sess_dup(curr_rtsp_s)) == NULL)
                    return 1;
                curr_rtsp_s = curr_rtsp_s->next;
                if ((curr_rtsp_s->media_queue =
                     rtsp_med_create(rtsp_th->transport.fd)) ==
                    NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_s->media_queue->rtp_sess;
                curr_rtsp_m = curr_rtsp_s->media_queue;
            }
            curr_rtsp_m->medium_info = sdp_m;

            // setup rtp format list for current media
            for (tkn = sdp_m->fmts;
                 *tkn && !(!(pt = strtoul(tkn, &ch, 10))
                            && ch == tkn); tkn = ch) {
                switch (sdp_m->media_type) {
                case 'A':
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, AU))
                        return 1;
                    break;
                case 'V':
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, VI))
                        return 1;
                    break;
                default:
                    // not recognized
                    if (rtp_announce_pt
                        (curr_rtsp_m->rtp_sess, pt, NA))
                        return 1;
                    break;
                }
            }

            for (sdp_attr = sdp_m->attr_list; sdp_attr;
                 sdp_attr = sdp_attr->next) {
                if (!strncmpcase(sdp_attr->a, "control", 7)) {
                    tkn = sdp_attr->a + 7;    // 7 == strlen("control")
                    while ((*tkn == ' ') || (*tkn == ':'))    // skip spaces and colon
                        tkn++;
                    curr_rtsp_m->filename = tkn;
                } else
                    if (!strncmpcase(sdp_attr->a, "rtpmap", 6))
                {
                    /* We assume the string in the format:
                     * rtpmap:PaloadType EncodingName/ClockRate[/Channels] */
                    tkn = sdp_attr->a + 6;    // 6 == strlen("rtpmap")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (((pt = (uint8) strtoul(tkn, &tkn, 10)) >= 96)
                        && (pt <= 127)) {
                        while (*tkn == ' ')
                            tkn++;
                        if (!(ch = strchr(tkn, '/'))) {
                            nms_printf(NMSML_WARN,
                                   "Invalid field rtpmap.\n");
                            break;
                        }
                        *ch = '\0';
                        if (rtp_dynpt_reg (curr_rtsp_m->rtp_sess, pt, tkn))
                            return 1;
                        switch (sdp_m->media_type) {
                        case 'A':
                            sscanf(ch + 1, "%u/%c",
                                &curr_rtsp_m->rtp_sess->ptdefs[pt]->rate,
                                &RTP_AUDIO(curr_rtsp_m->rtp_sess->ptdefs[pt])->
                                   channels);
                            break;
                        case 'V':
                            sscanf(ch + 1, "%u",
                                   &curr_rtsp_m->rtp_sess->ptdefs[pt]->rate);
                            break;
                        default:
                            // not recognized
                            break;
                        }
                        *ch = '/';
                        tkn = ++ch;
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                            "Warning: rtpmap attribute is trying to set a"
                            "non-dynamic payload type: not permitted\n");
                    }
                } else
                    if (!strncmpcase(sdp_attr->a, "fmtp", 4)) {
                    /* We assume the string in the format:
                     * fmtp:PaloadType <format specific parameters> */
                    tkn = sdp_attr->a + 4;    // 4 == strlen("fmtp")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if ((pt = (uint8) strtoul(tkn, &tkn, 10)) <= 127) {
                        while (*tkn == ' ')
                            tkn++;
                        rtp_pt_attr_add(curr_rtsp_m->rtp_sess->ptdefs, pt, tkn);
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                            "Warning: fmtp attribute is trying to set an"
                            "out of bounds payload type: not permitted\n");
                    }
                } else if (!strncmpcase(sdp_attr->a, "med", 3)) {    // dirty keyword from old fenice used for dinamic payload change
                    sdp_medium_info m_info;
                    /* We assume the string in the format:
                     * med:sdp-like m= field */
                    tkn = sdp_attr->a + 3;    // 3 == strlen("med")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (sdp_parse_m_descr(&m_info, tkn)) {
                        nms_printf(NMSML_ERR,
                               "malformed a=med: from fenice\n");
                        return 1;
                    }
                    // check if everything is correct
                    if (!(pt = strtoul(m_info.fmts, &ch, 10)) 
                        && ch == m_info.fmts) {
                        nms_printf(NMSML_ERR,
                            "Could not determine pt value in a=med: string"
                            " from fenice\n");
                        return 1;
                    }
                    switch (sdp_m->media_type) {
                    case 'A':
                        if (strncmpcase
                            (tkn, "audio ", 6)) {
                            nms_printf(NMSML_ERR,
                                "a=med; attribute defined a different media"
                                " type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             AU))
                            return 1;
                        break;
                    case 'V':
                        if (strncmpcase
                            (tkn, "video ", 6)) {
                            nms_printf(NMSML_ERR,
                                "a=med; attribute defined a different media"
                                " type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             VI))
                            return 1;
                        break;
                    default:
                        // not recognized
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             NA))
                            return 1;
                        break;
                    }
                }
            }
        }
        break;
    case DESCRIPTION_MH_FORMAT:
        /* not yet implemented */
        // break;
    default:
        nms_printf(NMSML_ERR, "Unknown decription format.\n");
        return 1;
        break;
    }

    return 0;
}
