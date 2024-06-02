/* generr.c  -  Utility for returning a requested error code

Copyright (C) 1999  Wolf Bergenheim

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

History
2024-05-24 - creation - WB

*/
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int n;
    randomize();
    if (argc < 2) {
	return 0;
    } else {
	if (strcmp(argv[1], "-r") == 0) {
	    n = atoi(argv[2]);
	    return random(n+1);
	}
	return atoi(argv[1]);
    }
}
