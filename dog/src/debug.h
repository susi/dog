/*
DEBUG.H   -  DEBUG definitions

Copyright (C) 2024  Wolf Bergenheim

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
*/

#ifndef __DOG_DEBUG__
#define __DOG_DEBUG__

#ifdef _ENV_DEBUG_
#define DOG_DEBUG
#define EXE_DEBUG
#define ENV_DEBUG
#endif

#ifdef _DOG_DEBUG_
#define DOG_DEBUG
#endif

#ifdef _BAT_DEBUG_
#define BAT_DEBUG
#endif

#ifdef _DEBUG_ALL_

#define DOG_DEBUG
#define EXE_DEBUG
#define BAT_DEBUG
#define B_DEBUG
#define LIST_DEBUG
#define PARSE_DEBUG
#define PROMPT_DEBUG
#define ENV_DEBUG

#define DO_DEBUG
#define LS_DEBUG
#define CA_DEBUG
#define CD_DEBUG
#define EH_DEBUG
#define CA_DEBUG
#define XX_DEBUG

/* EXT specific */
#define BP_DEBUG
#define EXT_DEBUG
#define LS_DEBUG
#define MV_DEBUG
#define RM_DEBUG
#define SZ_DEBUG
#define TP_DEBUG

#endif /* _DEBUG_ALL_  */
#endif /* __DOG_DEBUG__  */
