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
**************************************************************************/
#include <mem.h>

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
#ifdef b_debug
    printf("do_al:1:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if(*(p+(strlen(p)-1)) == ' ') *(p+(strlen(p)-1)) = '\0';
#ifdef b_debug
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
    printf("environment @ %Fp %u(%xh) bytes\n",_env,envsz,envsz);
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
#ifdef se_lookups
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
#ifdef b_debug
	printf("do_se:0:p:'%s' l:%d arg[%d]:'%s'\n", p, l, b, arg[b]);
#endif
	strncat(p,arg[b], 200-l);
	strcat(p," ");
	l = strlen(p);
#ifdef b_debug
	printf("do_se:0:p:'%s' l:%d arg[%d]:'%s'\n", p, l, b, arg[b]);
#endif
    }
    l = strlen(p); /*l*/
#ifdef b_debug
    printf("do_se:1:p:'%s' l:%d\n", p, l);
#endif
    if(p[l-1] == ' ') {
	p[l-1] = '\0';
	l--;
    }
#ifdef b_debug
    printf("do_se:2:p:'%s' l:%d\n", p, l);
#endif
    if (l <= 0) p = NULL;
#ifdef b_debug
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

#ifdef b_debug
   printf("Looking for '%s' block at %04X:0000h vlen:%d\n", varname, blockseg, vlen);
#endif


   while(*block) {
#ifdef b_debug
       printf("Checking if block[%d] is '=': %c\n", nlen, block[nlen]);
#endif
       if(block[nlen] == '=') { /*possible match */
#ifdef b_debug
	   printf("Found possible match: '%Fs'\n", block);
#endif
	   if (_fstrnicmp(block, varname, nlen) == 0) { /*found it! */
	       block = _fstrchr(block, '=');
	       block++;
	       last = _fmemccpy(value, block, '\0', vlen);
#ifdef b_debug
	       printf("Found match: '%Fs'=='%s'\n", block, varname);
#endif
	       if (last == NULL) {
		   *(value+vlen-1) = '\0';
	       }
	       else {
		   *(last-1) = 0;
	       }
#ifdef b_debug
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
  BYTE t[512],far *rest,*p,*b,i,far *eoe, far *evalue,found=0;
  WORD w,ss,writesize,nlen,vlen,envleft,blocksz;
  BYTE  far *block;

  block = MK_FP(blockseg,0);
  nlen = strlen(varname);
  vlen = strlen(value);
#ifdef b_debug
  printf("setudata:0:'%s'='%s' block@%04X:0000h\n", varname, value, blockseg);
#endif

  writesize = nlen+vlen+2; /* = strings + 0 + '=' */
  b=malloc(writesize);
  memset(b, '\0', writesize);
#ifdef b_debug
  printf("setudata:1:writesize=%d\n", writesize);
#endif

  strupr(varname);
#ifdef b_debug
  printf("setudata:2:b:'%s', varname='%s'\n", b, varname);
#endif
  strncpy(b,varname,nlen);
#ifdef b_debug
  printf("setudata:3:b:'%s', varname='%s'\n", b, varname);
#endif
  b[nlen] = '=';
#ifdef b_debug
  printf("setudata:4:b:'%s', varname='%s'\n", b, varname);
#endif
#ifdef b_debug
  printf("setudata:5:b:'%s', varname='%s'\n", b, varname);
#endif

  if (value != NULL) strcat((b+nlen+1),value);
#ifdef b_debug
  printf("setudata:6:b:'%s', value='%s'\n", b, value);
#endif
  blocksz = peek(blockseg-1,3) << 4;

  evalue = eoe = rest = block;


  /* make eoe point to last byte of used env */

  for(w=0;w<blocksz;w++,eoe++) {
    if(*eoe== 0) {
      if(*(++eoe) == 0) {
        i = *(++eoe);
        eoe+=2;
        while(i>0) {
          while(*(eoe++) != '\0');
          i--;
        }

        break;
      }

    }
    else {
    }
  }

  /* check if var exists, if it does write new value in place of old
   if not then write new var to end of env */

#ifdef b_debug
    printf("rest=%Fp\n",rest);
#endif

  while(*rest!='\0') {
    if(*(rest+nlen) == '=') { /*possible match */
#ifdef b_debug
    printf("rest+nlen=%Fp\n",(rest+nlen));
#endif
      *(rest+nlen) = '\0';
      sprintf(t,"%Fs",rest);
      *(rest+nlen) = '=';
      if(stricmp(varname,t) == 0) { /*found it! */
        /*save pos*/
        evalue = rest;
        /* make rest point to next var */
        while(*(rest++)!='\0');
        /*bail out */
        found = 1;
        break;
      }
      else { /* no match; point rest to next */
        while(*(rest++));
      }
    }
    else { /*no match */
      while(*(rest++)!='\0');
    }
  } /* rest points to '\0' if end of env */
  if(found==0) evalue = rest;

#ifdef b_debug
    printf("evalue=%Fp\n",evalue);
#endif

  sprintf(t,"%Fs",evalue);
  /* calculate the number of bytes left in the env. */
  envleft = blocksz - FP_OFF(eoe) + (found * strlen(t));

  if(writesize > envleft) {
    printf("Memoryblock at segment 0x%x is to small to fit variable\n", blockseg);
    return 1;
  }

  /*calc save size */
  ss = FP_OFF(eoe) - FP_OFF(rest) + 1;

  /*save rest of environment*/
  p=malloc(ss);
  for(w=0;w<ss;w++) {
    *(p+w) = *(rest+w);
  }

  if((found == 1) && (value == NULL)) {
        w=0;
    /*the env var will be owerwritten with rest.*/
  }
  else { /*write value to environment. evalue points*/
#ifdef b_debug
    printf("b=(%s)\n",b);
#endif
    for(w=0;w<writesize;w++) {
      *(evalue+w) = *(b+w);
#ifdef b_debug
      printf("*(b+w)=(%c)\n",*(b+w));
#endif
    }
  }
  evalue += w;

  /*put the rest back*/
  for(w=0;w<ss;w++) {
    *(evalue+w) = *(p+w);
  }

  free(p);
  free(b);

  return 0;
}

/****************************************************************************/

WORD mkudata(WORD oldseg, WORD *nseg, WORD bsz, WORD nbsz)
{
	WORD w;
	BYTE far *p,far *s,far *d;
  printf("nbsz = %x\n",nbsz);
  while( (w = allocmem(nbsz,nseg)) != 0xffff) {
    printf("newseg=%x nbsz=%x\n",*nseg,nbsz);
    nbsz = w;
		perror("mkudata()");
		printf("w = %x\n",w);
    printf("newseg=%x nbsz=%x\n",*nseg,nbsz);
		if(nbsz == 0) exit(-3);
  }
	if(w == 0) exit(-3);

  p = MK_FP(*nseg,0); /* point to beg of block*/
#ifdef b_debug
  printf("p=%Fp\n",p);
#endif
  *(p) = 0;
  *(++p) = 0;

  if(nbsz >= bsz) { /* copy stuff from oldseg to newseg */
    bsz <<= 4; /* bytes */
		w = *nseg;
    s = MK_FP(oldseg,0);
    d = MK_FP(w,0);

#ifdef b_debug
    printf("oldseg=%x bsz=%x\n",oldseg,bsz);
    printf("newseg=%x nbsz=%x\n",*nseg,nbsz);
#endif
    for(w=0;w<bsz;w++) {
      d[w] = s[w];
#ifdef b_debug_2
      printf("s(%Fp)->%Fc d(%Fp)->%Fc\n",s+w,*(s+w),d+w,*(d+w));
#endif
    }
  }
  freemem(oldseg);
  return nbsz;
}


/****************************************************************************/
WORD myallocmem(WORD sz, WORD *seg) {
	WORD s,e,m;
	asm clc
	asm mov ah,48h;
	asm mov bx,sz;
	asm int 21h;
	asm jc mam_error;
	asm mov s, ax;
	*seg = s;
	return 0;
	mam_error:
	asm mov m,bx;
	asm mov e,ax;

	printf("Error(%x): Only %d bytes (%x paragraphs) available\n",e,m*0x10,m);


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
#ifdef b_debug
        printf("evarreplace:0:newcom[%u]=(%c) newcom[%u]=(%c)\n",i,newcom[i],i+1,newcom[i+1]);
#endif
        i++;
      }
      else if(isdigit(com[i+1])) {
        for(j=0,i++;isdigit(com[i]);i++) {
          j = com[i] - '0' + (10 * j);
        }
        if (j > 19) j=19;
        strcat(newcom,varg[j]);
#ifdef b_debug
        printf("evarreplace:1:j = %d varg[%d]=(%s) newcom=(%s)\n",j,j,varg[j],newcom);
#endif
      }
      else {
        i++;
        memset(eval,0,200);
        memset(evar,0,50);
        for(j=0;(com[i] != '%') && (i<ln);j++,i++) {
          evar[j] = com[i];
#ifdef b_debug
          printf("evarreplace:2-0:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
        }
#ifdef b_debug
          printf("evarreplace:2-1:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
        evar[j]= '\0';
/*        i++; */
#ifdef b_debug
          printf("evarreplace:2-2:evar[%u]=(%c);com[%u]=(%c)\n",j,evar[j],i,com[i]);
#endif
	  getevar(evar,eval,200); /* replace %varname% with value*/
        strcat(newcom,eval);
#ifdef b_debug
        printf("evarreplace:3:evar=(%s) eval=(%s) newcom=(%s) com[%u]=(%c)\n",evar,eval,newcom,i,com[i]);
#endif
      }
    }
    else {
      tmp[0]=com[i];
      tmp[1]=0;
#ifdef b_debug
        printf("evarreplace:4:com[%d]=(%c)\n",i,com[i]);
#endif
      strcat(newcom,tmp);
    }
  }

#ifdef b_debug
        printf("evarreplace:5:com=(%s)\n",com);
#endif
  strcpy(com,newcom);
#ifdef b_debug
        printf("evarreplace:6:com=(%s)\n",com);
#endif
  return;
}
