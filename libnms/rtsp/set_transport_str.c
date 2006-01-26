/* * 
 *  $Id:set_transport_str.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/utils.h>

int set_transport_str(struct rtp_session *rtp_sess, char **str)
{
	char buff[256];
	char addr[128];		/* Unix domain is largest */
	in_port_t ports[2];

	memset(buff, 0, sizeof(buff));
	
	rtp_transport_get(rtp_sess, RTP_TRANSPORT_SPEC, buff, sizeof(buff));
	*(buff+strlen(buff))=';';
	if (rtp_transport_get_delivery(rtp_sess) == multicast)
		sprintf(buff+strlen(buff), "multicast;");
	else
		sprintf(buff+strlen(buff), "unicast;");
	if ( rtp_transport_get(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, addr, sizeof(addr)) == RTP_TRANSPORT_SET )
		sprintf(buff+strlen(buff), "destination=%s;", addr);
	if ( rtp_transport_get(rtp_sess, RTP_TRANSPORT_SRCADDRSTR, addr, sizeof(addr)) == RTP_TRANSPORT_SET )
		sprintf(buff+strlen(buff), "source=%s;", addr);
	if ( rtp_transport_get_layers(rtp_sess) )
		sprintf(buff+strlen(buff), "layers=%d;", rtp_transport_get_layers(rtp_sess));
	if (rtp_transport_get_mode(rtp_sess) == record)
		sprintf(buff+strlen(buff), "mode=record;");
	else
		sprintf(buff+strlen(buff), "mode=play;");
	if (rtp_transport_get_append(rtp_sess))
		sprintf(buff+strlen(buff), "append;");
	if (rtp_transport_get_ttl(rtp_sess))
		sprintf(buff+strlen(buff), "ttl=%d;", rtp_transport_get_ttl(rtp_sess));
	if ( rtp_transport_get_mcsports(rtp_sess, ports) == RTP_TRANSPORT_SET )
		sprintf(buff+strlen(buff), "port=%d-%d;", (int)ports[0], (int)ports[1]);
	if ( rtp_transport_get_cliports(rtp_sess, ports) == RTP_TRANSPORT_SET )
		sprintf(buff+strlen(buff), "client_port=%d-%d;", (int)ports[0], (int)ports[1]);
	if(rtp_transport_get_ssrc(rtp_sess))
		sprintf(buff+strlen(buff), "ssrc=%u;", rtp_transport_get_ssrc(rtp_sess));
	
	/* eliminiamo l'ultimo ; */
	/* drop last ';' */
	*(buff+strlen(buff)-1)='\0';

	if (!(*str=strdup(buff)))
		return nms_printf(NMSML_FATAL, "set_transport_str: Could not duplicate string!\n");

	return 0;
}