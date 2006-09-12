/* * 
 *  $Id:rtsp.c 267 2006-01-12 17:19:45Z shawill $
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

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <nemesi/rtsp.h>
#include <nemesi/version.h>

int (*cmd[COMMAND_NUM]) (rtsp_thread *, ...);
int (*state_machine[STATES_NUM]) (rtsp_thread *, short);

/**
* Thread rtsp.
* Waits for commands and handle them
* It is modeled as a state machine
* */

void *rtsp(void *rtsp_thrd)
{
	rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_thrd;
	struct command *comm = rtsp_th->comm;
	int command_fd = rtsp_th->pipefd[0];
	fd_set readset;
	char ch[1];
	int n;
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_cleanup_push(rtsp_clean, rtsp_thrd);

	while (1) {
		FD_ZERO(&readset);

		FD_SET(command_fd, &readset);
		if (nmst_is_active(&rtsp_th->transport))
			FD_SET(rtsp_th->transport.fd, &readset);
		if (select(max(rtsp_th->transport.fd, command_fd) + 1 , &readset, NULL, NULL, NULL) < 0){
			nms_printf(NMSML_FATAL, "(%s) %s\n", PROG_NAME, strerror(errno));
			pthread_exit(NULL);
		}
		if (nmst_is_active(&rtsp_th->transport))
			if (FD_ISSET(rtsp_th->transport.fd, &readset)) {
				if ((n=rtsp_recv(rtsp_th)) < 0)
					pthread_exit(NULL);
				else if (n > 0){
					if (full_msg_rcvd(rtsp_th))
						if (handle_rtsp_pkt(rtsp_th)) {
							// nms_printf(NMSML_ERR, "\nError!\n");
							rtsp_reinit(rtsp_th);
						}
				} else {
					nms_printf(NMSML_ERR, "Server died prematurely!\n");
					rtsp_reinit(rtsp_th);
					nms_printf(NMSML_NORM, "Session closed.\n");
				}
			}
		if (FD_ISSET(command_fd, &readset)) {
			pthread_mutex_lock(&(rtsp_th->comm_mutex));
			read(command_fd, ch, 1);
			if (cmd[comm->opcode] (rtsp_th, comm->arg)) {
				nms_printf(NMSML_DBG3, "Error handling user command.\n\n");
				rtsp_th->busy = 0;
			}
			rtsp_th->comm->opcode = NONE;
			pthread_mutex_unlock(&(rtsp_th->comm_mutex));
		}
	}
	
	pthread_cleanup_pop(1);
/*	return NULL; */
}
