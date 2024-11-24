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

RM=bin\rm -v -f
RT=bin\rt -v
CP=bin\cp -v -f
ZIP=zip -9
VERSION=085b
BIN=DOG$(VERSION)x.zip
SRC=DOG$(VERSION)s.zip

!ifndef BORLAND
BORLAND=C:\BORLANDC
!endif

MAKE=$(MAKE) -DBORLAND=$(BORLAND)

dog: src

all: src

src:
	cd src
	$($(MAKE)) all
	cd ..

install:
	cd src
	$(MAKE) install
	cd ..

bindist: install
	$(ZIP) -@ tmp.zip <<!
bin\*.com
bin\*.dog
bin\*.ico
doc\manual.txt
doc\readme_x.txt
doc\rn*.txt
doc\ggpl.txt
!
	mkdir DOG
	unzip tmp.zip -d DOG
	$(RM) tmp.zip
	$(ZIP) -r -m $(BIN) DOG

srcdist:
	$(ZIP) -@ tmp.zip <<!
src\Makefile
src\*.c
src\*.h
src\dog.dog
src\msdos.dog
src\ext\Makefile
src\ext\*.c
src\ext\*.h
src\ext\*.dog
src\util\Makefile
src\util\*.c
src\util\*.h
Makefile
doc\manual.txt
doc\readme_s.txt
doc\rn*.txt
doc\ggpl.txt
!
	mkdir DOG
	unzip tmp.zip -d DOG
	$(RM) tmp.zip
	$(ZIP) -r -m $(SRC) DOG

dist: install bindist srcdist

.ignore
clean:
	$(RM) $(BIN) $(SRC)
