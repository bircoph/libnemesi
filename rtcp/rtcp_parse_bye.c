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
#include <nemesi/comm.h>
#include <nemesi/rtspinternals.h>

int rtcp_parse_bye(rtp_ssrc * ssrc, rtcp_pkt * pkt)
{
    rtsp_thread * rtsp_t;
    int i;
    for (i = 0; i < pkt->common.count; i++)
        nms_printf(NMSML_DBG1, "Received BYE from SSRC: %u\n",
            pkt->r.bye.src[i]);

    rtsp_t = ssrc->rtp_sess->owner;
    rtsp_t->rtp_th->run = 0;
    return 0;
}
