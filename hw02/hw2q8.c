#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/*
 * Prints the total number of bytes used by a list of files passed on
 * the command line. If stat fails to get information on a file,
 * an error message is printed, and the file is skipped.
 *
 * If no names are passed on the command line, the program prints 0.
 */
int main(int argc, char *argv[])
{
	struct stat file; /* Holds the information from stat() */
	uint64_t size = 0; /* Total size of the files in bytes */
	int i;

	/* argv[0] contains the name of the program; ignore it */
	for (i = 1; i < argc; ++i) {
		if (stat(argv[i], &file) == -1) { /* Attempt to get stats */
			fprintf(stderr, "Error reading '%s': %s\n",
				argv[i], strerror(errno));
		}

		size += (uint64_t) file.st_size;
	}

	/* Print size to stdout */
	printf("%lld\n", size);
	return 0;
}

