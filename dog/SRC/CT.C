/*

CT.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

*/
#pragma argsused
void do_ct(BYTE n)
{
    asm{
        mov ax,3d02h            ;/* open file using handle */
        mov dx,offset arg[1]    ;/* filename */
        int 21h                 ;/* open file... er device */
        jc do_ct_err
        mov bx,ax               ;/* save handle */
        mov ax,4400h            ;/* IOCTL - Get dev info */
        int 21h
        jc do_ct_err
        test dl,80h             ;/* is it a device */
        jz do_ct_err
        mov ax,4406h
        int 21h                 ;/* IOCT - Get input status */
        jc do_ct_err
        or al,al                ;/* is it ready? */
        jz do_ct_dnr
        mov ax,4407h
        int 21h                 ;/* IOCT - Get output status */
        jc do_ct_err
        or al,al                ;/* is it ready? */
        jz do_ct_dnr
        mov cx,0003h            ;/* STDERR */
        mov ah,46h              ;/* DUP2: make handle in CX same as */
    }
    do_ct_dup2:                 ;/* handle in BX = handle of input dev */
    asm{
        int 21h
        jc do_ct_err
        loop do_ct_dup2         ;/*decreases cx and jmp to label while cx>0 */
    }
    return;

    do_ct_err:
        fprintf(stderr,"Invalid device.\n");
        return;       
    do_ct_dnr:
        fprintf(stderr,"Device %s not ready error.\n",arg[1]);
        return;
}    
