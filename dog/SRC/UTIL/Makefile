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
CFLAGS=-B -d -mt -O1 -Z -f- -g15 -tDc -Fc -Tq -Tml -Tmx -Tt
!if $(DEB)
DEBUG=-Ddebug=1
!endif
PROGS=head.com help.com which.com

.c.com:
	$(CC) $(CFLAGS) $(DEBUG) $.

all: $(PROGS)

hd: head.com

hh: help.com

wh: which.com

cp: cp.com

rm: rm.com

cp.com:
	cd ..\ext
	make cp
	cp -v cp.com ..\util
	make clean
	cd ..\util

rm.com: cp
	cd ..\ext
	make cp rm
	cp -v rm.com ..\util
	make clean
	cd ..\util

install: cp.com $(PROGS)
	cp -v *.com ..\..\bin

.ignore
clean: rm
	rm *.obj
	rm *.com
