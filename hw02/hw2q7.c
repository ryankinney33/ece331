#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/utsname.h>

/*
 * Uses uname() to get the operating system release.
 * Results are printed to stdout, followed by a newline.
 */
int main(int argc, char *argv[])
{
	struct utsname buf; /* Operating system release is buf.release */
	if (uname(&buf) == -1) { /* Get system info with error checking */
		fprintf(stderr, "Error from uname: %s\n", strerror(errno));
		return 1;
	}

	/* Print the operating system release to stdout */
	puts(buf.release);
	return 0;
}

