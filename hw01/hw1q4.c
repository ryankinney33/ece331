#include <stdio.h>

/*
 * Prints all passed command line arguments to stdout,
 * including argv[0], which contains the program name.
 * Each argument is printed on separate lines.
 */
int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; ++i) /* argc contains the number of args */
		printf("Argument %i is %s\n", i, argv[i]);
	return 0;
}

