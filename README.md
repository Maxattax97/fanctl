# fanctl

A simple fan controller that checks for CPU temperature bounds and enables/disables the fan accordingly. Runs nicely in the background. This tool is especially handy for overclocked Raspberry Pi's.

## Build

Requires WiringPi.

```
make
```

## Usage

```
fanctl --help
```

## System Service

Copy `fanctl.sh` to `/etc/init.d`. Minor tweaks may be required (such as the path).
