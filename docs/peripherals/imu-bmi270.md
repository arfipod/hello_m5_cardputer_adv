# IMU: BMI270

The BMI270 is on the shared I2C bus and is expected at address `0x69`.

The current driver detects the device. Full initialization requires verifying
the Bosch init sequence, licensing constraints and any required configuration
blob before adding a production dependency.
