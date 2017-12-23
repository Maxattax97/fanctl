#include "fanControl.h"

const float ABS_ZERO = -273.15;

float upperThreshold = 60.0;
float lowerThreshold = 55.0;
float cpuTemp = -274.0; // Impossible value; below Absolute Zero.
int fanPin = 7;

float getCpuTemp() {
	FILE* file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (file == NULL) {
		printf("Failed to read CPU temperature.\n");
		cpuTemp = ABS_ZERO - 1.0;
	} else {
		int cpuTempInt = (int) ABS_ZERO;
		fscanf(file, "%d", &cpuTempInt);
		fclose(file);
		cpuTemp = ((float) cpuTempInt) / 1000;
	}
	return cpuTemp;
}

// Usage: fanctl [-t] [-l] [enable|disable] [-p <pin #>] [-i <interval>] [-th <upper> <lower>]
int main (int argc, char** argv) {
	char printTemperature = 0;
	char logEnabledDisabled = 0;
	int interval = 1000;
	int enableDisable = 0;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			printTemperature = 1;
		} else if (strcmp(argv[i], "-l") == 0) {
			logEnabledDisabled = 1;
		} else if (strcmp(argv[i], "-p") == 0) {
			if (i + 1 <= argc) {
				fanPin = atoi(argv[i+1]);
				if (fanPin < 0 || fanPin > 40) {
					printf("Invalid pin number. Must be a WiringPi pin.\n");
					return 1;
				}
			} else {
				printf("Must supply pin number.\n");
				return 1;
			}
		} else if (strcmp(argv[i], "enable") == 0) {
			enableDisable = 1;
		} else if (strcmp(argv[i], "disable") == 0) {
			enableDisable = -1;
		} else if (strcmp(argv[i], "-i") == 0) {
			if (i + 1 <= argc) {
				interval = atoi(argv[i+1]);
				printf("Interval set to %d milliseconds.\n", interval);
				if (interval < 10 || interval > 60000) {
					printf("Must supply an interval between 10 and 60000 milliseconds.\n");
					return 2;
				}
			} else {
				printf("Must supply interval in milliseconds.\n");
				return 2;
			}
		} else if (strcmp(argv[i], "-th") == 0) {
			if (i + 2 <= argc) {
				upperThreshold = atof(argv[i+1]);
				lowerThreshold = atof(argv[i+2]);
				printf("Upper threshold set to %f C and lower threshold set to %f C.\n", upperThreshold, lowerThreshold);
				if (upperThreshold < ABS_ZERO || lowerThreshold < ABS_ZERO || upperThreshold < lowerThreshold) {
					printf("Must supply valid temperatures in Celsius in descending order.\n");
					return 3;
				}
			} else {
				printf("Must supply an upper and lower threshold in Celsius.\n");
				return 3;
			}
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("Usage: fanctl [enable|disable] [-t] [-l] [-p <pin #>] [-i <interval] [-th <upper> <lower>]\n\n");
			printf("A lightweight fan controller designed for the Raspberry Pi for a simple circuit using a fan and GPIO, ground, and 5V pins.\n");
			printf("To simply turn the fan on or off, use the enable/disable options.\n\n");
			printf("Options:\n");
			printf("  -t  Prints the temperature in Celsius each interval.\n");
			printf("  -l  Prints when the fan is enabled and disabled.\n");
			printf("  -p  Sets the GPIO pin to be used by fanctl. (Default 7)\n");
			printf("  -i  Checks the CPU temperature at the given interval in milliseconds. (Default 1000)\n");
			printf("  -th Sets the upper and lower bounds in Celsius for enabling and disabling the fan respectively. (Default 60, 55)\n\n");
			printf("Example: fanctl -t -l -i 1000 -p 7 -th 60 55\n");
			return 0;
		}
	}

	if (wiringPiSetup() == -1) {
		printf("Failed to setup GPIO via WiringPi.\n");
		return 4;
	}

	pinMode(fanPin, OUTPUT);
	if (enableDisable == 1) {
		digitalWrite(fanPin, 1);
		printf("Fan enabled.\n");
		return 0;
	} else if (enableDisable == -1) {
		digitalWrite(fanPin, 0);
		printf("Fan disabled.\n");
		return 0;
	}

	digitalWrite(fanPin, 0); // Shut the fan off.

	char printedTemperature = 0;
	while (1) {
		printedTemperature = 0;
		getCpuTemp();

		if (cpuTemp < ABS_ZERO) {
			printf("Invalid CPU temperature: %f C\n", cpuTemp);
			// Maintain previous fan setting. The sensor may be failing.
			printedTemperature = 1;
		} else if (digitalRead(fanPin) == 1
				&& cpuTemp <= lowerThreshold) {
			// Fan is on and below disabling temperature.
			digitalWrite(fanPin, 0);
			if (logEnabledDisabled == 1) {
				printf("%f C, fan disabled.\n", cpuTemp);
				printedTemperature = 1;
			}
		} else if (digitalRead(fanPin) == 0
				&& cpuTemp >= upperThreshold) {
			// Fan is off and is above enabling temperature.
			digitalWrite(fanPin, 1);
			if (logEnabledDisabled == 1) {
				printf("%f C, fan enabled.\n", cpuTemp);
				printedTemperature = 1;
			}
		} else if (printTemperature == 1 && printedTemperature == 0) {
			printf("%f C\n", cpuTemp);
		}

		delay(interval);
	}

	return 0;
}
