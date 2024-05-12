/*

CT.C - DOG - Change Terminal (CTTY)

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
David McIlwraith (DMcI)

History
25.03.00 - started... based on the code written by David MacIlwraith
2024-05-11 - Building as a module. - WB
*/
#include "dog.h"

#pragma argsused
void do_ct(BYTE n)
{
	asm MOV ax,3d02h            ;/* open file using handle */
	asm MOV dx,offset arg[1]    ;/* filename */
	asm INT 21h                 ;/* open file... er device */
	asm JC do_ct_err
	asm MOV bx,ax               ;/* save handle */
	asm MOV ax,4400h            ;/* IOCTL - Get dev info */
	asm INT 21h
	asm JC do_ct_err
	asm test dl,80h             ;/* is it a device */
	asm JZ do_ct_err
	asm MOV ax,4406h
	asm INT 21h                 ;/* IOCT - Get input status */
	asm JC do_ct_err
	asm OR al,al                ;/* is it ready? */
	asm JZ do_ct_dnr
	asm MOV ax,4407h
	asm INT 21h                 ;/* IOCT - Get output status */
	asm JC do_ct_err
	asm OR al,al                ;/* is it ready? */
	asm JZ do_ct_dnr
	asm MOV cx,0003h            ;/* STDERR */
	asm MOV ah,46h              ;/* DUP2: make handle in CX same as */

  do_ct_dup2:                 ;/* handle in BX = handle of input dev */

	asm INT 21h
	asm JC do_ct_err
	asm loop do_ct_dup2         ;/*decreases cx and jmp to label while cx>0 */

    return;

    do_ct_err:
        fprintf(stderr,"Invalid device.\n");
        return;
    do_ct_dnr:
        fprintf(stderr,"Device %s not ready error.\n",arg[1]);
        return;
}
