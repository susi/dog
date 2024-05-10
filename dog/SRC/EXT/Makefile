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
CFLAGS=-B -d -mt -O1 -O- -Z -lt
!if $(DEB)
DEBUG=-Ddebug=1 -Dmv_debug=1
!endif

PROGS=bp.com br.com cl.com cp.com dt.com ls.com mv.com rm.com sz.com tp.com vf.com vr.com

.c.com:
	$(CC) $(CFLAGS) $(DEBUG) $.

all: $(PROGS)

bp: bp.com

br: br.com

cl: cl.com

cp: cp.com

dt: dt.com

ls: ls.com

mv: mv.com

rm: rm.com

sz: sz.com

tp: tp.com

vf: vf.com

vr: vr.com

install: $(PROGS)
	cp -v $** ..\..\bin\

.ignore
clean: rm
	rm *.OBJ
	rm *.COM
