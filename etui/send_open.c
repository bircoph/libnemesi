/* * 
 *  ./etui/send_open.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#include <nemesi/etui.h>

int send_open(struct RTSP_args *rtsp_args, char *urlname)
{

	if (seturlname(rtsp_args, urlname) > 0)
		return 1;
	pthread_mutex_lock(&(rtsp_args->comm_mutex));
		rtsp_args->comm->opcode = OPEN;
		write(rtsp_args->pipefd[1], "o", 1);
		rtsp_args->rtsp_th->busy=1;
	pthread_mutex_unlock(&(rtsp_args->comm_mutex));
	
	return 0;
}
