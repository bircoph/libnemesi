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

#include <stdio.h>
#include <unistd.h>

#include <nemesi/types.h>
#include <nemesi/output.h>

#include <nemesi/plugin.h>

plugin_init("L16", 106)

#define CHANNELS 2
#define FACTOR_OVERSAMPLE 5
#define ELEVEN 11

int decode(char *data, int len, nms_output *outc)
{
	nms_au_fnc *funcs = outc->audio->functions;
	int i, j, c;
	/*
	char outbuff[len*ELEVEN+len/40];
	char *p=outbuff;
	*/
	uint8 *outbuff, *p;
	uint32 req_len;
	uint8 adjust=0, adjust40=0;

	req_len=len*ELEVEN+len/40;
	if ( (outbuff=p=funcs->get_buff(req_len)) == NULL )
		return 1;

	/* Endianess, Oversample to 44100Hz -- very very quick, raw and dirt */
	for(i=0; i < len; i+=2){
		for(j=0; j < FACTOR_OVERSAMPLE+adjust; j++){
#ifndef WORDS_BIGENDIAN
			for(c=0; c < CHANNELS; c++) {
				*(p++)=data[i+1];
				*(p++)=data[i];
			}
#else
			for(c=0; c < CHANNELS; c++) {
				*(p++)=data[i];
				*(p++)=data[i+1];
			}
#endif
		}
		if ( !(i%40) )
			adjust40=1;
		adjust=!adjust;
		if (adjust40 && !adjust){
			adjust=1;
			adjust40=0;
		}
	}
	funcs->play_buff(outbuff, req_len, outc->elapsed);
/*
	if ( write(audio_fd, (char *)outbuff, len*ELEVEN+len/40) == -1 ){
		fprintf(stderr,"\n\n\nCould not write on Audio Board\n\n\n");
		return 1;
	}
*/	
	return 0;
}

int get_plugin_pt(void)
{
	return 106;
}
