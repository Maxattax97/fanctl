all: fanctl

fanctl: fanControl.c fanControl.h
	gcc -Werror -Wall -std=gnu99 -g -lwiringPi fanControl.c -o fanctl

clean:
	rm *.o fanctl
