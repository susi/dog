/*

EH.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

**************************************************************************/

void do_eh( BYTE n)
{
    BYTE i,j;

    for(i=1;i<n;i++) {
        for(j=0;j < strlen(arg[i]);j++) {
            if(arg[i][j] == '%') {
                switch(arg[i][++j]) {
                    case '%' :
                        putchar('%');
                        break;
                    case  '_':
                        putchar(' ');
                        break;
                    case  'b':
                        putchar('|');
                        break;
                    case  'e':
                        putchar('\x1b');
                        break;
                    case  'g':
                        putchar('>');
                        break;
                    case  'h':
                        putchar('\b');
                        break;
                    case  'l':
                        putchar('<');
                        break;
                    case  'n':
                        putchar('\n');
                        break;
                    case  'r':
                        putchar('\r');
                        break;
                    case  't':
                        putchar('\t');
                        break;
                    default :
                        putchar(arg[i][j]);

                }
            }
            else
                putchar(arg[i][j]);

        }
        printf(" ");
    }
    printf("\n");
}


