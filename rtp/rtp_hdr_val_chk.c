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

#include <nemesi/rtp.h>

int rtp_hdr_val_chk(rtp_pkt *pkt, int len)
{
	if ( pkt->ver != RTP_VERSION ){
		uiprintf("RTP Header not valid: mismatching version number!");
		uiprintf(BLANK_LINE);
		return 1;
	}
	if ( (pkt->pt>=200) && (pkt->pt<=204) ){
		uiprintf("RTP Header not valid: mismatching payload type!");
		uiprintf(BLANK_LINE);
		return 1;
	}
	if ( (pkt->pad) && ( *(((uint8 *)pkt)+len-1) > (len - ((uint8 *)(pkt->data)-(uint8 *)pkt)) )){
		uiprintf("RTP Header not valid: mismatching lenght!");
		uiprintf(BLANK_LINE);
		return 1;
	}
	if ( (pkt->cc) && ( pkt->cc > (len - ((uint8 *)(pkt->data)-(uint8 *)pkt))-((*(((uint8 *)pkt)+len-1)) * pkt->pad) )){
		uiprintf("RTP Header not valid: mismatching CSRC count!");
		uiprintf(BLANK_LINE);
		return 1;
	}
	
	return 0;
}
