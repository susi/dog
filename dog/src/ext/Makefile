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

CFLAGS=-B -E$(ASM) -d -mt -O1 -Z -Fs -f- -g15 -tDc -Fc -Tq -Tml -Tmx -Tt -I..
LDFLAGS=-lc -ld -lt -lTdc

# See debug.h for all the possible debugs. ext.h includes it
# _DEBUG_ALL_ turns all all debugging and durns off optimization
!if $(DEBUG)
DEBUG=-D_DEBUG_ALL_ -N -Od
LDFLAGS=$(LDFLAGS) -ll -lm -ls -lv
!endif

PROGS=bp.com br.com cl.com cm.com cp.com ds.com dt.com hd.com hh.com ls.com mv.com rm.com rt.com sz.com tp.com vf.com vr.com wi.com
MANUAL=..\..\doc\manual.txt

.c.com:
	$(CC) @&&!
$(CFLAGS)
$(LDFLAGS)
$(DEBUG)
$.
!

all: $(PROGS)

bp: bp.com

br: br.com

cl: cl.com

cm: cm.com

cp: cp.com

ds: ds.com

dt: dt.com

hd: hd.com

hh: hh.com

ls: ls.com

mv: mv.com

pp: pp.dog cp.com
	$(CP) pp.dog ..\..\bin\

pt: pt.dog cp.com
	$(CP) pt.dog ..\..\bin\

rm: rm.com

sz: sz.com

tp: tp.com

vf: vf.com

vr: vr.com

wi: wi.com

rt.com: rm.com cp.com
	$(CP) rm.com rt.com

rt: rt.com

man: hh
	hh all > $(MANUAL)

install: $(PROGS) man pp pt
	$(CP) *.com ..\..\bin\

.ignore
clean: rm
	rm *.OBJ
	rm *.COM
	rm $(MANUAL)
