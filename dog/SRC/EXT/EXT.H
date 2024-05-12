/*
UTIL.H - Header for DOG utilities.

Copyright (C) 1999, 2000  Wolf Bergenheim

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Developers:
Wolf Bergenheim (WB)

*/


#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <ctype.h>
#include <alloc.h>
#include <io.h>
#include <process.h>
#include <errno.h>

/* See ..\debug.h for all the possible debugs. ext.h includes it
   _DEBUG_ALL_ turns all all debugging and durns off optimization */
#include "debug.h"

#define BYTE unsigned char
#define WORD unsigned int
#define DWORD unsigned long

/* we want to use the ones in dos.h. not in dir.h */
#if 0
#define findfirst(A,B,C) _dos_findfirst(A,C,B)
#define findnext _dos_findnext
#define ffblk       find_t
#define ff_attrib   attrib
#define ff_ftime    wr_time
#define ff_fdate    wr_date
#define ff_fsize    size
#define ff_name	    name
#endif

#ifdef MK_FP
#undef MK_FP
#endif
#define MK_FP(seg,ofs) ((void far*)(((unsigned long)(seg) << 16) | (ofs)))

#ifndef FA_NORMAL
#define FA_NORMAL 0x0
#endif

#define EVER ;;


#define FNF 18
#define PATH_SIZE 80

#define _BS 0x8;
#define _TAB 0x9;
#define _SPC 0x32;

#define FLAG_UNSET 0
#define FLAG_SET 1
