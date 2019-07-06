#makefile

KeybordMouse: C/main.c
		gcc -Wall -O2 -o KM.out C/main.c C/findDevice.c -I H

