# fanctl

A simple fan controller that checks for CPU temperature bounds and enables/disables the fan accordingly. Runs nicely in the background. This tool is especially handy for overclocked Raspberry Pi's.

## Build

Requires WiringPi.

```
# Build the executable binary
make

# Install the system service
make install
```

## Usage

```
fanctl --help
```
