/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

#include <nemesi/rtsp.h>
#include <nemesi/methods.h>

int send_teardown_request(struct RTSP_Thread *rtsp_th)
{

	char b[256];
	struct RTSP_Session *rtsp_sess;
	struct RTSP_Medium *rtsp_med;

	memset(b, 0, 256);

	if ( get_curr_sess(NULL, &rtsp_sess, &rtsp_med))
		return 1;

	if ( rtsp_sess->content_base != NULL)
		sprintf(b, "%s %s/%s %s\n", CLOSE_TKN, rtsp_sess->content_base, rtsp_med->filename, RTSP_VER);
	else
		sprintf(b, "%s %s %s\n", CLOSE_TKN, rtsp_med->filename, RTSP_VER);

	sprintf(b + strlen(b), "CSeq: %d\n", ++(rtsp_sess->CSeq));
	if (rtsp_sess->Session_ID != 0)	/*must add session ID? */
		sprintf(b + strlen(b), "Session: %llu\n", rtsp_sess->Session_ID);
	strcat(b, "\r\n");

	if (!tcp_write(rtsp_th->fd, b, strlen(b))) {
		uiprintf("Cannot send TEARDOWN request...\n");
		return 1;
	}

	sprintf(rtsp_th->waiting_for, "%d.%d", RTSP_CLOSE_RESPONSE, rtsp_sess->CSeq);

	return 0;
}

