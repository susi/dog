/*
DOG.C  -  Alternate command processor for (currently) MS-DOS ver 3.30

Copyright (C) 1999  Wolf Bergenheim

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

Contact author: email: eugenetswong@yahoo.com

History

29.09.99 - Eugene Wong took code from dog.c before it was removed
         - made with Pacific C; at time of writing this is 
             - portable to Turbo C 2.01 (freeware not public domain)
             - which can be found at the Borland Museum at
             - community.borland.com/museum/
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>
#include <io.h>


int		year;	/* year */
char	month,	/* month */
		day,	/* day */
		hour,	/* hours */
		minute,	/* minutes */
		second,	/* seconds */
		hs,	/* hundreths seconds */
		i;


getCurDate(){
	union REGS regs;

	regs.h.ah = 0x2a;
	int86(0x21,&regs,&regs);
	year = regs.x.cx;
	month = regs.h.dh;
	day = regs.h.dl;
}

setDate(){
	union REGS regs;

	regs.h.ah = 0x2b;
	regs.x.cx = year;
	regs.h.dh = month;
	regs.h.dl = day;
	int86(0x21,&regs,&regs);
	i = regs.h.al;	/* 00 = successful; ff = fail */
}

getCurTime(){
	union REGS regs;

	regs.h.ah = 0x2c;
	int86(0x21,&regs,&regs);
	hour = regs.h.ch;
	minute = regs.h.cl;
	second = regs.h.dh;
	hs = regs.h.dl;
}

setTime(){
	union REGS regs;

	regs.h.ah = 0x2d;
	regs.h.ch = hour;
	regs.h.cl = minute;
	regs.h.dh = second;
	int86(0x21,&regs,&regs);
	i = regs.h.al;
}

dispDate(){
    printf("Current date is %04d.%02d.%02d\n", year, month, day);
}

dispTime(){
	printf("Current time is %d.%02d.%02d,%02d\n", hour, minute, second, hs);
}

dispStopWatch(long seconds){
seconds++;
}


main(argc, argv)
{
	getCurDate();
	getCurTime();
	dispDate();
	dispTime();
	
}




/*****************************************************
 *****************************************************
 *****************************************************
 * any code below here is just kept here for reference
 * to add features and functions not already in the
 * program
 *****************************************************
 *****************************************************
 *****************************************************/


/*****************************************************/

void do_tm(n)short n;
{
    short i=0;
    short int *p,
    	str[50];

/* get time */
/*    MOV ah,2ch
    INT 21h
    MOV h,ch
    MOV min,cl
    MOV sec,dh
    MOV ohs,dl
#endasm
*/
    if (n==1) dispTime();

    else if (n==2) {
/*
        if (stricmp(arg[1],"-s") == 0) {
            dispTime();
            printf("Enter new time: ");
            getln(str,50);
            if(cBreak) {
                cBreak = 0;
                return;
            }
            i = parse_time(str);
            if (i == 0)
                return;
            while(i == 0x0ff) {
                puts("Invalid time!!");
                printf("Enter new time: ");
                if(cBreak == 1) {
                    cBreak = 0;
                    return;
                }
                getln(str,50);
                i = parse_time(str);
            }
            return;
        }
        else {
            puts("Invalid switch / argument.");
            return;
        }
*/
    }
    else {
        puts("Invalid number of arguments.");
        return;
    }

}

/*****************************************************/
/*
    Improved version By Eugene Wong.
*/

short parse_time(short *s)
{
    short i,hour,min,sec,hsec;
    hour = 25;
    min = 61;
    sec = 61;
    hsec = 101; /* hundreth seconds */

    if(strlen(s) == 0) return 0;

    while (isdigit(*s)) hour = (hour) * 10 + (*s++) - '0';
    s++;  /* pass colon or period */
    while (isdigit(*s)) min  = (min)  * 10 + (*s++) - '0';
    s++;  /* pass colon or period */
    while (isdigit(*s)) sec  = (sec)  * 10 + (*s++) - '0';
    s++;  /* pass colon or period */
    while (isdigit(*s)) hsec = (hsec) * 10 + (*s++) - '0';
    if ((*s == 'P') || (*s == 'p')) hour += 12;
    else if (hour == 12) hour = 0;
    if (hour > 23 || min > 60 || sec > 60 || hsec > 99) return 0x0ff;
/*
#asm
#endasm
*/
    return i;
}

/*****************************************************/

void do_dt(short n)
{
    short yr;
    short mo,d,*p,str[50],i;
/*
#asm
	MOV ah,2ah
	INT 21h
	OV yr,CX
	OV mo,DH
	OV d,DL
#endasm
*/
    if(n == 1) {
	    dispDate();
    }
    else if (n==2) {
/*
        if (stricmp(arg[1],"-s") == 0) {
	        dispDate();
            printf("Enter new date (dd-mm-yy): ");
            getln(str,50);
            if(cBreak) {
                cBreak = 0;
                return;
            }
            i = parse_date(str);
            if (i == 0)
                return;
            while(i == 0x0ff) {
                puts("Invalid date!!");
                printf("Enter new date (dd-mm-yy): ");
                if(cBreak == 1) {
                    cBreak = 0;
                    return;
                }
                getln(str,50);
                i = parse_time(str);
            }
            return;
        }
        else {
            puts("Invalid switch / argument.");
            return;
        }
*/
    }
    else {
        puts("Invalid number of arguments.");
        return;
    }



}

/*****************************************************/

short parse_date(short *s)
{
    short i,day,month;
    short year;
    day = 32;
    month = 13;
    year = 3000;
    i=0;

    if(strlen(s) == 0) return 0;

    while (isdigit(*s)) day = (day) * 10 + (*s++) - '0';
    s++;  /* pass hyphen or dot */
    while (isdigit(*s)) month  = (month)  * 10 + (*s++) - '0';
    s++;  /* pass hyphen or dot */
    while (isdigit(*s)) {
        year  = (year)  * 10 + (*s++) - '0';
        i++;
   }

    if (day > 31 || month > 12) return 0x0ff;
    switch(i) {
        case 1:
            year += 2000;
            break;
        case 2:
            if(year <80) year += 2000;
            else year += 1900;
            break;
        case 4:
            if((year < 1980) || (year > 2099)) return 0x0ff;
            break;
        default:
            return 0x0ff;
    }
/*
#asm
#endasm
*/
	return i;
}
