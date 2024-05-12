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
LINK=C:\BORLANDC\BIN\TLINK

#-w turn on all warnings
CFLAGS=-B -d -mt -O1 -Z -Fs -f- -g15 -tDc -Fc -Tq -Tml -Tmx -Tt
LINKFLAGS=/c /d /t /Tdc

# See debug.h for all the possible debugs.
# _DEBUG_ALL_ turns all all debugging and durns off optimization
!if $(DEBUG)
MAKEDEBUG=-DDEBUG
DEBUG=-D_DEBUG_ALL_ -N -Od
LINKFLAGS=$(LINKFLAGS) /l /m /s /v
!endif

!if $(WARN)
CFLAGS=$(CFLAGS) -w
LINKFLAGS=$(LINKFLAGS)
!endif

DOG=dog.com
PROGS=$(DOG)
COMOBJ=c:\borlandc\lib\c0ft.obj
OBJS=dog.obj bat.obj ints.obj alse.obj cc.obj cmrd.obj ct.obj eh.obj hh.obj xx.obj

.c.obj:
	$(CC) $(CFLAGS) -c $(DEBUG) $.

dog.com: $(OBJS) dog.h debug.h
	$(LINK) $(LINKFLAGS) @&&!
$(COMOBJ) $(OBJS)
$*
$*
cs.lib
!

all: ext util dog

dog: $(DOG) dog.h debug.h

ext:
	cd ext
	$(MAKE) $(MAKEDEBUG) all
	cd ..

util:
	cd util
	$(MAKE) $(MAKEDEBUG) all
	cd ..

cp: cp.com

cp.com:
	cd ext
	$(MAKE) cp
	cp -v cp.com ..
	$(MAKE) clean
	cd ..

rm: rm.com

rm.com: cp.com
	cd ext
	$(MAKE) cp rm
	cp -v rm.com ..
	$(MAKE) clean
	cd ..

install: dog
	cd ext
	$(MAKE) install
	cd ..
	cd util
	$(MAKE) install
	cd..
	cp -v dog.com ..\bin

.ignore
clean: rm.com
	cd ext
	$(MAKE) clean
	cd ..
	cd util
	$(MAKE) clean
	cd..
	rm *.obj
	rm *.com
