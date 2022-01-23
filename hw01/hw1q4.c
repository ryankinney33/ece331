#include <stdio.h>

/*
 * Prints all passed command line arguments to stdout.
 * Each argument is printed on separate lines.
 */
int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) /* argv[0] contains the program name */
		printf("%s\n",argv[i]);
	return 0;
}

