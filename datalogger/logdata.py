#!/usr/bin/python3
import smbus
import sqlite3
from sqlite3 import Error
import datetime as dt
import sys

if len(sys.argv) < 2:
    print("Error: no sqlite3 database path specified", file = sys.stderr)
    sys.exit(1)

# i2c information
channel = 1
i2c_addr = 0x48
bus = smbus.SMBus(channel)

try:
    # Read the data from the TC74 sensor
    TC74_temp = bus.read_byte(i2c_addr)

    # Read the core temperature from sysfs
    with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
        core_temp = float(f.read()) / 1000 # temperature is read in milliCelsius

    # Get the current timestamp
    now = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M")

except Exception as e:
    print(e, file=sys.stderr)
    sys.exit(1)

try:
    # Open the database connection and set the cursor
    with sqlite3.connect(sys.argv[1]) as db:

        c = db.cursor()

        # Create the temperatures table if it does not yet exist
        c.execute("""CREATE TABLE IF NOT EXISTS temps (
                timestamp text PRIMARY KEY NOT NULL,
                room_temp real NOT NULL,
                core_temp real NOT NULL);""")

        # Insert the data into the database
        c.execute("""INSERT INTO temps(timestamp, room_temp, core_temp)
            VALUES(?,?,?)""", (now, TC74_temp, core_temp))

        # Finally, commit the data
        db.commit()

except Exception as e:
    print(e, file=sys.stderr)
    sys.exit(1)
