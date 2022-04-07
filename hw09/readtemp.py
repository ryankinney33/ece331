#!/usr/bin/python3
import smbus

# i2c information
channel = 1
i2c_addr = 0x48
bus = smbus.SMBus(channel)

# Read the data from the sensor
TC74_temp = bus.read_byte(i2c_addr)
print(TC74_temp)

# Read the core temperature from sysfs
with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
    core_temp = float(f.read())/1000 # temperature is read in millicelsius

print(core_temp)


# Add the data to the SQL database
