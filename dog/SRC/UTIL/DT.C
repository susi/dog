/*
DT.C - version 1.0
    Get and/or set date and time.

Copyright (C) 2000  Wolf Bergenheim

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

This program is part of DOG - The DOG Operating Ground

History

30.11.99 - Created by Wolf Bergenheim
12.03.00 - Added DOSCC struct to fix program -WB

*/
#include "util.h"

#define NEITHER 0
#define TIME 1
#define DATE 2
#define BOTH 3
#undef DWORD
#define DWORD unsigned char far *

struct DOSCC {
    WORD dtf;       /* Day and Time Format 0=USA, 1=Europe 2=Japan*/
    BYTE mk[5];     /* currency string */
    BYTE Ms[2];     /* thousand separator */
    BYTE ds[2];     /* decimal separator */
    BYTE dts[2];    /* date separator */
    BYTE tms[2];    /* time separator*/
    BYTE csf;       /* currency symbol format*/
    BYTE ndd;       /* digits after decimal */
    BYTE tf;        /* timeformat 0=12h 1=24h */
    DWORD cmca;     /* case map call address */
    BYTE dls[2];    /* datalist separator */
    BYTE reserved[10];
}ccb;

BYTE mm=0,dy=0,wk=0,h=0,m=0,s=0,ms=0, str[50]={0};
WORD off=0,seg=0,yy=0,cc=0;
static BYTE Day[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static BYTE Time[2][3] = {"am","pm"};

int get_sw(int nargs, char *arg[]);
void put_tm(void);
void put_dt(void);
void set_tm(void);
void set_dt(void);
BYTE parse_tm(void);
BYTE parse_dt(void);

int main(int nargs, char *arg[])
{
    int set;
    if(nargs == 1) {
        put_tm();
        put_dt();
    }
    else {
        set = get_sw(nargs,arg);
        if((set & TIME) == TIME){
            put_tm();
            set_tm();
        }
        if((set & DATE) == DATE){
            put_dt();
            set_dt();
        }
    }
    return 0;
}

#pragma argsused /* no complaints from the compiler */
int get_sw(int nargs, char *arg[])
{
    switch(toupper(arg[1][1])) {
        case '?':
        case 'H':
        default :
            printf("Usage: %s [-[s|d|t]]\ns = set date AND time\nd = set DATE\nt = set TIME\n",arg[0]);
            return NEITHER;
        case 'D':
            return DATE;
        case 'S':
            return BOTH;
        case 'T':
            return TIME;
    }
}

void put_dt(void)
{
    asm{
        MOV AH,2Ah
        INT 21h         ;/*get DATE*/
        MOV wk,AL
        MOV dy,DL
        MOV mm,DH
        MOV yy,CX
        MOV DX, offset ccb
        MOV AX,3800h
        INT 21h         ;/*get country info.*/
        MOV cc,BX
    }

    switch(ccb.dtf) {
        case 0:
            printf("%s %02d%s%02d%s%d\n",Day[wk],mm,ccb.dts,dy,ccb.dts,yy);
            break;
        case 1:
            printf("%s %02d%s%02d%s%d\n",Day[wk],dy,ccb.dts,mm,ccb.dts,yy);
            break;
        case 2:
            printf("%s %d%s%02d%s%02d\n",Day[wk],yy,ccb.dts,mm,ccb.dts,dy);
            break;
    }
    return;
}

void put_tm(void)
{
    asm{
        MOV AH,2Ch
        INT 21h         ;/*GET TIME*/
        MOV h,CH
        MOV m,CL
        MOV s,DH
        MOV _ms,DL
        MOV DX, offset ccb
        MOV AX,3800h
        INT 21h         ;/*get country info.*/
        MOV cc,BX
    }

    if((ccb.tf&1) == 0) {
        if(h==0) h = 12;
        printf("%02d%s%02d%s%02d%c%02d%s\n",(h>12)?h-12:h,ccb.tms,m,ccb.tms,s,(ccb.dtf==0)?'.':',',ms,(h>12)?Time[1]:Time[0]);
    }
    else
        printf("%02d%s%02d%s%02d%c%02d\n",h,ccb.tms,m,ccb.tms,s,(ccb.dtf==0)?'.':',',ms);

    return;
}

void set_dt(void)
{
    BYTE res;

    while(1) {
        printf("Enter new date ");
        switch(ccb.dtf) {
            case 0:
                printf("(mm%sdd%syy): ",ccb.dts,ccb.dts);
                break;
            case 1:
                printf("(dd%smm%syy): ",ccb.dts,ccb.dts);
                break;
            case 2:
                printf("(yy%smm%sdd: ",ccb.dts,ccb.dts);
                break;
        }
        res = parse_dt();
        if(res==0x0FF) {
            puts("Invalid date.\n");
        }
        else {
            break;
        }
    }
    return;
}

void set_tm(void)
{
    BYTE res;

    while(1) {
        printf("Enter new time: ");
        res = parse_tm();
        if(res == 0x0FF)
            puts("Invalid time.\n");
        else
            break;
    }
    return;
}


BYTE parse_dt(void)
{
    BYTE *p,l,i;

    p=gets(str);
    i=0;
    l=strlen(str);
    if(l==0) return 0;
    dy =0; mm=0;yy=0;
    switch(ccb.dtf) {
        case 0:
            while((isdigit(*p))&&(p<str+l)) mm=mm*10+(*p++)-'0';
            p++;
            while((isdigit(*p))&&(p<str+l)) dy=dy*10+(*p++)-'0';
            p++;
            while((isdigit(*p))&&(p<str+l)) {
                yy=yy*10+(*p++)-'0';
                i++;
            }
            p++;
            break;
        case 1:
            while((isdigit(*p))&&(p<str+l)) dy=dy*10+(*p++)-'0';
            p++;
            while((isdigit(*p))&&(p<str+l)) mm=mm*10+(*p++)-'0';
            p++;
            while((isdigit(*p))&&(p<str+l)) {
                yy=yy*10+(*p++)-'0';
                i++;
            }
            p++;
            break;
        case 2:
            while((isdigit(*p))&&(p<str+l)) {
                yy=yy*10+(*p++)-'0';
                i++;
            }
            p++;
            while((isdigit(*p))&&(p<str+l)) mm=mm*10+(*p++)-'0';
            p++;
            while((isdigit(*p))&&(p<str+l)) dy=dy*10+(*p++)-'0';
            p++;
            break;
    }

    if((dy >31) || (mm>12)) return 0x0FF;
    switch(i) {
        case 1:
            yy +=2000;
            break;
        case 2:
            yy += (yy<80)?2000:1900;
            break;
        case 4:
            if((yy<1980)||(yy>2099)) return 0x0FF;
            break;
        default :
            return 0x0FF;
    }

    asm{
        MOV AX,2B00h
        MOV CX,yy
        MOV DH,mm
        MOV dL,dy
        INT 21h
        MOV i,AL
    }
    return i;
}



BYTE parse_tm(void)
{
    BYTE *p,l,i;

    p=gets(str);
    i=0;
    l=strlen(str);
    if(l==0) return 0;
    h=0; m=0; s=0; ms=0;

    while((isdigit(*p))&&(p<str+l)) h = h * 10 + (*p++) -'0';
    p++;    /* pass delimiter */
    while((isdigit(*p))&&(p<str+l)) m = m * 10 + (*p++) -'0';
    p++;    /* pass delimiter */
    while((isdigit(*p))&&(p<str+l)) s = s * 10 + (*p++) -'0';
    p++;    /* pass delimiter */
    while((isdigit(*p))&&(p<str+l)) ms = ms * 10 + (*p++) - '0';
    if (toupper(*p) == 'P') h+=12;
    else if(h == 12) h=0;

    if((h >23) || (m>60) || (s>60) || (ms>99)) return 0x0FF;

    asm{
        MOV AX,2D00h
        MOV CH,h
        MOV CL,m
        MOV DH,s
        MOV dL,ms
        INT 21h         ;/*set time*/
        MOV i,AL
    }
    return i;
}


