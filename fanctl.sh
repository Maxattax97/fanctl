#!/bin/bash
### BEGIN INIT INFO
#
# Provides: fanctl
# Required-Start: $remote_fs
# Required-Stop: $remote_fs
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Enables CPU fan control.
# Description: Enables the Raspberry Pi, thermal controlled CPU fan.
#
### END INIT INFO

PROG="fanctl"
PROG_PATH="/usr/bin"
PROG_ARGS=""
PID_PATH="/var/run"

start() {
	if [ -e "$PID_PATH/$PROG.pid" ]; then
		# Program is already running.
		echo "Error: $PROG is already running." 1>&2
		exit 1
	else
		$PROG_PATH/$PROG $PROG_ARGS 2>&1 >/var/log/$PROG &
		touch "$PID_PATH/$PROG.pid"
		echo "$PROG started."
	fi
}

stop() {
	if [ -e "$PID_PATH/$PROG.pid" ]; then
		# Program is running, kill it.
		killall $PROG
		rm "$PID_PATH/$PROG.pid"
		echo "$PROG stopped."
	else
		echo "Error: $PROG not running." 1>&2
		exit 1
	fi
}

if [ "$(id -u)" != "0" ]; then
	echo "This script must be run as root." 1>&2
	exit 1
fi

case "$1" in
	start)
		start
		exit 0
	;;
	stop)
		stop
		exit 0
	;;
	reload|restart|force-reload)
		stop
		start
		exit 0
	;;
	**)
		echo "Usage: $0 {start|stop|reload}" 1>&2
	;;
esac
