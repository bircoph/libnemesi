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

#include <unistd.h>
#include <getopt.h>

#include <nemesi/etui.h>

int parse_main_cl(int argc, char **argv)
{
	int ret = 0;
	char usage = 0; // printf usage at the end of function
	// getopt variables
	int ch;
	int option_index = 0;
	struct option long_options[] = { CL_LONG_OPTIONS, {0, 0, 0, 0} };
	long i; // index
	long v; // verbosity
	char *v_err;

	opterr = 0;
	optind = 1;
	while (((ch = getopt_long(argc, argv, CL_OPTIONS, long_options, &option_index)) != -1))
		switch (ch) {
		case 'h':	/* help */
			usage = 1;
			ret = 1; // 2 => print usage() at the end of function
			// return 1;
			break;
		case 'V':	/* version */
			version();
			ret = 1;
			// return 1;
			break;
		case 'v':	/* verbose */
			//! salvo in v il livello di "verbosity" corrente
			v = nmsprintf(-1, NULL);
			if (optarg) {
				if (*optarg == 'v') {
					for(i=0;optarg[i]=='v'; i++);
					if (optarg[i]!='\0') {
						nmserror("Invalid argument to verbose\n");
						usage = 1;
						ret = -1;
					}
					v += i+1;
				} else if ( (isdigit(*optarg)) ) {
					v = strtol(optarg, &v_err, 10);
					if ( (*v_err) ) {
						nmserror("Invalid argument to \"verbose\" option 1\n");
						usage = 1;
						ret = -1;
					}
				} else
					return -nmserror("Invalid argument to \"verbose\" option 2\n");
			} else if ( (optind<argc) ) {
				i = strtol(argv[optind], &v_err, 10);
				if ( !(*v_err) ) {
					v = i;
					optind++;
				} else
					v++;
			} else
				v++;
			if ( v != nmsprintf(-1, NULL) )
				nmsprintf(v, NULL);
				// nmsprintf(0, "Verbosity level set to %d\n", nmsprintf(v, NULL));
			break;
		case 1:
			nmsprintf(3, "Audio module chosen\n");
			break;
		case '?':
			if (isprint(optopt))
				nmserror("Unknown option `-%c'.\n", optopt);
			else
				nmserror("Unknown option character `\\x%x'.\n", optopt);
			// ch = 'h';
			usage = 1;
			ret = -1;
			// return -1;
		default:
			break;
		}

	if ( usage )
		usage();
	if ( ret < 0 )
		nmserror ("Error parsing command line... exit\n");
	return ret;
}