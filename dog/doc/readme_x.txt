How to install?

You can use (but it's not manditory) to use DOG as your primary shell. Here
is how:
comment any lines beginning with SHELL from by adding REM and a space in front
of the line. Then add tis line:

SHELL=C:\DOG\BIN\DOG.COM -P -E:1024 -A:1024

In the directory dog\bin there is a file called dog.dog to be used as an example
You will probably want to convert your FDAUTO.BAT file to DOG.DOG.
The example DOG.DOG is loosely based on the FreeDOS 1.3 version of FDAUTO.BAT

Note that the drive and path may varry according to where you put DOG.
This line will start DOG as the primary shell next time when DOS restarts.

For more information please refer to the dog website at:
https://dog.zumppe.net/
