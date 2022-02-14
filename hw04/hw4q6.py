#!/usr/bin/python3
import sys
import requests

# Extract the requested URL from the command line arguments
try:
    url_string = sys.argv[1]
except IndexError:
    sys.exit('Error: No URL specified.')

# Extract the requested file from the URL
pieces = url_string.split('/')
fname = pieces[-1] # also remove queries from the file name

# Check if no protocol, like http://, was entered and no file name was requested
# Insert http as the protocol and assume the requested file was index.html
if len(pieces) == 1:
    url_string = 'http://' + url_string
    fname = 'index.html'

# Check if there was a file name, but no protocol
elif pieces[1]:
    url_string = 'http://' + url_string

# Check if there was no requested file name
if not fname:
    fname = 'index.html'

print('Fetching:', url_string)
while True:
    # Try to read the URL and write the received content
    try:
        r = requests.get(url_string)
        r.raise_for_status() # Raise an exception for an HTTP Error

        with open(fname, 'wb') as f:
            f.write(r.content)
        break

    # If the requested URL is a directory, add a '/' to the end of the URL
    except IsADirectoryError:
        url_string = url_string + '/'
        fname = 'index.html'
    except Exception as err:
        sys.exit(str(err))
