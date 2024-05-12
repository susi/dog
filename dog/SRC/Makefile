# Makefile   - DOG - Alternate command processor for freeDosDOS
# Copyright (C) 2024 Wolf Bergenheim

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

CC=C:\BORLANDC\BIN\BCC
#Use -B to use TASM to assemple inline
#-w turn on all warnings
CFLAGS=-B -d -mt -O1 -Z -f- -g15 -tDc -Fc -Tq -Tml -Tmx -Tt
!if $(DEB)
DEBUG=-Dbat_debug=1 -Ddebug=1 -Od
!endif
DOG=dog.com
PROGS=$(DOG)
OBJS=dog.obj bat.obj ints.obj  alse.obj cc.obj cmrd.obj ct.obj eh.obj hh.obj xx.obj

.c.obj:
	$(CC) $(CFLAGS) -c $(DEBUG) $.

dog.com: $(OBJS)
	tlink /c /d /t /Tdc c:\borlandc\lib\c0t.obj $**,dog,,cs

#dog.com: dog.c dog.h bat.c ints.c  alse.c cc.c cmrd.c ct.c eh.c hh.c xx.c
#	$(CC) $(CFLAGS) -lt $(DEBUG) $*.c

all: ext util dog

dog: $(DOG) dog.h

ext:
	cd ext
	make all
	cd ..

util:
	cd util
	make all
	cd ..

cp: cp.com

cp.com:
	cd ext
	make cp
	cp -v cp.com ..
	make clean
	cd ..

rm: rm.com

rm.com: cp
	cd ext
	make cp rm
	cp -v rm.com ..
	make clean
	cd ..

install: dog
	cd ext
	make install
	cd ..
	cd util
	make install
	cd..
	cp -v dog.com ..\bin

.ignore
clean: rm
	cd ext
	make clean
	cd ..
	cd util
	make clean
	cd..
	rm *.obj
	rm *.com
