@echo off
rem CC.BAT   - DOG - Alternate command processor for (currently) MS-DOS ver 3.30
rem 
rem Copyright (C) 1999,2000 Wolf Bergenheim

rem This program is free software; you can redistribute it and/or
rem modify it under the terms of the GNU General Public License
rem as published by the Free Software Foundation; either version 2
rem of the License, or (at your option) any later version.

rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.

rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

rem Developers:
rem Wolf Bergenheim (WB)
rem echo on
if (%CC%) == () goto define
goto start
:define
set CC=D:\BORLANDC\BIN\BCC
set SET=1
:start
if (%1)==() goto use
%CC% -B -d -mt -O1 -tDc %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end
:use
echo Usage: %0 PROGRAM.C
echo compiles programs to tiny (.COM) mode, and optimizes for smallest size
goto end
:clean
set CC=
set SET=
:end
if (%SET%) == (1) goto clean
