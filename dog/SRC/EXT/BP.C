/*

BP.C   - DOG - Alternate command processor for freeDOS

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

#include "ext.h"

WORD t,f;


int init(int n, char *arg[]);
void do_bp(void);

int main(int nargs, char *args[])
{
	BYTE r;

	r = init(nargs, args);
	if (r == 0)
		do_bp();

	return r;
}


/*
 * return -1 Error, = OK, 1 OK, but stop.
 */
int init(int n, char *arg[])
{

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
		  return 1;
		case 2:
			if (arg[1][0] == '-') {
				 switch (arg[1][1]) {
					case 'h':
					case 'H':
					case '?':
						printf("usage: bp [frec time]\n");
						return 1;
					default :
						printf("Incorrect switch %s\n",arg[1]);
				 }
			}
			else {
				 printf("usage: bp [frec time]\n");
			}
			return -1;
		case 3:
			/* TODO: This should be done in a more elegant way */
			f = atoi(arg[1]);
			t = atoi(arg[2]);
		return 0;
		default:
			puts("Stunned! Too many parameters, ignoring parameter owerflow.");
			f = atoi(arg[1]);
			t = atoi(arg[2]);
			return 0;
	 }
}

void do_bp(void)
{

#ifdef bp_debug
	printf("tone: %uHz %ums\n",f,t);
#endif
	if(t == 0)
		 ;
	 else if(f == 0) {
			delay(t);
	 }
	 else {
			sound(f);
			delay(t);
			nosound();
	  }

	return;
}

