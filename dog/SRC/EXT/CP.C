/*

CP.C - DOG - Alternate command processor for freeDOS

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

TODO: Make the whole thing!

History
18.03.00 - Extracted from DOG.C - WB

****************************************************************************/

#include "ext.h"

/* BUFF_SIZE = 10240 Bytes / 16 bytes/paragraph = 640 16-byte-paragraps */
#define BUFF_SIZE 10240
#define BUFF_PARA 280h

int main(BYTE n, BYTE *arg[])
{
	BYTE i,*src,*dst;
	WORD r,sd,dd,buff_seg,buff_sz = BUFF_SIZE;
	DWORD src_sz,trg_sz;
	struct ffblk ffb;
	
	
	if(findfirst(arg[2],&ffb,FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN) == 0) {
		fprintf(stderr,"Replace %s (Yes/No)? ",ffb.ff_name);
		i = getchar();
		if((i == 'n') || (i == 'N'))
			return -1;
	}
	i = findfirst(arg[1],&ffb,FA_NORMAL|FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN);
	if (i == -1) {
		fprintf(stderr,"File %s NOT found\n",arg[1]);
		return -1;
	}
	
	asm MOV AH,48h       ; 
	asm MOV BX,BUFF_PARA ;
	asm INT 21h          ; /* allocate memory */
	asm JNC cp_1         ; /* allocation successfull */
	asm SHL BX           ; /* BX is number of 16-byte paragraphs */
	asm SHL BX           ; /* BX is number of 16-byte paragraphs */
	asm SHL BX           ; /* BX is number of 16-byte paragraphs */
	asm SHL BX           ; /* BX is number of 16-byte paragraphs */
	asm MOV buff_sz,BX   ; /* save the new buff sz now number of bytes */
	asm cp_1:     
	asm MOV buff_seg,AX  ; /* save segment */
	asm MOV AH,3Dh       ;
	asm MOV AL,21h       ; /* open RO,DENYWRITE = 0100001b */
	asm MOV DX,arg[1]    ; /* filename in DS:DX */
	asm INT 21h          ; /* open file */
	asm JC cp_err        ; /* handle error */
	asm MOV sd,AX        ; /* save HANDLE */

	asm MOV AH,3Ch       ;
	asm MOV AL,21h       ; /* open RO,DENYWRITE = 0100001b */
	asm MOV DX,arg[2]    ; /* filename in DS:DX */
	asm INT 21h          ; /* open file */
	asm JC cp_err        ; /* handle error */
	asm MOV dd,AX        ; /* save HANDLE */
	
	asm MOV CX,buff_sz   ; /* number of bytes to read */
	asm MOV BX,sd        ; /* source file descriptor */
	asm MOV DX,buff_seg  ;
	asm PUSH DS          ;
	asm PUSH DX          ;
	asm POP DS           ;
	asm XOR DX,DX        ;
			
	asm cp_2:            ; /* repat until bytes read = 0 */
	
	asm MOV AH 3fh       ;
	asm INT 21h          ; /* READ from file */
	asm JC cp_err        ; /* an error occured */

	asm MOV CX, AX       ; /* CX is now bytes read */
	asm JCXZ cp_3        ; /* if bytes read = 0 we are finished */
	
	asm MOV AH 40h       ;
	asm INT 21h          ; /* WRITE to file */
	asm JC cp_err        ; /* an error occured */
	asm JMP cp_2         ;
		
	asm cp_3:
	
	return 0;
}
