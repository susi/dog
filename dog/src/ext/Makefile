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
DEBUG=-D_DEBUG_ALL_ -N -Od
LINKFLAGS=$(LINKFLAGS) -ll -lm -ls -lv
!endif

PROGS=bp.com br.com cl.com cp.com dt.com ls.com mv.com rm.com sz.com tp.com vf.com vr.com rt.com

.c.com:
	$(CC) $(CFLAGS) $(LINKFLAGS) $(DEBUG) $.

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

rt.com: rm.com cp.com
	$(CP) rm.com rt.com

rt: rt.com

install: $(PROGS)
	$(CP) *.com ..\..\bin\

.ignore
clean: rm
	rm *.OBJ
	rm *.COM
