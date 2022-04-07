#!/usr/bin/python3
import smbus
import sqlite3
from datetime import datetime

# Get the current date and time
now = datetime.now();

# i2c information
channel = 1
i2c_addr = 0x48
bus = smbus.SMBus(channel)

# Read the data from the TC74 sensor
TC74_temp = bus.read_byte(i2c_addr)

# Read the core temperature from sysfs
with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
    core_temp = float(f.read())/1000 # temperature is read in millicelsius

# Open the database connection and set the cursor
db = sqlite3.connect('temperatures.db')
cursor = db.cursor()

# Format the data into the sqlite command and insert the row
cmd = 'INSERT INTO temps(DATETIME, TC74, CORE) VALUES ("{}", {}, {})'.format(
        now, TC74_temp, core_temp)
cursor.execute(cmd)

# Finally, commit the changes and exit
db.commit()
db.close()
