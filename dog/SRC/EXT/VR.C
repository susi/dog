/*

VR.C - DOG - Alternate command processor for freeDOS

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
2002-05-15 - Added support for detecting Windows version (<= 4.*)
**************************************************************************/

#include "ext.h"

int is_win(BYTE *pmaj, BYTE *pmin, WORD *pmode);
int detect_switcher(void);

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
  WORD w,DR_vr,win_mode;
	BYTE b,DOS_ma=3,DOS_mi=30,DOS_OEM=0xFD,win_ma=0,win_mi=0;
	BYTE DOG_ma=0,DOG_mi=0,DOG_re=0x0b,dogvr[161];
	struct dosemu_detect far* given = (void far*) DOSEMU_MAGIC_LOCATION;
	struct dosemu_detect expect = {DOSEMU_MAGIC};

		/* test for dosemu */

	if ((expect.u.magic[0] == given->u.magic[0]) && (expect.u.magic[0] == given->u.magic[0]))
		printf("Running under Dosemu, version %d.%d.%d.%d\n",given->v.ver[3],given->v.ver[2],given->v.ver[1],given->v.ver[0]);

	/* check for DOG int d0 */

	asm MOV ax,35d0h
	asm int 21h
	asm push es
	asm pop ax
	asm cmp ax,0000h
	asm jz no_DOG
	asm MOV ah,01h
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
    printf("DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
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
      if(is_win(&win_ma,&win_mi,&win_mode) == 1) {
        printf("Window$ version %u.%u\n",win_ma,win_mi);
      }
    }
    else {
      printf("DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
    }
    break;
   case 7 :
    if(is_win(&win_ma,&win_mi,&win_mode) == 1) {
      if ((DOS_mi==10) && (win_ma == 4) && (win_mi == 0))
        printf("Microsoft Windows 95 OSR2\n%s",dogvr);
      else if ((DOS_mi==0) && (win_ma == 4) && (win_mi == 0))
        printf("Microsoft Windows 95\n%s",dogvr);
      else if ((DOS_mi==10) && (win_ma == 4) && (win_mi == 3))
        printf("Microsoft Windows 98\n%s",dogvr);
      else
        printf("Windows %u.%02u\nDOS version %u.%u\n%s",win_ma,win_mi,DOS_ma,DOS_mi,dogvr);
    }
    else
      printf("Unknown DOS version %u.%u\n%s",DOS_ma,DOS_mi,dogvr);
    
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

#define REAL_MODE           1
#define STANDARD_MODE       2
#define ENHANCED_MODE       3

#define SYSTEM_VM           1

int detect_switcher(void)
{
    int retval = 1;
    asm push di
    asm push es
    asm xor bx, bx
    asm mov di, bx
    asm mov es, bx
    asm mov ax, 4b02h
    asm int 2fh
    asm mov cx, es
    asm or cx, di
    asm je no_switcher
done:       
    asm pop es
    asm pop di
    return retval;
no_switcher:
    retval = 0;
    goto done;
}

int is_win(BYTE *pmaj, BYTE *pmin, WORD *pmode)
{
  WORD maj=0, retval = 0;
  BYTE min=0, mode=0;
    
  /* make sure someone, anyone has INT 2Fh */
	asm MOV ax,352fh
	asm int 21h
	asm push es
	asm pop ax
	asm cmp ax,0000h
	asm jz is_win_1
  asm jmp is_win_end
  is_win_1:
  /* call 2F/160A to see if Windows X*/
  asm mov ax, 160ah
  asm int 2fh
  asm cmp ax,0000h
  asm jne iswin_1

  asm mov mode, cx  /* CX=2 means Std; CX=3 means Enh */
  asm mov maj, bh   /* BX = major/minor (e.g., 030Ah) */
  asm mov min, bl
  *pmaj = maj;
  *pmin = min;
  *pmode = mode;
  return 1;

  iswin_1:
  /* call 2F/1600 to see if Windows 3.0 Enhanced mode or Windows/386 */
  asm mov ax, 1600h
  asm int 2fh
  asm mov maj, al
  asm mov min, ah
  if ((maj == 1) || (maj == 0xFF)) {  /* Windows/386 2.x is running */
    *pmaj = 2;              /* Windows/386 2.x */
    *pmin = 1;              /* don't know; assume 2.1? */
    *pmode = ENHANCED_MODE; /* Windows/386 sort of like Enhanced */
    return 1;
  }
  else if (! ((maj == 0) || (maj == 0x80))) { /* AL=0 or 80h if no Windows */
    /* must be Windows 3.0 Enhanced mode */
    *pmaj = maj;
    *pmin = min;
    *pmode = ENHANCED_MODE;
    return 1;
  }
    
  /* call 2F/4680 to see if Windows 3.0 Standard or Real mode; but, 
   this could be a "3.0 derivative" such as DOSSHELL task switcher! */
  asm mov ax, 4680h
  asm int 2fh
  asm mov retval, ax
  if (retval == 0) {            /* AX=0 if 2F/4680 handled */
    /* make sure it isn't DOSSHELL task switcher */
    if (detect_switcher())
      return 0;
    *pmaj = 3;
    *pmin = 0;
        
        /* either have Windows Standard mode or Real mode; to 
           distinguish, have to do fake Windows broadcasts with
           2F/1605.  Yuk!  We'll avoid that here by assuming
           3.0 Standard mode.  If you really want to distinguish
           3.0 Standard mode and Real mode, see _MSJ_, March 1991,
           p. 113; and MS KB articles Q75943 and Q75338 */
    *pmode = STANDARD_MODE;
    return 1;
  }
  is_win_end:
  /* still here -- must not be running Windows */
  return 0;
}
