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
2002-02-22 - rewrote in C - no ASM - WB
****************************************************************************/
#include <io.h>
#include <fcntl.h>
#include "ext.h"

/* BUFF_SIZE = 10240 Bytes / 16 bytes/paragraph = 640 16-byte-paragraps */
#define BUFF_SIZE 10240
#define BUFF_PARA 280h

int main(BYTE n, BYTE *arg[])
{
	signed char i;
	int src,dst,r;
	WORD *buff;
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
	
	trg_sz = ffb.ff_fsize;
	
	if ((buff = malloc(BUFF_SIZE)) == NULL) {
		fprintf(stderr,"Not ennough memory!\n");
		return -1;
	}
	
	if ((src = open(arg[1],O_RDONLY | O_BINARY)) == -1) {
		fprintf(stderr,"Cannot open %s error is: %s\n",arg[1],perror(""));
		return -1;
	}
	if ((dst = open(arg[2],O_WRONLY | O_TRUNC | O_CREAT | O_BINARY)) == -1) {
		fprintf(stderr,"Cannot open %s error is: %s\n",arg[2],perror(""));
		return -1;
	}

	while(!eof(src)) {
		r = read(src,buff,BUFF_SIZE);
		write(dst,buff,r);
	}
	
	close(src);
	close(dst);
	
	return 0;
}
