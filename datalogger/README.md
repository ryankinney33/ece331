# ECE 331 Datalogger
A simple data logger and web plotter for the Raspberry Pi. Reads the room temperature from the [Microchip TC74](https://www.microchip.com/en-us/product/TC74) and CPU core temperature from the sysfs interface. The data is collected once per minute and logged in an [SQLite](https://www.sqlite.org/) database. Data is plotted on the web using [lighttpd](https://www.lighttpd.net/) and [Chart.js](https://www.chartjs.org/).

## Installation
### *Step 1: Install prerequisite packages*
1. Install Lighttpd
	```
	$ sudo apt install lighttpd
	```
2. Install SQLite3
	```
	$ sudo apt install sqlite3
	```
3. Install PHP
	```
	$ sudo apt install php8.0-fpm php8.0-sqlite3
	```
4. Clone the repository
	```
	$ git clone git@gitlab.eece.maine.edu:ryan.kinney/datalogger.git
	```
5. Move project files to the correct locations

	By default, Lighttpd serves files located in `/var/www/html/`. Either move or create links to the files there. (Note: `/path/to/datalogger` refers to the path where the datalogger files are located).

	***Option 1: Create links***
	```
	$ sudo chown -R pi:www-data /var/www/
	$ ln -s /path/to/datalogger/index.php /var/www/html/index.php
	$ ln -s /path/to/datalogger/fetch.php /var/www/html/fetch.php
	```
	***Option 2: Move files***
	```
	$ sudo chown -R pi:www-data /var/www/
	$ mv /path/to/datalogger/index.php /var/www/html/index.php
	$ mv /path/to/datalogger/fetch.php /var/www/html/fetch.php
	```

### *Step 2: Configuration*
1. Connect the TC74 temperature sensor to the Raspberry Pi's GPIO.
2. Enable i2c on the Raspberry Pi (GUI, raspi-config, etc.)
2. Configure Lighttpd
	```
	$ sudo systemctl enable lighttpd
	$ sudo lighttpd-enable-mod fastcgi fastcgi-php-fpm
	$ sudo service lighttpd force-reload
	```
	Add `".db"` into the `url.access-deny` configuration option in the configuration file: `/etc/lighttpd/lighttpd.conf`.
3. Add execute permission to `logdata.py`
	```
	$ chmod +x /path/to/datalogger/logdata.py
	```
4. Configure the crontab

	Edit the user's crontab with:
	```
	$ crontab -e
	```
	Add the following job to log data once per minute:
	```
	* * * * * /path/to/datalogger/logdata.py /var/www/html/temps.db > /dev/null 2> /dev/null
	```
