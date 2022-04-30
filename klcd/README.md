
# ECE 331 S22 Kernel Driver Project

Ryan Kinney

*See the project description for details on wiring and GPIO pins used.*

The test programs are: open, clear, write, backwards, and locktest.  
- `open` tests opening the /dev/lcd file in various ways  
- `clear` tests the ioctl system call  
- `write` tests writing the LCD  
- `backwards` tests writing to the LCD with the cursor in reverse mode  
- `locktest` tests the locking capability of the kernel module  

All test programs (and the kernel module) can be compiled with `make`.

