all: fanctl

fanctl: fanControl.c fanControl.h
	gcc -Werror -Wall -std=gnu99 -g -lwiringPi fanControl.c -o fanctl

install:
	sudo cp fanctl /usr/bin/
	sudo cp fanctl.service /lib/systemd/system/
	sudo systemctl enable fanctl.service
	sudo systemctl start fanctl.service

clean:
	rm *.o fanctl
