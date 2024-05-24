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
CP=cp -v -f
CFLAGS=-B -d -mt -O1 -Z -Fs -f- -g15 -tDc -Fc -Tq -Tml -Tmx -Tt -I..
LINKFLAGS=-lc -ld -lt -lTdc

# See debug.h for all the possible debugs. ext.h includes it
# _DEBUG_ALL_ turns all all debugging and durns off optimization
!if $(DEBUG)
DEBUG=-D_DEBUG_ALL_ -N -Od -w
LINKFLAGS=$(LINKFLAGS) -ll -lm -ls -lv
!endif

PROGS=head.com help.com which.com generr.com

.c.com:
	$(CC) $(CFLAGS) $(DEBUG) $.

all: $(PROGS)

hd: head.com

hh: help.com

wh: which.com

ge: generr.com

cp: cp.com

rm: rm.com

cp.com:
	cd ..\ext
	$(MAKE) cp
	$(CP) cp.com ..\util
	$(MAKE) clean
	cd ..\util

rm.com: cp
	cd ..\ext
	$(MAKE) cp rm
	$(CP) rm.com ..\util
	$(MAKE) clean
	cd ..\util

install: cp.com $(PROGS)
	$(CP) *.com ..\..\bin

.ignore
clean: rm
	rm *.obj
	rm *.com
