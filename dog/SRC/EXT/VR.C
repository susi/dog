/*

VR.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

TODO: Fix recoginion of DR-DOS

History
18.03.00 - Extracted from DOG.C - WB
06.04.00 - Added DR-DOS detection. - WB
02.08.00 - Modified DR-DOS detection to detect DR-DOS regardles of version
           returned in int 21h/30h - WB

**************************************************************************/

#include "ext.h"


int main(void)
{
    WORD w,DR_vr;
    BYTE b,DOS_ma=3,DOS_mi=30,DOS_OEM=0xFD;
	  BYTE DOG_ma=0,DOG_mi=0,DOG_re=0x0b;
	
		asm MOV ax,01h
		asm INT 0d0h
		asm MOV bh,ah
		asm SHR bh,1
		asm SHR bh,1
		asm SHR bh,1
		asm SHR bh,1
		asm MOV DOG_ma,bh
		asm AND ah,0fh
		asm MOV DOG_mi,ah
		asm MOV DOG_re,al
		
    asm MOV ax,3000h
    asm INT 21h
		asm MOV DOS_ma ,al
		asm MOV DOS_mi ,ah
		asm MOV DOS_OEM,bh
		asm MOV b,bl
		asm MOV w,cx
		asm MOV ax, 4452h
    asm STC
    asm INT 21h
    asm JC vr_NODR
    asm cmp ax, 4452h
    asm JE vr_NODR
		asm MOV DR_vr,ax

    
    printf("DR-DOS Product 0x%x\n\tProviding DOS %u.%u interface\nDOG version %u.%u.%02x\n",DR_vr,DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
    return 0;

    vr_NODR:
    switch(DOS_ma) {  /*versions 1,2 & 4 NOT supported */
        case 3 :
            printf("DOS version %u.%u\nDOG version %u.%u.%02x",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
            break;
        case 5 :
        case 6 :
        case 7 :
			       asm MOV ax,3306h
             asm INT 21h
             asm CMP bh,50
             asm JE vr_NT
             asm JMP vr_DOS
        vr_NT:
            printf("DOS Command Prompt under Windows NT\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
            break;
        vr_DOS:
            switch(DOS_OEM) {
                case  0x00:
                    if((DOS_mi ==0) && (DOS_ma==6)) DOS_mi = 1;
                    printf("PC-DOS version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
                    break;
                case  0xEE:
                    printf("DR DOS version 5.0 OR 6.0 (pretends to be MS-DOS %u.%u)\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
                    break;
                case  0xEF:
                    printf("Novell DOS 7\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
                    break;
                case  0xFD:
                    printf("FreeDOS version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
                    if(b == 0xff)
                        printf("FreeDOS Kernel (build 1993 or prior)\n");
                    else
                        printf("FreeDOS Kernel version %u.%u.%u\n",b,w >> 8,w & 0xff);
                    break;
                case  0xFF:
                    printf("M$-DOS version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
                    break;
                default :
                    printf("DOS version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
            }
            break;
/*
        case 7 :
            if (DOS_mi==10)
                printf("Microsoft Windows 95 OSR2\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
            else
                printf("Microsoft Windows 95\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
            break;
*/
        case 10 :
            printf("OS/2 version 1.%u\nDOG version %u.%u.%02x\n",DOS_mi/10,DOG_ma,DOG_mi,DOG_re);
            break;
        case 20 :
            if (DOS_mi < 30)
                printf("OS/2 version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma/10,DOS_mi/10,DOG_ma,DOG_mi,DOG_re);
            else if(DOS_mi==30)
                printf("OS/2 Warp 3.0 virtual DOS machine\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
            else if(DOS_mi==40)
                printf("OS/2 Warp 4.0 virtual DOS machine\nDOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
            break;
        default :
            printf("Unknown DOS version %u.%u\nDOG version %u.%u.%02x\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi,DOG_re);
    }

	 return 0;

}

