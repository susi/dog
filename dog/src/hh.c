/*

HH.C   - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

Copyright (C) 1999,2000 Wolf Bergenheim

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

History
18.03.00 - Extracted from DOG.C - WB
06.04.00 - Started - WB
2002-03-01 - Internal and external commands are now sepatate
2024-05-11 - Building as a module. - WB
2024-10-14 - Adding PP as an external command
*/

#include "dog.h"

void do_hh(BYTE n)
{
    BYTE i, hhh[7], na;

    puts("Cannot find HH.COM");
    if(n == 1) {
        printf("The internal commands are:\n");
        for(i=0;i<_NCOMS;i++) {
            printf("%s %s\n",commands[i],command_des[i]);
        }
        printf("The external commands are:\n");
        for(i=0;i<_NECOMS;i++) {
            printf("%s %s\n",ext_commands[i],ext_command_des[i]);
        }
    }
    else {
	for(i=0;i<_NCOMS;i++) {
	    if(strnicmp(arg[1], commands[i], 2) == 0) {
		printf("Internal command %s: %s\n", commands[i], command_des[i]);
		return;
	    }
	}
	for(i=0;i<_NCOMS;i++) {
	    if(strnicmp(arg[1], ext_commands[i], 2) == 0) {
		printf("External command %s: %s\n", ext_commands[i], ext_command_des[i]);
		return;
	    }
	}
	printf("Unknown command %s\n", arg[1]);
    }
}
