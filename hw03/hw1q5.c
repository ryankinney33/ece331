#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/*
 * Calculates and prints the sum of an array of 8-bit unsigned integers.
 * The size of the array is passed via command line arguments,
 * and the array is filled with randomly generated values before summing.
 *
 * The maximum size of the array is 16000000 elements.
 */
int main(int argc, char *argv[])
{
	int32_t size;
	time_t t;
	uint8_t *array;
	int i;
	uint32_t sum = 0;

	/* Get the size of the array */
	if (argc > 1) { /* Ensure an argument was passed to the program */
		size = atoi(argv[1]);

		if (size < 0) { /* Negative-sized array? */
			printf("0\n");
			return 0;
		}
		size = (size > 16000000) ? 16000000 : size; /* Size limit */
	} else {
		printf("0\n");
		return 0;
	}

	srand(time(&t));

	/* Create and sum the array */
	array = malloc(size*sizeof(uint8_t));

	for (i = 0; i < size; ++i)
		array[i] = (unsigned)rand();

	for (i = 0; i < size; ++i)
		sum += array[i];

	free(array);


	printf("%u\n",sum);
	return 0;
}

