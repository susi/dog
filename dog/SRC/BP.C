/*

BP.C   - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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
15.08.00 - Extracted from BAT.C, changed t & f to beWORD instead of BYTE - WB

*/

#ifdef port
#include "dog.h"
#endif

void do_bp(BYTE n)
{
	WORD t,f;

#ifdef bat_debug
	BYTE b;
printf("n = %u\n",n);
for(b=0;b<n;b++)
printf("do_bp:arg[%d]=(%s)\n",b,arg[b]);
#endif

    switch (n) {
        case 0:
        case 1:
            putchar('\x07');
            break;
        case 2:
            if (arg[1][0] == '-') {
                switch (arg[1][1]) {
                    case 'h':
                    case 'H':
                    case '?':
                        printf("bp [frec time]\n");
                        break;
                    default :
                        printf("Incorrect switch %s\n",arg[1]);
                        break;
                }
            }
            break;
        case 3:
    		f = atoi(arg[1]);
    		t = atoi(arg[2]);
#ifdef bp_debug
printf("tone: %uHz %ums\n",f,t);
#endif
    		if(t == 0)
    			break;
    		if(f == 0) {
    			delay(t);
    			break;
    		}
            sound(f);
            delay(t);
    		nosound();
            break;
        default:
            puts("Invalid number of arguments.");
    }        
    return;
}

