/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef __VERSION_H
#define __VERSION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEMESI_SVN
// #define SVNREV "$Rev$"
#include <svnrev.h>
#define VERSION_NAME "Yet another bug fixed (svn version)"
#else
#define VERSION_NAME "In itinere (change it on release)"
#endif
#define PROG_NAME "NeMeSI"
#define PROG_DESCR "NEtwork MEdia Streamer I"
#endif
