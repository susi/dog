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
CP=cp -v -f
!ifndef BORLAND
BORLAND=C:\BORLANDC
!endif
BCCDIR=$(BORLAND)\BIN

MAKE=$(MAKE) -DBORLAND=$(BORLAND)
CC=$(BCCDIR)\BCC.EXE
LD=$(BCCDIR)\TLINK.EXE
ASM=$(BCCDIR)\TASM.EXE

CFLAGS=-B -d -f- -mt -O1 -Tml -Tc
LDFLAGS=/c /m /t /Tdc
COMOBJ=$(BORLAND)\lib\c0t.obj

# See debug.h for all the possible debugs.
# _DEBUG_ALL_ turns all all debugging and durns off optimization
!if $(DEBUG)
MAKEDEBUG=-DDEBUG
DEBUG=-D_DEBUG_ALL_ -N -Od
LDFLAGS=$(LDLAGS) /l /m /s /v
COMOBJ=$(BORLAND)\lib\c0s.obj
!endif

!if $(ED)
MAKEDEBUG=-DED
DEBUG=-D_ENV_DEBUG_ -N -Od
LDFLAGS=$(LDFLAGS) /l /m /s /v
COMOBJ=$(BORLAND)\lib\c0s.obj
!endif

!if $(OG)
MAKEDEBUG=-DOG
DEBUG=-D_DOG_DEBUG_ -N -Od
LDFLAGS=$(LDFLAGS) /l /m /s /v
COMOBJ=$(BORLAND)\lib\c0s.obj
!endif

!if $(BAT)
MAKEDEBUG=-DBAT
DEBUG=-D_BAT_DEBUG_ -N -Od
LDFLAGS=$(LDFLAGS) /l /m /s /v
COMOBJ=$(BORLAND)\lib\c0s.obj
!endif

!if $(WARN)
CFLAGS=$(CFLAGS) -w
LDFLAGS=$(LDFLAGS)
COMOBJ=$(BORLAND)\lib\c0s.obj
!endif

DOG=dog.com
PROGS=$(DOG)
OBJS=dog.obj bat.obj ints.obj alse.obj cc.obj cmrd.obj ct.obj eh.obj hh.obj xx.obj

.c.obj:
	$(CC) $(CFLAGS) $(DEBUG) -c $.


dog.com: $(OBJS) dog.h debug.h
	$(LD) $(LDFLAGS) @&&!
$(COMOBJ) $(OBJS),$*,$*,$(BORLAND)\lib\cs.lib
!

tint24.com: tint24.c ints.c
	$(CC) -B -d -mt -f- -tDc -Tt -lt tint24.c ints.c

all: ext util dog tint24

dog: $(DOG) dog.h debug.h

tint24: tint24.com

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
	$(CP) cp.com ..
	$(MAKE) clean
	cd ..

rm: rm.com

rm.com: cp.com
	cd ext
	$(MAKE) cp rm
	$(CP) rm.com ..
	$(MAKE) clean
	cd ..

install: dog
	cd ext
	$(MAKE) install
	cd ..
	cd util
	$(MAKE) install
	cd..
	$(CP) dog.com ..\bin

.ignore
clean: rm.com
	cd ext
	$(MAKE) clean
	cd ..
	cd util
	$(MAKE) clean
	cd..
	rm *.asm
	rm *.obj
	rm *.com
