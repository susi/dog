/*
    Improved version By Eugene Wong.
*/


BYTE parse_time(BYTE *s)
{
    BYTE i,hour,min,sec,hsec;
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

    asm{
        MOV ah,2Dh
        XOR al,al
        MOV CH,hour
        MOV CL,min
        MOV DH,sec
        MOV DL,hsec
        INT 21h         /*set time*/
        MOV i,AL        /*00 = sucessfull ;ff = fail*/
    }
    return i;
}

/**************************************************************************/

void do_tm(BYTE n)
{
    BYTE i=0;
    BYTE *p,str[50],h,min,sec,ohs;

    asm {
        MOV ah,2ch
        INT 21h
        MOV h,ch
        MOV min,cl
        MOV sec,dh
        MOV ohs,dl
    }
    if (n==1) {
        printf("Current time is %d.%02d.%02d,%02d\n",h,min,sec,ohs);
    }

    else if (n==2) {

        if (stricmp(arg[1],"-s") == 0) {
            printf("Current time is: %d.%02d.%02d,%02d\n",h,min,sec,ohs);
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
    }
    else {
        puts("Invalid number of arguments.");
        return;
    }

}

/**************************************************************************/

BYTE parse_date(BYTE *s)
{
    BYTE i,day,month;
    WORD year;
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

    asm{
        MOV ah,2Bh
        XOR al,al
        MOV CX,year
        MOV DH,month
        MOV DL,day
        INT 21h         /*set date*/
        MOV i,AL        /*00 = sucessfull ;ff = fail*/
    }
    return i;
}

/**************************************************************************/

void do_dt(BYTE n)
{
    WORD yr;
    BYTE mo,d,*p,str[50],i;

    asm {
        MOV ah,2ah
        INT 21h
        MOV yr,CX
        MOV mo,DH
        MOV d,DL
    }
    if(n == 1) {
        printf("Current date is %02d.%02d.%04d\n",d,mo,yr);
    }
    else if (n==2) {

        if (stricmp(arg[1],"-s") == 0) {
            printf("Current date is %02d.%02d.%04d\n",d,mo,yr);
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
    }
    else {
        puts("Invalid number of arguments.");
        return;
    }



}

/**************************************************************************/
