/* * 
 *  ./E-gui/src/main.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:16 $
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

/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "interface.h"
#include "support.h"

#include <ourhdr.h>
#include <nemesi/main.h>
#include <nemesi/utils.h>
#include <nemesi/etui.h>
#include <nemesi/rtpptdefs.h>

int (*decoders[128])(char *, int, uint8 *(*)());

struct rtp_pt_def rtp_pt_defs[128] = {
/*   0 -   3 */	{ "PCMU", AU, 8000, 1},		{ "res", AU, 0, 0},		{ "G726-32", AU, 8000, 1},	{ "GSM", AU, 8000, 1},
/*   4 -   7 */	{ "res", AU, 0, 0},		{ "DVI4", AU, 8000, 1},		{ "DVI4", AU, 16000, 1},	{ "LPC", AU, 8000, 1},
/*   8 -  11 */	{ "PCMA", AU, 8000, 1},		{ "G722", AU, 8000, 1},		{ "L16", AU, 44100, 2},		{ "L16", AU, 44100,1},
/*  12 -  15 */	{ "QCELP", AU, 8000, 1},	{ "res", AU, 0, 0},		{ "MPA", AU, 90000, 0},		{ "G728", AU, 8000, 1},
/*  16 -  19 */	{ "DVI4", AU, 11025, 1},	{ "DVI4", AU, 22050, 1},	{ "G729", AU, 8000, 1},		{ "res", AU, 0, 0},
/*  20 -  23 */	{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},
/*  24 -  27 */	{ "unas", VI, 0, 0},		{ "CelB", VI, 90000, 0},	{ "JPEG", VI, 90000, 0},	{ "unas", VI, 0, 0},
/*  28 -  31 */	{ "nv", VI, 90000, 0},		{ "unas", VI, 0, 0},		{ "unas", VI, 0, 0},		{ "H261", VI, 90000, 0},
/*  32 -  35 */	{ "MPV", VI, 90000, 0},		{ "res", VI, 0, 0},		{ "res", VI, 0, 0},		{ "unas", NA, 0, 0},
/*  36 -  39 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  40 -  43 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  44 -  47 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  48 -  51 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  52 -  55 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  56 -  59 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  60 -  63 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  64 -  67 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  68 -  71 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  72 -  75 */	{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},
/*  76 -  79 */	{ "res", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  80 -  83 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  84 -  87 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  88 -  91 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  92 -  95 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  96 -  99 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 100 - 103 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 104 - 107 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 108 - 111 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 112 - 115 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 116 - 119 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 120 - 123 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 124 - 127 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0}
};

int
main (int argc, char *argv[])
{
	gint tag;
	GtkWidget *app1;
	GnomeAppBar *appbar;
	
	struct RTSP_args *rtsp_args;
	char *urlname = NULL;
	char statusstr[27];
	pthread_t rtsp_tid;
	void *ret;

#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE);
#endif

	gnome_init ("neme-gui", VERSION, argc, argv);

	/*
	* The following code was added by Glade to create one of each component
	* (except popup menus), just so that you see something after building
	* the project. Delete any components that you don't want shown initially.
	*/
	app1 = create_app1 ();
	get_egui(app1);
	gtk_widget_hide(lookup_widget(app1,"button8"));
	gtk_widget_hide(lookup_widget(app1,"button10"));
	gtk_widget_show (app1);
	
	/* crea e inizializza il thread RTSP */
	if((rtsp_tid=init_main(&rtsp_args)) == -1)
		return 1;

  	/*  */
	tag=gdk_input_add(UIINPUT_FILENO, GDK_INPUT_READ, (GdkInputFunction) txtprintf, \
			(gpointer) lookup_widget(app1, "text1"));

	/* inizializzo la status bar */
	appbar=(GnomeAppBar *)lookup_widget(app1, "appbar1");
	sprintf(statusstr,"NeMeSI status: %s", statustostr(rtsp_args->rtsp_th->status));
	gtk_label_set_text((GtkLabel *)(appbar->status), statusstr);

	/* scansione della riga di comando */
	if (parse_cl(argc, argv, &urlname) > 0)
		return 1;
	uiprintf("\nWelcome! This is %s - %s -- version %s (%s)\n", PROG_NAME, \
			PROG_DESCR, VERSION, VERSION_NAME);
	if (urlname != NULL) {
		uiprintf("Connect: Please wait, opening \"%s\"\n", urlname);
		send_open(rtsp_args, urlname);
		gui_throbber();
	}

	gtk_main ();
	gdk_input_remove(tag);
	
	/* THREAD CANCEL */	
	fprintf(stderr, "Sending cancel signal to all threads\n");
	if(rtsp_tid > 0){
		if (pthread_cancel(rtsp_tid) != 0)
			fprintf(stderr, "Error while sending cancelation to RTSP Thread.\n");
		else
			pthread_join(rtsp_tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			fprintf(stderr, "Warning! RTSP Thread joined, but  not canceled!\n");
	} else
		fprintf(stderr, "Cannot send cancel signal to RTSP Thread\n");

	fprintf(stderr, "\nBye bye!\n\n");

	return 0;
}

