/*

SE.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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
**************************************************************************/

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
    printf("do_se:1:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if(*(p+(strlen(p)-1)) == ' ') *(p+(strlen(p)-1)) = '\0';
#ifdef b_debug    
    printf("do_se:2:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if (strlen(p) == 0) p = NULL;
    setalias(arg[1],p);
    free(p);
  }
  
}

/**************************************************************************/

void do_se( BYTE n)
{
  BYTE b,*p;
  WORD w;
  
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
    p[0] = '\0';
    for(b=2;b<n;b++) {
      strcat(p,arg[b]);
      strcat(p," ");
    }
#ifdef b_debug    
    printf("do_se:1:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if(*(p+(strlen(p)-1)) == ' ') *(p+(strlen(p)-1)) = '\0';
#ifdef b_debug    
    printf("do_se:2:p(last)=(%c)\n",*(p+(strlen(p)-1)));
#endif
    if (strlen(p) == 0) p = NULL;
    setevar(arg[1],p);
    free(p);
  }
  
}


/****************************************************************************/

BYTE *getevar(BYTE *varname, BYTE *value)
{
  return getudata(varname, value, envseg);
}

/****************************************************************************/

BYTE *getalias(BYTE *varname, BYTE *value)
{
  return getudata(varname, value, aliasseg);
}

/****************************************************************************/

BYTE *getudata(BYTE *varname, BYTE *value, WORD blockseg)
{
  BYTE *p,*q,ev[512];
  WORD nlen;
  
  BYTE  far *block = MK_FP(blockseg,0);
  
  p=varname;
  q=value;
  nlen = strlen(p);
  
  while(*block) {
    if(*(block+nlen) == '=') { /*possible match */
      *(block+nlen) = '\0';
      sprintf(ev,"%Fs",block);
      *(block+nlen) = '=';
      if(stricmp(varname,ev) == 0) { /*found it! */
        block += nlen+1;
        while(*block != '\0') {
          *(q++) = *(block++);
        }
        *q='\0';
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
  WORD w,ss,writesize,nlen,envleft,blocksz;
  BYTE  far *block;
  
  block = MK_FP(blockseg,0);
  nlen = strlen(varname);
  writesize = strlen(varname)+strlen(value)+2; /* = strings + 0 + '='*/
  b=malloc(writesize);
  strupr(varname);
  strcpy(b,varname);
  strcat(b,"=");
  if (value != NULL) strcat(b,value);
#ifdef b_debug
  printf("b=(%s)\n",b);
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
  
  while ( (w = allocmem(nbsz,nseg)) != 0xFFFF) {
    nbsz = w;
    fprintf(stderr,"Insufficient memory to allocate!\nReallocating\n");
  }
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

BYTE aliasreplace(BYTE *com)
{
  BYTE acandid[200], i, *p, *c;
  BYTE aliasvalue[200];
  
  c = com;
  
  for(i=0;(i<200) && (isfchar(com[i]));i++) {
      acandid[i] = *(c++);
  }
  acandid[i] = '\0';
  
  p = getalias(acandid,aliasvalue);
  if (p != NULL) {
    strcat(aliasvalue,c);
    strcpy(com,aliasvalue);
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
        getevar(evar,eval); /* replace %varname% with value*/
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

