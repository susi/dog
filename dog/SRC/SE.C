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

**************************************************************************/

void do_se( BYTE n)
{
    BYTE b,*p;
    WORD w;

    if(n == 1) {
        printf("env @ %Fp %u(%xh) bytes\n",_env,envsz,envsz);
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
        if (strlen(p) == 0) p = NULL;
        setevar(arg[1],p);
        free(p);
    }
    
}


/****************************************************************************/

BYTE *getevar(BYTE *varname, BYTE *value)
{
    BYTE *p,*q,ev[512];
    WORD nlen;

    p=varname;
    q=value;
    nlen = strlen(p);

    while(*_env) {
        if(*(_env+nlen) == '=') { /*possible match */
            *(_env+nlen) = '\0';
            sprintf(ev,"%Fs",_env);
            *(_env+nlen) = '=';
            if(stricmp(varname,ev) == 0) { /*found it! */
                _env += nlen+1;
                while(*_env != '\0') {
                    *(q++) = *(_env++);
                }
                /* restore _env */
                *q='\0';
                _env=MK_FP(envseg,0);
                return value;
            }
            else { /* no match in _env to next */
                while(*(_env++) != '\0');
            }
        }
        else { /*no match */
            while(*(_env++) != '\0');
        }
    } /* _env points to '\0' if end of env */

    /* env not found return NULL */
    /* restore _env */
    _env=MK_FP(envseg,0);
    return NULL;
}

/****************************************************************************/

void setevar(BYTE *varname, BYTE *value)
{
    BYTE t[512],far *rest,*p,*b,i,far *eoe, far *evalue,found=0;
    WORD w,ss,writesize,nlen,envleft;

    nlen = strlen(varname);
    writesize = strlen(varname)+strlen(value)+2; /* = strings + 0 + '='*/
    b=malloc(writesize);
    strupr(varname);
    strcpy(b,varname);
    strcat(b,"=");
    strcat(b,value);
    envsz = peek(envseg-1,3) << 4;

    evalue = eoe = rest = _env;


    /* make eoe point to last byte of used env */

    for(w=0;w<envsz;w++,eoe++) {
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


    while(*rest!='\0') {
        if(*(rest+nlen) == '=') { /*possible match */
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
    if(found==0)
        evalue = rest;

    sprintf(t,"%Fs",evalue);
    /* calculate the number of bytes left in the env. */
    envleft = envsz - FP_OFF(eoe) + (found * strlen(t));

    if(writesize > envleft) {
        puts("Out of environment space");
        return;
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
        for(w=0;w<writesize;w++) {
            *(evalue+w) = *(b+w);
        }
    }
    evalue += w;

    /*put the rest back*/
    for(w=0;w<ss;w++) {
        *(evalue+w) = *(p+w);
    }

    free(p);
    free(b);

    return;
}
