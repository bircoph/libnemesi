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

#include <stdlib.h>
#include <string.h>

//! with this define we declare global variables contained in video.h
#define NMS_GLOBAL_OUTPUT
#include <nemesi/output.h>

#include <nemesi/main.h>
#include <nemesi/preferences.h>
#include <nemesi/comm.h>

int output_init(NMSOutputHints *hints)
{
	if (!(nmsoutc = malloc(sizeof(NMSOutput))))
		nmserror("Could not alloc output struct");

	nmsoutc->elapsed=0;
	nmsoutc->sysbuff_ms = (hints && hints->sysbuff_ms) ? hints->sysbuff_ms : DEF_SYSBUFF_MS;

	nmsprintf(1, SEPARATOR);
	// AUDIO MODULE INIT
	// if ((nmsoutc->audio=audio_init("sdl")) == NULL) {
	if (hints && hints->audio && !strcmp(hints->audio, "noaudio")) {
		nmsprintf(1, "No Audio\n");
		nmsoutc->audio = NULL;
	} else if ((nmsoutc->audio=audio_init(hints ? hints->audio : NULL, nmsoutc->sysbuff_ms)) == NULL) {
		nmserror("Audio module not available");
		// fprintf(stderr, "Audio module not available: setting \"output\" to \"disk\"\n");
		// rem_avail_pref("output card");
		// edit_pref("output disk");
	}

	nmsprintf(1, SEPARATOR);
	// VIDEO MODULE INIT
	if (hints && hints->video && !strcmp(hints->video, "novideo")) {
		nmsprintf(1, "No Video\n");
		nmsoutc->video = NULL;
	} else if ((nmsoutc->video=video_preinit(hints ? hints->video : NULL, nmsoutc->sysbuff_ms)) == NULL) {
		if (!nmsoutc->audio) {
			nmserror("Video module not available: setting \"output\" to \"null\"");
			rem_avail_pref("output card");
			// edit_pref("output disk");
		} else
			nmserror("Video module not available");
	}

	nmsprintf(1, SEPARATOR);
	// DISKWRITER MODULE INIT
	if (hints && hints->diskwriter && !strcmp(hints->diskwriter, "nodisk")) {
		nmsprintf(1, "No DiskWriter\n");
		nmsoutc->diskwriter = NULL;
	} else if ( (nmsoutc->diskwriter=diskwriter_init(hints ? hints->diskwriter : NULL)) == NULL ) {
		fprintf(stderr, "Disk Writer module not available\n");
		rem_avail_pref("output disk");
		if ( strcmp("card", get_pref("output")) )
			return nmserror("No output device available\n Cannot continue");
	} else
		nmsprintf(1, "Diskwriter succesfully initialized\n");
	nmsprintf(1, SEPARATOR);

	return 0;
}

