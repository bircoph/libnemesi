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

#include <nemesi/diskwriter.h>
#include <fcntl.h>
#include <unistd.h>

int diskwriter_init(void)
{
	int fd;
	struct disk_buff *disk_buffer;

	if ( (fd=open( DEFAULT_FILENAME, O_EXCL)) > 0 ) {
		close(fd);
	} else if ( (fd=creat( DEFAULT_FILENAME, 00644)) > 0 ) {
		close(fd);
		unlink( DEFAULT_FILENAME );
	} else {
		fprintf(stderr, "\nError initializzing Disk Writer Module: you have not write permission\n");
		return 1;
	}
	
	if ( (disk_buffer=db_init()) == NULL ) {
		fprintf(stderr, "\nFailed while initializing Disk Writer Buffer\n");
		return 1;
	}

	global_disk_buffer=disk_buffer;
	
	return 0;
}

