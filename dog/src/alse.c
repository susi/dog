/*
ALSE.C - DOG - Alias and Set Env commands

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
2002-03-07 - extended the setevar and getevar functions to accept a third
             parameter: the segment to work in. Renamed them to setudata and
             getudata respectively. - WB
2002-03-08 - added mkudata().
2002-03-10 - added do_al(). It works like do_se().
2002-03-10 - finaly! aliasreplace() is done! and it works! We now have
             working aliases.
2002-03-15 - fixed a bug in do_se() snd do_al(); they appended a space to
             the variable value string. new function evarreplace(). scans
             the command line for strings like %evar% and %10% and replaces
             them with the appropriate string.
2024-05-11 - fixed getudata (and getevar and getalias) to require providing
             the size of value. To avoid some nasy buffer overruns.
             Also updated do_se and setudata to use standard functions.
2024-05-11 - Building as a module.
2024-05-19 - Fixed mkudata(), basically rewrote it, also using myallocmem and myfreemem. -WB
2024-06-09 - Fixed setudata() to also honor the "owner string" after the env
             values. Also using _fmemcpy instead of hend-written copy function.
             ENV (and Alias) structure: The N for the number of extra strings is
             basically always 1, and the string is the full name of the program
             owning the block.
             KEY=VALUE\0...KEYN=VALUEN\0\0
             WORD<N strings>string\0...stringN -WB
2024-06-11 - updated mkudata() to set up an env block including a comspec at the end.
             setudata() also works correctly when setting the first value. -WB
**************************************************************************/
#include <mem.h>
#include "dog.h"

static WORD usedbytes(WORD block, WORD bsz);


void do_al( BYTE n)
{
  BYTE b,*p;
  WORD w;
  BYTE far *aliasp;

	aliasp = MK_FP(aliasseg,0);
  if(n == 1) {
    printf("alias @ %Fp %u(%xh) bytes\n",aliasp,aliassz,aliassz);
    for(w=0;w<aliassz;w++) {
      if(*(aliasp+w)== 0) {
        if(*(aliasp+w+1) == 0)
        break;
        else
        puts("");
      }
      else
      printf("%Fc",*(aliasp+w));

    }
    printf("\n\n");
  }
  else {
    p = malloc(200);
    p[0] = '\0';
    for(b=2;b<n;b++) {
      strcat(p,arg[b]);
      strcat(p," ");
    }
#ifdef B_DEBUG_DISABLED
    printf("do_al:1:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if(*(p+(strlen(p)-1)) == ' ') *(p+(strlen(p)-1)) = '\0';
#ifdef B_DEBUG_DISABLED
    printf("do_al:2:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if (strlen(p) == 0) p = NULL;
    setalias(arg[1],p);
    free(p);
  }

}

/**************************************************************************/

void do_se( BYTE n)
{
    BYTE b,*p, *q;
  WORD w, l;

  if(n == 1) {
    printf("environment @ %Fp %u(%04Xh) bytes\n",_env,envsz,envsz);
    for(w=0;w<envsz;w++) {
      if(*(_env+w)== 0) {
        if(*(_env+w+1) == 0)
        break;
        else
        puts("");
      }
      else
      printf("%Fc",*(_env+w));

    }
    printf("\n\n");
  }
  else {
    p = malloc(200);
    memset(p, '\0', 200);
    l=0;
#ifdef SE_LOOKUPS
    if(n < 3) {
	q = getevar(arg[1], p, 200);
	if (q == NULL) {
	    free(p);
	    printf("'%s' no such environment variable\n", arg[1]);
	    return;
	}
	else {
	    printf("%s=%s\n", strupr(arg[1]), p);
	    free(p);
	    return;
	}
    }
#endif
    for(b=2;b<n;b++) {
#ifdef B_DEBUG
	printf("do_se:0:p:'%s' l:%d arg[%d]:'%s'\n", p, l, b, arg[b]);
#endif
	strncat(p,arg[b], 200-l);
	strcat(p," ");
	l = strlen(p);
#ifdef B_DEBUG
	printf("do_se:0:p:'%s' l:%d arg[%d]:'%s'\n", p, l, b, arg[b]);
#endif
    }
    l = strlen(p); /*l*/
#ifdef B_DEBUG
    printf("do_se:1:p:'%s' l:%d\n", p, l);
#endif
    if(p[l-1] == ' ') {
	p[l-1] = '\0';
	l--;
    }
#ifdef B_DEBUG
    printf("do_se:2:p:'%s' l:%d\n", p, l);
#endif
    if (l <= 0) p = NULL;
#ifdef B_DEBUG
    printf("do_se:3:p:'%s' l:%d\n", p, l);
#endif
    setevar(arg[1], p);
    free(p);
  }

}


/****************************************************************************/

BYTE *getevar(BYTE *varname, BYTE *value, WORD vlen)
{
    return getudata(varname, value, envseg, vlen);
}

/****************************************************************************/

BYTE *getalias(BYTE *varname, BYTE *value, WORD vlen)
{
    return getudata(varname, value, aliasseg, vlen);
}

/****************************************************************************/

BYTE *getudata(BYTE *varname, BYTE *value, WORD blockseg, WORD vlen)
{
   WORD nlen;

   BYTE  far *block = MK_FP(blockseg,0);
   BYTE far *last;
   nlen = strlen(varname);

#ifdef B_DEBUG
   printf("Looking for '%s' block at %04X:0000h vlen:%d\n", varname, blockseg, vlen);
#endif

   while(*block) {
#ifdef B_DEBUG_DISABLED
       printf("Checking if block[%d] is '=': %c\n", nlen, block[nlen]);
#endif
       if(block[nlen] == '=') { /*possible match */
#ifdef B_DEBUG
	   printf("Found possible match: '%Fs'\n", block);
#endif
	   if (_fstrnicmp(block, varname, nlen) == 0) { /*found it! */
	       block = _fstrchr(block, '=');
	       block++;
	       last = _fmemccpy(value, block, '\0', vlen);
#ifdef B_DEBUG
	       printf("Found match: '%Fs'=='%s'\n", block, varname);
#endif
	       if (last == NULL) {
		   *(value+vlen-1) = '\0';
	       }
	       else {
		   *(last-1) = 0;
	       }
#ifdef B_DEBUG
	       printf("Found match: '%s':'%s'\n", varname, value);
#endif
	       return value;
	   }
	   else { /* no match in block to next */
	       while(*(block++) != '\0');
	   }
       }
       else { /*no match */
	   while(*(block++) != '\0');
       }
   } /* block points to '\0' if end of variable area */
   /* env not found return NULL */
   return NULL;
}

/****************************************************************************/

void setevar(BYTE *varname, BYTE *value)
{
  setudata(varname, value, envseg);
}

/****************************************************************************/

void setalias(BYTE *varname, BYTE *value)
{
  setudata(varname, value, aliasseg);
}

/****************************************************************************/

BYTE setudata(BYTE *varname, BYTE *value, WORD blockseg)
{
  BYTE far *rest,*p,i, far *evalue, found=0;
  WORD w,ss,writesize,nlen,vlen,envleft,blocksz,ovlen=0,used;
  BYTE  far *block;

  block = MK_FP(blockseg,0);
  nlen = strlen(varname);
  vlen = strlen(value);
#ifdef ENV_DEBUG
  printf("setudata:0:'%s'='%s' block@%04X:0000h\n", varname, value, blockseg);
#endif

  writesize = nlen+vlen+2; /* = strings + 0 + '=' */
  strupr(varname);
  blocksz = BLOCKSZ(blockseg);

  evalue = rest = block;
#ifdef ENV_DEBUG
  printf("setudata:1:evalue=%Fp, rest=%Fp, block=%Fp\n", evalue, rest, block);
#endif

  /* calculate number of bytes used */
  used = usedbytes(blockseg, (blocksz>>4));
#ifdef ENV_DEBUG
  printf("setudata:1: block=%Fp uses %u bytes\n", block, used);
#endif
  /* used is now the amount of bytes needed by the env variables and special strings */

  /* check if var exists, if it does write new value in place of old
   if not then write new var to end of env */
  while(*rest!='\0') {
#ifdef ENV_DEBUG
      printf("setudata:2: rest=%Fs (%Fp) rest[%d] is '=': %c\n",
	     rest, rest, nlen, rest[nlen]);
#endif
    if(rest[nlen] == '=') { /*possible match */
#ifdef ENV_DEBUG
	printf("setudata:3: Found possible match: '%Fs'\n", rest);
#endif
      if(_fstrnicmp((char far *)varname, rest, nlen) == 0) { /*found it! */
	  /*save pos*/
	  evalue = rest+nlen+1; /* evalue points now at the char after the = */
#ifdef ENV_DEBUG
	  printf("setudata:4:evalue=%Fp, rest=%Fp, block=%Fp\n", evalue, rest, block);
#endif
	  /* make rest point to next var */
	  while(*(rest++)!='\0');
	  ovlen = FP_OFF(rest) - FP_OFF(evalue) - 1;
	  /*bail out */
	  found = 1;
#ifdef ENV_DEBUG
	  printf("setudata:5:evalue=%Fs %Fp, rest=%Fp, ovlen=%u\n", evalue, evalue, rest, ovlen);
#endif
	  break;
      }
      else { /* no match; point rest to next */
	  rest += nlen+1;
	  while(*(rest++));
      }
    }
    else { /*no match, go to the next */
	while(*(rest++)!='\0');
    }
  } /* rest points to '\0' if end of env */

  if(found == 0) {
      if (rest == block) {
	  /* empty environment */
	  evalue = block;
	  ovlen = 0;
	  rest++;
#ifdef ENV_DEBUG
	  printf("setudata:6a:evalue=%Fp, rest=%Fp, block=%Fp\n", evalue, rest, block);
#endif
      } else {
	  evalue = rest;
	  ovlen=0;
#ifdef ENV_DEBUG
	  printf("setudata:6b:evalue=%Fp, rest=%Fp, block=%Fp\n", evalue, rest, block);
#endif
      }
  }

#ifdef ENV_DEBUG
  printf("setudata:7:evalue=%Fp, rest=%Fp, block=%Fp\n", evalue, rest, block);
#endif

  /* calculate the number of bytes left in the env. */
  envleft = blocksz - used + ((nlen + ovlen + 2)*found);
#ifdef ENV_DEBUG
  printf("setudata:8:envleft=%u, blocksz=%u, used=%u, writesize=%u vlen=%u ovlen=%d\n",
	 envleft, blocksz, used, writesize, vlen, ovlen);
#endif

  /* check if new value is of same size as old */
  if (vlen == ovlen) {
      _fstrncpy(evalue, (char far *)value, vlen);
#ifdef ENV_DEBUG
      printf("setudata:=:evalue=%Fs (%Fp), value='%s', vlen=%u\n",
	     evalue, evalue, value, vlen);
#endif
      return 0;
  }

  if(writesize > envleft) {
    printf("Memoryblock at segment 0x%x is to small to fit variable\n", blockseg);
    return 1;
  }

  /*calc save size */
  ss = used - FP_OFF(rest);
#ifdef ENV_DEBUG
  printf("setudata:9:envleft=%u, writesize=%u ss=%u used=%u rest:%Fp\n",
	 envleft, writesize, ss, used, rest);
#endif

  /*save rest of environment*/
  p=malloc(ss);
  if (p == NULL) {
      puts("setudata: Out of memory");
      return 1;
  }
  _fmemcpy((char far *)p, rest, ss);
#ifdef ENV_DEBUG
  printf("setudata:A:copied %u bytes from %Fp\n",
	 ss, rest);
#endif

  if((found == 1) && (value == NULL)) {
      /*the env var will be owerwritten with rest.*/
      /*rewind evalue to beginning of var name */
#ifdef ENV_DEBUG
      printf("setudata:B:rest:%Fs (%Fp) evalue=%Fp nlen=%u vername=%s\n",
	     rest, rest, evalue, nlen, varname);
#endif
      evalue -= nlen + 1;
      rest = evalue;
  }
  else { /*write value to environment. evalue points*/
      if (found == 0) {
	  if (evalue == block) {
	      rest = block;
	  }
	  /* write env name and = before value, update evalue pointer */
	  _fmemcpy(rest, (char *)varname, nlen);
	  rest += nlen;
	  *rest = '=';
	  rest++;
	  evalue=rest;
#ifdef ENV_DEBUG
	  printf("setudata:C:rest:%Fs (%Fp) evar=%Fp\n", rest, rest, evalue);
#endif
      }
      _fmemcpy(evalue, (char far *)value, vlen+1); /* also copy the terminating \0 */
      evalue += vlen+1;
      rest = evalue;
  }
#ifdef ENV_DEBUG
  printf("setudata:10:envleft=%u, writesize=%u ss=%u used=%u rest:%Fp\n",
	 envleft, writesize, ss, used, rest);
#endif

  /*put the rest back*/
  _fmemcpy(rest, (char far *)p, ss);

  free(p);

  return 0;
}

/****************************************************************************/

WORD mkudata(WORD oldseg, WORD *nseg, WORD bsz, WORD nbsz, BYTE *comspec)
{
    WORD w, minspace;
    BYTE far *p,far *s,far *d;

#ifdef ENV_DEBUG
    printf("mkudata():0:oldseg=%04Xh nseg=%p bsz=%04Xh nbsz=%04Xh\n",
	   oldseg, nseg, bsz, nbsz);
#endif
    if(oldseg > 0) {
	minspace = usedbytes(oldseg, bsz);
	if((nbsz << 4) < minspace) {
	    printf("WARNING: "
		   "Block @ %04Xh:0000 won't fit in %u bytes! "
		   "%u < %u - adjusting\n", oldseg, (nbsz << 4),
		   (nbsz << 4), minspace);
	    nbsz = (minspace >> 4) + 1; /* +1 paragraph to make sure we have enough */
	}
    }
#ifdef ENV_DEBUG
    printf("mkudata():0:oldseg=%04Xh nseg=%p bsz=%04Xh nbsz=%04Xh\n",
	   oldseg, nseg, bsz, nbsz);
#endif
    w = nbsz;
#ifdef ENV_DEBUG
    printf("mkudata():1:myallocmem(%p(%04Xp), %p)\n",&w,w,nseg);
#endif
    while(myallocmem(&w, nseg) == 1) {
#ifdef ENV_DEBUG
	printf("mkudata():2a:myallocmem FAIL nseg=%p nbsz=%04Xh\n",nseg,nbsz);
#endif
	return 0;
    }
    nbsz = w;
#ifdef ENV_DEBUG
    printf("mkudata():2b:myallocmem OK nseg=%04Xh(%p) nbsz=%04Xh\n",*nseg,nseg,nbsz);
#endif

  if(oldseg==0 || bsz==0) {
      p = MK_FP(*nseg,0); /* point to beg of block*/
#ifdef ENV_DEBUG
      printf("mkudata():3:p=%Fp (%04X:0000)\n", p, *nseg);
#endif
      p[0] = 0;
      p[1] = 0;
      if (comspec != NULL) {
	  p[2] = 1;
	  p[3] = 0;
	  _fmemcpy(&p[4], (char far *)comspec, strlen(comspec)+1);
      }
      else {
	  p[2] = 0;
	  p[3] = 0;
      }
      return nbsz;
  }
  /* copy stuff from oldseg to newseg */
  s = MK_FP(oldseg,0);
  d = MK_FP((*nseg),0);
#ifdef ENV_DEBUG
    printf("mkudata():4:oldseg=%04Xh bsz=%04Xh\n", oldseg, bsz);
    printf("mkudata():4:newseg=%04Xh nbsz=%04Xh\n", *nseg, nbsz);
#endif
    _fmemcpy(d, s, minspace);
#ifdef ENV_DEBUG
    printf("mkudata():6:copied %u bytes from %Fp to %Fp\n",minspace, s, d);
#endif

  myfreemem(oldseg);
  return nbsz;
}

/****************************************************************************/

WORD usedbytes(WORD block, WORD bsz)
{
    WORD i, e;
    char far *p;
    WORD mbsz, blocksize=0, l, n=0;
#ifdef ENV_DEBUG
    WORD envsize=0;
#endif

    mbsz = BLOCKSZ(block) >> 4;
    p = MK_FP(block, 0);
#ifdef ENV_DEBUG
    printf("usedbytes:0:Block @ %Fp size %04Xh =? %04Xh bytes\n", p, bsz, mbsz);
#endif
    if(bsz > mbsz) {
	printf("WARNING WARNING SUS block size! %04Xh > %04Xh bytes\n", bsz, mbsz);
	bsz = mbsz;
    }
    while(*p != '\0') {
	l = _fstrlen(p);
#ifdef ENV_DEBUG_DISABLED
	printf("usedbytes:1:%Fs (%04Xh)\n",p, l);
#endif
	p+=l+1; /* point to the next string, or NUL if end of block */
	n++; /* count strings, because why not */
	blocksize += l + 1; /* the NUL char also takes up memory space */
    }
    blocksize++; /* the last NUL */
#ifdef ENV_DEBUG
    envsize=blocksize;
#endif
    /* look for the additional strings after the env area */
    p++; /* point to the number of extra strings */
    e = *((WORD far *)p);
#ifdef ENV_DEBUG
    printf("usedbytes:2:p=%d (%Fp)\n", e, p);
#endif
    p+=2; /* point to the first extra string */
    blocksize += 2;
    for(i=0; i < e; i++) {
	l = _fstrlen(p);
#ifdef ENV_DEBUG_DISABLED
	printf("'%Fs' (%04Xh)\n", p, l);
#endif
	p+=l+1; /* point to the next string */
	blocksize+=l+1; /* the NUL char also takes up memory space */
    }
#ifdef ENV_DEBUG
    printf("Block @ %04X:0000 has %u strings and uses %u bytes.\n", block, n, envsize);
    printf("Block also has %d system strings and uses %u bytes total\n", e, blocksize);
#endif
    return blocksize;
}

/****************************************************************************/
BYTE myallocmem(WORD *sz, WORD *seg) {
    WORD s,e,m;

#ifdef DOG_DEBUG
    printf("alloc():1:Trying to allocate %04Xh paragraphs of data\n", *sz);
#endif
    s = *sz;
    asm clc;
    asm mov ah,48h;    /* DOS Alloc mem */
    asm mov bx,s;
    asm int 21h;
    asm jc mam_error;
    asm mov s, ax;
#ifdef DOG_DEBUG
    printf("alloc():2:Allocated %04Xh paragraphs of data in seg %04Xh(%p)\n", *sz, s, seg);
#endif
    *seg = s;
    return 0;

mam_error:
    asm mov m,bx;
    asm mov e,ax;

    printf("Error(%x): Only %u bytes (%u paragraphs) available\n",e,m<<4,m);

    *sz = m;
    return 1;
}

/****************************************************************************/
BYTE myfreemem(WORD segment) {

    WORD s,e,m;
    asm clc;
    asm mov ah,49h;       /* DOS Free mem */
    asm mov bx, segment;
    asm mov es, bx;
    asm int 21h;
    asm jc mfm_error;

    return 0;

mfm_error:
    asm mov m,bx;
    asm mov e,ax;

    printf("Error(%x): Cannot free segment %04Xh\n", e, segment);

    return 1;
}

/****************************************************************************/

BYTE aliasreplace(BYTE *com)
{
  BYTE acandid[200], i, *p, *c;
  BYTE aliasvalue[200];

  c = com;

  for(i=0;(i<200) && (isfchar(com[i]));i++) {
      acandid[i] = *(c++);
  }
  acandid[i] = '\0';

  p = getalias(acandid,aliasvalue,200);
  if (p != NULL) {
      strncat(aliasvalue,c,200);
      strncpy(com,aliasvalue, 200);
    return 0;
  }
  return 0xFF;
}

/****************************************************************************/

void evarreplace(BYTE *com, BYTE ln)
{
  BYTE tmp[2],eval[200], evar[50], i, j, *p, *c;
  BYTE newcom[200];

  memset(newcom,0,200);

  for(i=0;i<ln;i++) {
    if(com[i] == '%') {
      if( com[i+1] == '%' ) {
        strcat(newcom,"%");
#ifdef B_DEBUG_DISABLED
        printf("evarreplace:0:newcom[%u]=(%c) newcom[%u]=(%c)\n",i,newcom[i],i+1,newcom[i+1]);
#endif
        i++;
      }
      else if(isdigit(com[i+1])) {
        for(j=0,i++;isdigit(com[i]);i++) {
          j = com[i] - '0' + (10 * j);
        }
        if (j > 19) j=19;
        strcat(newcom, varg[j]);
#ifdef B_DEBUG
        printf("evarreplace:1:j = %d varg[%d]=(%s) newcom=(%s)\n",j,j,varg[j],newcom);
#endif
      }
      else {
        i++;
        memset(eval,0,200);
        memset(evar,0,50);
        for(j=0;(com[i] != '%') && (i<ln);j++,i++) {
          evar[j] = com[i];
#ifdef B_DEBUG_DISABLED
          printf("evarreplace:2-0:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
        }
#ifdef B_DEBUG_DISABLED
          printf("evarreplace:2-1:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
        evar[j]= '\0';
/*        i++; */
#ifdef B_DEBUG_DISABLED
          printf("evarreplace:2-2:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
	  getevar(evar,eval,200); /* replace %varname% with value*/
        strcat(newcom,eval);
#ifdef B_DEBUG_DISABLED
        printf("evarreplace:3:evar=(%s) eval=(%s) newcom=(%s) com[%u]=(%c)\n",evar,eval,newcom,i,com[i]);
#endif
      }
    }
    else {
      tmp[0]=com[i];
      tmp[1]=0;
#ifdef B_DEBUG
        printf("evarreplace:4:com[%d]=(%c)\n",i,com[i]);
#endif
      strcat(newcom,tmp);
    }
  }

#ifdef B_DEBUG
        printf("evarreplace:5:com=(%s)\n",com);
#endif
  strcpy(com,newcom);
#ifdef B_DEBUG
        printf("evarreplace:6:com=(%s)\n",com);
#endif
  return;
}

/****************************************************************************/

void get_block_owner(WORD seg, BYTE *owner, BYTE size)
{
    BYTE far *p;
    WORD e, l;

    p = MK_FP(seg, 0);
    while(*p != '\0') {
	l = _fstrlen(p);
#ifdef ENV_DEBUG_DISABLED
	printf("get_block_owner:1:%Fs (%04Xh)\n",p, seg);
#endif
	p+=l+1; /* point to the next string, or NUL if end of block */
    }
    /* p points to end of env now */
    p++; /* point to the number of extra strings */
    p+=2; /* point to the first extra string */
#ifdef ENV_DEBUG
    printf("get_block_owner:2:p=%Fs (%Fp)\n", p, p);
#endif
    _fstrncpy((char far*)owner, p, size);
    return;
}
