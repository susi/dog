/*

MV.C - DOG - Alternate command processor for freeDOS

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

TODO: support moving over drives, implement -i

History
18.03.00 - Extracted from DOG.C - WB
20.10.00 - Renamed nn to mv (It should havee been named mv from the beginning)
           Fixed do_mv. - WB
03.01.02 - Fixed bug that prevented moving to/from root directory (same as in
            rm.c) - WB
2003-03-11 - Added wildcard support - WB (used K.Harikirian's source from CP.C)

**************************************************************************/

#include <io.h>
#include <fcntl.h>
#include "ext.h"

BYTE flag_i = FLAG_UNSET;
BYTE flag_v = FLAG_UNSET;
BYTE flag_h = FLAG_UNSET;

WORD newname(BYTE *oldname, BYTE *newname);
BYTE *trueName(BYTE *name,BYTE *tn);
void print_help(void);

int main(BYTE n, BYTE *arg[])
{
	struct ts_args {
		BYTE **patt;
		BYTE npatt;
	}mv_p;
	BYTE sn_fil[MAXPATH+13], st_fil[MAXPATH+13], dn_fil[MAXPATH+13], dt_fil[MAXPATH+13];
	BYTE s_drv[MAXDRIVE], s_dir[MAXDIR], s_fil[MAXFILE], s_ext[MAXEXT];
	BYTE d_drv[MAXDRIVE], d_dir[MAXDIR], d_fil[MAXFILE], d_ext[MAXEXT];
	BYTE *p, b, ndir[129], odir[129], nn[129]={0},on[129]={0},tn[12]={0}, c;
#ifdef MV_WILD
	BYTE fn[129]={0}, wild=0;
#endif
	WORD r;
	BYTE i,j,k;
	struct ffblk ffb,*fb=&ffb;
	struct ffblk ff;

	k = 0;
	mv_p.npatt = 0;

	for(i=1;i<n;i++) {
		if ((arg[i][0] == '-') || (arg[i][0] == '/')) {
			for(j=1;arg[i][j]!='\0';j++) {
				switch(arg[i][j]) {
				 case 'i':
					flag_i = FLAG_SET;
					flag_v = FLAG_SET;
					break;
				 case 'H':
				 case 'h':
				 case '?':
					print_help();
						return 0;
				 case 'v':
					flag_v = FLAG_SET;
					break;
				}
			}
		}
		else {
			mv_p.patt[k] = arg[i];
			k++;
		}
	}

	mv_p.npatt = k;

	if(mv_p.npatt == 0){
		fputs("Missing source and destination",stderr);
		print_help();
		return -1;
	}
	else if(mv_p.npatt == 1){
		mv_p.patt[1] = "*.*";
		mv_p.npatt++;
	}
	else if(mv_p.npatt > 2) {
		fputs("Too many file arguments",stderr);
		print_help();
		return -1;
	}

	strcpy(sn_fil,mv_p.patt[0]);
	fnsplit(sn_fil, s_drv, s_dir, s_fil, s_ext);

	strcpy(dn_fil,mv_p.patt[1]);

	b = findfirst(mv_p.patt[0],fb,FA_NORMAL|FA_DIREC|FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN);
	if((b != 0) && (errno == ENOFILE) ) {
		printf("Can not find %s.\n",mv_p.patt[0]);
		return -1;
	}

	while (b == 0) {

		strcpy(sn_fil,mv_p.patt[0]);
		strcpy(dn_fil,mv_p.patt[1]);

		strcpy(sn_fil, s_drv);
		strcat(sn_fil, s_dir);
		strcat(sn_fil, ffb.ff_name); /*Got the fullpath of source*/

		fnsplit(sn_fil, s_drv, s_dir, s_fil, s_ext);
		fnsplit(dn_fil, d_drv, d_dir, d_fil, d_ext);

		/*Replace wildcards '?' and/or '*'*/
		for(j = 0; j < strlen(d_fil); j++){
/* fileneme */
			if(d_fil[j] == '?'){
				d_fil[j] = s_fil[j];
#ifdef MV_DEBUG
				printf("%s:%d: ? found at %d: s_fil[%d]=%c\n",__FILE__,__LINE__,j,j,s_fil[j]);
#endif
			}
			else if((d_fil[j] == '*') && (strlen(s_fil) >= strlen(d_fil))){
#ifdef MV_DEBUG
				printf("%s:%d: * found at %d: s_fil='%s' d_fil='%s'\n",__FILE__,__LINE__,j,j,s_fil,d_fil);
#endif
				for(k = j; k <= strlen(s_fil); k++){
					d_fil[k] = s_fil[k];
				}
			}
			else if((d_fil[j] == '*') && (strlen(s_fil) < strlen(d_fil))){
#ifdef MV_DEBUG
				printf("%s:%d: * found at %d: s_fil='%s' d_fil='%s', source SHORTER than dest\n",__FILE__,__LINE__,j,j,s_fil,d_fil);
#endif
				for(k = j; k <= strlen(d_fil); k++){
					d_fil[k] = '\0';
#ifdef MV_DEBUG
					printf("%s:%d:            %d: s_fil[%d]='%c'\n",__FILE__,__LINE__,k,k,s_fil[k]);
#endif
				}
			}
		}
/* extension */
		for(j = 0; j < strlen(d_ext); j++){
			if(d_ext[j] == '?'){
				d_ext[j] = s_ext[j];
#ifdef MV_DEBUG
				printf("%s:%d: ? found at %d: s_ext[%d]=%c\n",__FILE__,__LINE__,j,j,s_ext[j]);
#endif
			}
			else if((d_ext[j] == '*') && (strlen(s_ext) >= strlen(d_ext))){
#ifdef MV_DEBUG
				printf("%s:%d: * found at %d: s_ext='%s' d_ext='%s'\n",__FILE__,__LINE__,j,j,s_ext,d_ext);
#endif
				for(k = j; k <= strlen(s_ext); k++){
					d_ext[k] = s_ext[k];
#ifdef MV_DEBUG
				printf("%s:%d:            %d: s_ext[%d]='%c'\n",__FILE__,__LINE__,k,k,s_ext[k]);
#endif
				}
			}
			else if((d_ext[j] == '*') && (strlen(s_ext) < strlen(d_ext))){
#ifdef MV_DEBUG
				printf("%s:%d: * found at %d: s_ext='%s' d_ext='%s', source SHORTER than dest\n",__FILE__,__LINE__,j,j,s_ext,d_ext);
#endif
				for(k = j; k <= strlen(d_ext); k++){
					d_ext[k] = '\0';
				}
			}
		}

		strcpy(dn_fil, d_drv);
		strcat(dn_fil, d_dir);
		strcat(dn_fil, d_fil);
		strcat(dn_fil, d_ext);

		if(flag_v == FLAG_SET) printf("%s --> %s",sn_fil,dn_fil);
		if(flag_i == FLAG_SET) {
			if(findfirst(dn_fil,&ff,FA_NORMAL|FA_DIREC|FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN)==0) {
				if((ff.ff_attrib & FA_DIREC) == FA_DIREC)
					printf(": Replace EXISTING directory? (Y/N)? ");
				else
					printf(": Replace EXISTING file? (Y/N)? ");
				c=(BYTE)getchar();
				if(c=='y' || c=='Y') {
					if((ff.ff_attrib & FA_DIREC) == FA_DIREC) {
						if(rmdir(dn_fil)!=0) {
							printf("Error removing directory");
							b = findnext(fb);
							continue;
						}
					}
					else {
						if(unlink(dn_fil)!=0) {
							printf("Error removing file");
							b = findnext(fb);
							continue;
						}
					}
				}
			}
			else {
				if((ffb.ff_attrib & FA_DIREC) == FA_DIREC)
					printf(": Move directory? (Y/N)? ");
				else
					printf(": Move file? (Y/N)? ");
				c=(BYTE)getchar();
				if(c=='y' || c=='Y');
				else {
					b = findnext(fb);
					continue;
				}
			}
		}

		printf("\n");


		if(rename(sn_fil,dn_fil)!=0) {
			switch(errno) {
			 case ENOENT:
				printf("Can not find file or directory %s.\n",sn_fil);
				break;
			 case EACCES:
				puts("Access denied.");
				break;
			 case ENOTSAM:
				puts("Source and destination on different drives");
				break;
			}
		}

		b = findnext(fb);
	}
	return 0;

}

/**************************************************************************/

WORD newname(BYTE *oldname, BYTE *newname)
{
	WORD r;

	asm mov ah,56h
	asm push ds
	asm pop es
	asm mov dx,oldname
	asm mov di,newname
	asm int 21h
	asm jnc	nn_ok
	asm mov r,ax


	return r;
	nn_ok:
	printf("r=0x%x\n",r);

	return 0;
}

/**************************************************************************/

void print_help(void)
{
	fputs("Usage: MV [OPTION]... SOURCE DEST\n  or:  MV [OPTION]... [PATH\\]PATTERN1 [PATH\\]PATTERN2\n\n", stderr);
	fputs("MoVe (Rename) SOURCE to DEST or All files with PATTERN1 [with an optional path]\n"
				"  to new names with PATTERN2 [with an optionional path]",stderr);
	fputs("The OPTIONS are:\n",stderr);
	fputs("\t-v:  verbose: print filename of each moved file\n",stderr);
/*  \t-i:  interactive mode: prompt (Y/N) for each file or directory\n */
	fputs("  -h|-H|-?:  display this help and exit\n",stderr);
	fputs("\nFILE is a name of a file to write\n", stderr);
	fputs("\nMV is part of DOG (http://dog.sf.net/)\n", stderr);
}
