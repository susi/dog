/*

VR.C - DOG - Alternate command processor for freeDOS

Copyright (C) 1999,2000-2024 Wolf Bergenheim

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
2024-05-10 - Cleaned up code and added DOSBox detection
2024-05-08 - Improved DR DOS detection
2024-05-13 - Fixed FreeDOS detection, also print OS_VERSION for FreeDOS
2024-10-10 - Always try to get true version (int21h/ax=3306h) as well as the kernel
             string (int21h/ax=33ffh), and display these when successful.
**************************************************************************/

#include "ext.h"
#include <stdlib.h>

int is_win(BYTE *pmaj, BYTE *pmin, WORD *pmode);
int detect_switcher(void);
WORD get2e(void);
const char * OEM2str(BYTE OEM);
WORD dog_test(char * dog_str);
WORD drdos_version(void);

#define DOSEMU_BIOS_DATE "02/25/93"
#define DOSEMU_MAGIC "$DOSEMU$"
#define DOSEMU_BIOS_DATE_LOCATION 0xF000FFF5
#define DOSEMU_MAGIC_LOCATION 0xF000FFE0
#define TRUE 0xFF
#define FALSE 0x00

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
    WORD DOG_vr,win_mode,sys_seg,cmd_seg,DR_vrnat, kernel_string_off, kernel_string_seg;
    BYTE DR_vr,DOS_ma=3,DOS_mi=30, DOS_t_ma=0, DOS_t_mi=0, DOS_OEM=0xFD,DOS_rev=0,win_ma=0,win_mi=0, vbe_check,serial[3];
    struct dosemu_detect far* given = (void far*) DOSEMU_MAGIC_LOCATION;
    struct dosemu_detect expect = {DOSEMU_MAGIC};
    char dog_str[161];
    char *drdos_VER, *drdos_OS, *fd_VER;
    char far *kernel_str = 0;
#ifdef VR_DEBUG
    BYTE DR_nat;
#endif

    /* test for dosemu */
    if ((expect.u.magic[0] == given->u.magic[0]) && (expect.u.magic[0] == given->u.magic[0]))
	printf("Running under Dosemu, version %d.%d.%d.%d\n",given->v.ver[3],given->v.ver[2],given->v.ver[1],given->v.ver[0]);

    /* Check for DOG */
    dog_test(dog_str);

    /* Get DOS Version */
    asm MOV ah,30h;
    asm MOV al,00h;
    asm INT 21h;
    asm MOV DOS_ma,  al;
    asm MOV DOS_mi,  ah;
    asm MOV DOS_OEM, bh;
    asm MOV serial[0], ch /* serial number is a 24 bit number in BL:CX - saving it in little-endian order */;
    asm MOV serial[1], cl;
    asm MOV serial[2], bl /* revision seq for FreeDOS */;
#ifdef VR_DEBUG
    printf("DEBUG: DOS version %u.%u rev %02Xh OEM:%02Xh serial: %02X%02X%02Xh\n",DOS_ma,DOS_mi,DOS_rev,DOS_OEM,serial[2],serial[1],serial[0]);
#endif
    /* Try to Get true DOS version */
    asm mov ax,3306h;
    asm int 21h;
    asm cmp al, 0FFh;
    asm je  old_dos;
    asm cmp ah, 00h;
    asm je  old_dos;
    asm mov DOS_t_ma, bl;
    asm mov DOS_t_mi, bh;
    asm and dl, 7h /* bits 0-2 */;
    asm mov DOS_rev, dl;
#ifdef VR_DEBUG
    printf("DEBUG: real DOS version %u.%u rev %02Xh\n", DOS_t_ma, DOS_t_mi, DOS_rev);
#endif
    /* Ignore version flag */
    /* Try to get (FreeDOS) kernel string */
    /* NOTE: This is skipped if 3306h is not supported */
    asm mov ax,33FFh;
    asm xor dx, dx;        /* It's stored in dx:ax */
    asm int 21h;
    asm cmp dx, 0h;
    asm jz  old_dos;
    asm mov kernel_string_seg,dx; /* kernel string offset */
    asm mov kernel_string_off,ax; /* kernel string offset */

    kernel_str = MK_FP(kernel_string_seg, kernel_string_off);
#ifdef VR_DEBUG
    printf("DEBUG: kernel string: %Fs", kernel_str);
#endif

  old_dos:
#ifdef VR_DEBUG
    if (DOS_t_ma == 0) {
	printf("DEBUG: DOS version %u.%02u rev %02Xh OEM:%02Xh serial: %02X%02X%02Xh\n",DOS_ma,DOS_mi,DOS_rev,DOS_OEM,serial[2],serial[1],serial[0]);
    }
    else {
	printf("DEBUG: DOS version %u.%02u (compat %u.%02u)\n"
	       "       rev %02Xh OEM:%02Xh serial: %02X%02X%02Xh\n",
	       DOS_t_ma, DOS_t_mi, DOS_ma, DOS_mi, DOS_rev,
	       DOS_OEM, serial[2], serial[1], serial[0]);
    }
#endif

    /* Check for DOSBox */
    if(DOS_OEM == 0xFF) {
	/* try to detect DOSBox */
	/* get list of lists */
	/* DOSBox stores it in segment 80h */
	asm mov ah, 52h;
	asm int 21h;
	asm push es;
	asm pop ax;
        asm mov sys_seg, ax;
        cmd_seg = get2e();
	/* DOSBox 0.74-2+ at least has these segments hard coded */
#ifdef VR_DEBUG
	printf("DEBUG: Command segment: %04Xh SYS segment: %04Xh\n", cmd_seg, sys_seg);
#endif
	if(cmd_seg == 0xf000 && sys_seg == 0x0080) {
	    printf("DOSBox pretends to be MS-DOS %u.%02u\n%s", DOS_ma, DOS_mi, dog_str);
	}
	/* not DOSBox, continue */
	else if(DOS_ma < 7) {
	    if (DOS_t_ma == 0) {
		printf("%s version %u.%02u\n", OEM2str(DOS_OEM), DOS_ma,DOS_mi);
	    }
	    else {
		printf("%s version %u.%02u (with kernel compatibility %u.%02u)\n",
		       OEM2str(DOS_OEM), DOS_t_ma,DOS_t_mi, DOS_ma,DOS_mi);
	    }
	    if (kernel_str != NULL) {
		printf("%Fs\n", kernel_str);
	    }
	    puts(dog_str);
	}
    }
    else if (DOS_OEM == 0x00 || DOS_OEM == 0xEE || DOS_OEM == 0xEF) { /* DR DOS */
	/* Check for DR DOS version */
	DR_vrnat = drdos_version();
	DR_vr = (BYTE)(DR_vrnat & 0x00ff);
	drdos_VER = getenv("VER");
#ifdef VR_DEBUG
	DR_nat = (BYTE)(DR_vrnat>>8);
	drdos_OS = getenv("OS");
	    printf("DEBUG: DR-DOS Product %02Xh Multi-user nature: %02Xh\n",DR_vr, DR_nat);
	    if (drdos_VER != NULL)
		printf("DEBUG: env VER='%s'\n", drdos_VER);
	    if (drdos_OS != NULL)
		printf("DEBUG: env OS='%s'\n", drdos_OS);
#endif
	switch(DR_vr) {
	case 0x41:
	    printf("DOS Plus 1.2\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x60:
	    printf("DOS Plus 2.?\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x63:
	    printf("DR DOS 3.41\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x64:
	    printf("DR DOS 3.42\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x65:
	    printf("DR DOS 5.0\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x67:
	    printf("DR DOS 6.0\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x70:
	    printf("PalmDOS 5.0\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x71:
	    printf("DR DOS 6.0 March 1993 \"business update\"\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x72:
	    printf("DR DOS 7.0\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	case 0x73:
	    printf("DR-DOS 7.03\n\tProviding DOS %u.%u interface\n",DOS_ma,DOS_mi);
	    break;
	default:
	    if (drdos_VER != NULL) {
		printf("DR DOS %s\n\tProviding DOS %u.%u interface\n",drdos_VER,DOS_ma,DOS_mi);
	    }
	    else {
		printf("PC-DOS version %u.%u\n",DOS_ma,DOS_mi);
	    }
	}
	if (kernel_str != NULL) {
	    printf("%Fs\n", kernel_str);
	}
	puts(dog_str);
    }
    else if (DOS_OEM == 0xFD) { /* FreeDOS */
	fd_VER = getenv("OS_VERSION");
	if (fd_VER != NULL) {
	    printf("FreeDOS version %s\n", fd_VER);
	}
	printf("FreeDOS Kernel compatibility %u.%u\n",DOS_ma,DOS_mi);
	if(serial[0] == 0xFF) {
	    printf("FreeDOS Kernel (build 1993 or prior)\n");
	}
	else if (kernel_str != NULL) {
	    printf("%Fs\n", kernel_str);
	}
	puts(dog_str);
    }
    else if(DOS_ma == 5 && DOS_mi == 50) { /* Windows NT reports DOS version 5.50 */
	printf("DOS Command Prompt under Windows NT\n%s", dog_str);
	return 0;
    }
    else if(DOS_OEM != 0xFF) {
	if (DOS_t_ma == 0) {
	    printf("%s version %u.%02u\n", OEM2str(DOS_OEM), DOS_ma,DOS_mi);
	}
	else {
	    printf("%s version %u.%02u (with kernel compatibility %u.%02u)\n",
		   OEM2str(DOS_OEM), DOS_t_ma,DOS_t_mi, DOS_ma,DOS_mi);
	}
	if (kernel_str != NULL) {
	    printf("%Fs\n", kernel_str);
	}
	puts(dog_str);
    }
    else {
	switch(DOS_ma) {  /*versions 1 - 4 Just print OEM and version*/
	case 7:
	    break;
	case 10:
	    printf("OS/2 version 1.%u\n%s",DOS_mi/10,dog_str);
	    break;
	case 20:
	    if (DOS_mi < 30)
		printf("OS/2 version %u.%u\n%s",DOS_ma/10,DOS_mi/10,dog_str);
	    else if(DOS_mi==30)
		printf("OS/2 Warp 3.0 virtual DOS machine\n%s",dog_str);
	    else if(DOS_mi==40)
		printf("OS/2 Warp 4.0 virtual DOS machine\n%s",dog_str);
	    break;
	default:
	    if (DOS_t_ma == 0) {
		printf("%s version %u.%02u\n", OEM2str(DOS_OEM), DOS_ma,DOS_mi);
	    }
	    else {
		printf("%s version %u.%02u (with kernel compatibility %u.%02u)\n",
		       OEM2str(DOS_OEM), DOS_t_ma,DOS_t_mi, DOS_ma,DOS_mi);
	    }
	    if (kernel_str != NULL) {
		printf("%Fs\n", kernel_str);
	    }
	    puts(dog_str);
	}
    }

    if (is_win(&win_ma,&win_mi,&win_mode) == 1) {
	if ((DOS_ma == 7) && (DOS_mi==10) && (win_ma == 4) && (win_mi == 0))
	    printf("Microsoft Windows 95 OSR2\n%s",dog_str);
	else if ((DOS_ma == 7) && (DOS_mi==0) && (win_ma == 4) && (win_mi == 0))
	    printf("Microsoft Windows 95\n%s",dog_str);
	else if ((DOS_ma == 7) && (DOS_mi==10) && (win_ma == 4) && (win_mi == 3))
	    printf("Microsoft Windows 98\n%s",dog_str);
	else
	    printf("Windows %u.%02u\n",win_ma,win_mi);
    }
    else {
	if ((DOS_ma == 7) && (DOS_OEM != 0xFD)) {
	    if (DOS_mi==0)
		printf("Microsoft Windows 95\n%s",dog_str);
	    else if (DOS_mi==10)
		printf("Microsoft Windows 98\n%s",dog_str);
	    else {
		if (DOS_t_ma == 0) {
		    printf("%s version %u.%02u\n", OEM2str(DOS_OEM), DOS_ma,DOS_mi);
		}
		else {
		    printf("%s version %u.%02u (with kernel compatibility %u.%02u)\n",
			   OEM2str(DOS_OEM), DOS_t_ma,DOS_t_mi, DOS_ma,DOS_mi);
		}
		if (kernel_str != NULL) {
		    printf("%Fs\n", kernel_str);
		}
		puts(dog_str);
	    }
	}
    }

    return 0;
}

WORD dog_test(char *dog_str)
{
    WORD DOG_vr;
    BYTE DOG_ma=0,DOG_mi=0,DOG_re=0x0b;

    /* check for DOG int d0 */
    asm mov ax,35d0h;
    asm int 21h;
    asm push es;
    asm pop ax;
    asm cmp ax,0000h;
    asm jz no_DOG    /* DOG not installed */;
    asm mov ax,0123h /* DOG function 1 with test */;
    asm int 0d0h     /* DOG service */;
    asm cmp ax,0123h /* AX unchanged, DOG not installed */;
    asm jz no_DOG;
    /* DOG version is packed as 0x123a = 1.2.3a or 0x083b = 0.8.3b */
    asm mov DOG_vr, ax;
    /* Unpack the dog version */
    asm mov bh,ah;
    asm shr bh,1;
    asm shr bh,1;
    asm shr bh,1;
    asm shr bh,1;
    asm MOV DOG_ma,bh;
    asm AND ah,0fh;
    asm MOV DOG_mi,ah;
    asm MOV DOG_re,al;
    asm and al, 0ah;  /* Test if code maturity is reasonable */
    asm cmp al, 0ah;  /* by anding it with 0ah and checking it */
    asm jnz no_DOG;   /* it should always be 0ah since */
                      /* it can only have values 0ah, 0bh or 0fh */
    sprintf(dog_str,"DOG version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
    return DOG_vr;

no_DOG:
    sprintf(dog_str,"DOG not detected.\n\nYou can get DOG from https://dog.zumppe.net/\n");
    return 0;
}

WORD drdos_version(void)
{
    WORD dr_vr;
    /* CALL INT 21h with AX = 4452h ("DR") and CF set to check if DR DOS */
    asm mov ax, 4452h;
    asm stc;
    asm int 21h;
    asm jnc maybe_dr;
    asm jmp not_dr;
 maybe_dr:
    asm cmp ax, 4452h;
    asm je not_dr;

    asm mov dr_vr,ax;
    return dr_vr;
not_dr:
    return 0;
}

#define REAL_MODE           1
#define STANDARD_MODE       2
#define ENHANCED_MODE       3

#define SYSTEM_VM           1

int detect_switcher(void)
{
    int retval = 1;
    asm push di;
    asm push es;
    asm xor bx, bx;
    asm mov di, bx;
    asm mov es, bx;
    asm mov ax, 4b02h;
    asm int 2fh;
    asm mov cx, es;
    asm or cx, di;
    asm je no_switcher;
done:
    asm pop es;
    asm pop di;
    return retval;
no_switcher:
    retval = 0;
    goto done;
}

WORD get2e(void)
{
    WORD int2e_seg=0;
    asm MOV ax,352eh;
    asm int 21h;
    asm push es;
    asm pop ax;
    asm mov int2e_seg,ax;

   return int2e_seg;
}

int is_win(BYTE *pmaj, BYTE *pmin, WORD *pmode)
{
  WORD maj=0, retval = 0;
  BYTE min=0, mode=0;

  /* make sure someone, anyone has INT 2Fh */
    asm MOV ax,352fh;
    asm int 21h;
    asm push es;
    asm pop ax;
    asm cmp ax,0000h;
    asm jz is_win_1;
  asm jmp is_win_end;
  is_win_1:
  /* call 2F/160A to see if Windows X*/
  asm mov ax, 160ah;
  asm int 2fh;
  asm cmp ax,0000h;
  asm jne iswin_1;

  asm mov mode, cx  /* CX=2 means Std; CX=3 means Enh */;
  asm mov maj, bh   /* BX = major/minor (e.g., 030Ah) */;
  asm mov min, bl;
  *pmaj = maj;
  *pmin = min;
  *pmode = mode;
  return 1;

  iswin_1:
  /* call 2F/1600 to see if Windows 3.0 Enhanced mode or Windows/386 */
  asm mov ax, 1600h;
  asm int 2fh;
  asm mov maj, al;
  asm mov min, ah;
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
  asm mov ax, 4680h;
  asm int 2fh;
  asm mov retval, ax;
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

const char * OEM2str(BYTE OEM)
{
    /* source RBIL https://www.ctyme.com/intr/rb-2711.htm */
    switch(OEM) {
    case 0x01:
	return "Compaq DOS";
    case 0x02:
	return "MS Packaged Product DOS";
    case 0x04:
	return "AT&T DOS";
    case 0x05:
	return "ZDS DOS";
    case 0x06:
    case 0x4D:
	return "Hewlett-Packard DOS";
    case 0x07:
	return "ZDS DOS";
    case 0x08:
	return "Tandon DOS";
    case 0x09:
	return "AST DOS";
    case 0x0A:
	return "Asem DOS";
    case 0x0B:
	return "Hantarex DOS";
    case 0x0C:
	return "SystemsLine DOS";
    case 0x0D:
	return "Packard-Bell DOS";
    case 0x0E:
	return "Intercomp DOS";
    case 0x0F:
	return "Unibit DOS";
    case 0x10:
	return "Unidata DOS";
    case 0x16:
	return "DEC DOS";
    case 0x17:
    case 0x23:
	return "Olivetti DOS";
    case 0x28:
	return "Texas Instruments DOS";
    case 0x29:
	return "Toshiba DOS";
    case 0x33:
	return "Novell DOS";
    case 0x34:
    case 0x35:
	return "MS Multimedia Systems DOS";
    case 0x5E:
	return "RxDOS";
    case 0x66:
    case 0xCD:
	return "PTS-DOS";
    case 0x99:
	return "General Software's Embedded DOS";
    case 0xED:
	return "OpenDOS/DR DOS";
    case 0xEE:
	return "DR DOS (http://www.drdos.org)";
    case 0xEF:
	return "Novell DOS";
    case 0xFD:
	return "FreeDOS - https://www.freedos.org/";
    case 0xFF:
	return "M$-DOS";
    default:
	return "Unknown DOS";
    }
}
