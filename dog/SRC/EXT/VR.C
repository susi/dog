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
02.01.02 - Added code for detecting DOSemu, fixed FreeDOS kernel version - WB

**************************************************************************/

#include "ext.h"

#define DOSEMU_BIOS_DATE "02/25/93"
#define DOSEMU_MAGIC "$DOSEMU$"
#define DOSEMU_BIOS_DATE_LOCATION 0xF000FFF5
#define DOSEMU_MAGIC_LOCATION 0xF000FFE0

struct dosemu_detect {
		union {
				char magic_[8];
				unsigned long magic[2];
		}u;
        union {
        unsigned char ver[4];
		    unsigned long version;
        }v;
};


int main(void)
{
  WORD w,DR_vr;
	BYTE b,DOS_ma=3,DOS_mi=30,DOS_OEM=0xFD;
	BYTE DOG_ma=0,DOG_mi=0,DOG_re=0x0b,dogvr[161];
	struct dosemu_detect far* given = (void far*) DOSEMU_MAGIC_LOCATION;
	struct dosemu_detect expect = {DOSEMU_MAGIC};

		/* test for dosemu */

	if ((expect.u.magic[0] == given->u.magic[0]) && (expect.u.magic[0] == given->u.magic[0]))
		printf("Running under Dosemu, version %d.%d.%d.%d\n",given->v.ver[3],given->v.ver[2],given->v.ver[1],given->v.ver[0]);

	/* check for DOG int d0 */

	asm nop
	asm nop
	asm nop
	asm nop
	asm nop
	asm MOV ax,35d0h
	asm int 21h
	asm push es
	asm pop ax
	asm cmp ax,0000h
	asm jz no_DOG
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
  sprintf(dogvr,"DOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);

	if(0) {
		no_DOG:
		sprintf(dogvr,"DOG not detected.\n\nYou can get DOG from http://dog.sf.net/\n");
	}
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


	printf("DR-DOS Product 0x%x\n\tProviding DOS %u.%u interface\n%s",DR_vr,DOS_ma,DOS_mi,dogvr);
  	return 0;

  vr_NODR:
	switch(DOS_ma) {  /*versions 1,2 & 4 NOT supported */
		 case 3 :
			printf("DOS version %u.%u\nDOG version %u.%u.%02x",DOS_ma,DOS_mi,dogvr);
			break;
		case 5 :
		case 6 :
			asm MOV ax,3306h
			asm INT 21h
			asm CMP bh,50
			asm JE vr_NT
			asm JMP vr_DOS
		vr_NT:
			printf("DOS Command Prompt under Windows NT\n%s",dogvr);
			break;
		vr_DOS:
			if (DOS_OEM ==	0x00) {
				if((DOS_mi ==0) && (DOS_ma==6)) DOS_mi = 1;
					printf("PC-DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
			}
			else if (DOS_OEM == 0xEE) {
				printf("DR DOS version 5.0 OR 6.0 (pretends to be MS-DOS %u.%u)\n%s",DOS_ma,DOS_mi,dogvr);
			}
			else if (DOS_OEM == 0xEF) {
				printf("Novell DOS 7\n%s",dogvr);
			}
			else if (DOS_OEM == 0xFD) {
				printf("FreeDOS Kernel compatibility %u.%u\n",DOS_ma,DOS_mi);
				if(b == 0xff)
					printf("FreeDOS Kernel (build 1993 or prior)\n");
				else {
					printf("FreeDOS Kernel version %u.%u.%u\n",w >> 8, w & 0x00ff, b);
						/* to do: get version string too */
				}
				puts(dogvr);
      }
			else if (DOS_OEM == 0xFF) {
				printf("M$-DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
			}
			else {
				printf("DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
			}
			break;
		case 7 :
			if (DOS_mi==10)
				printf("Microsoft Windows 95 OSR2\n%s",dogvr);
			else
				printf("Microsoft Windows 95\n%s",dogvr);
			break;

		case 10 :
			printf("OS/2 version 1.%u\n%s",DOS_mi/10,dogvr);
			break;
		case 20 :
			if (DOS_mi < 30)
				printf("OS/2 version %u.%u\n%s",DOS_ma/10,DOS_mi/10,dogvr);
			else if(DOS_mi==30)
				printf("OS/2 Warp 3.0 virtual DOS machine\n%s",dogvr);
			else if(DOS_mi==40)
				printf("OS/2 Warp 4.0 virtual DOS machine\n%s",dogvr);
			break;
		default :
			printf("Unknown DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
	}

	return 0;

}

