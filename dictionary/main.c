#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct dictionary *dict = NULL; /* The object used for the tests */
	char *tempval;

	/*
	 * Test 1: trying to use an uninitialized dictionary object
	 * The program should not crash during these tests. In theory,
	 * the dictionary functions should do nothing when passed a NULL
	 * dictionary object.
	 */
	printf("First test: uninitialized dictionary object.\n");

	/* Try to add and print some values */
	printf("Attempting to add the following key-value pairs:\n"
	       "Key: \"Grapes\"; Value: \"Wine\"\n"
	       "Key: \"/\"; Value: \"Not a forward slash\"\n");
	dict = dictionary_add(dict, "Grapes", "Wine");
	dict = dictionary_add(dict, "/", "Not a forward slash");
	printf("Printing the contents of the dictionary (should be empty):\n");
	dictionary_print(dict);

	/* Test the other functions too */
	tempval = dictionary_value(dict, "Grapes");
	printf("What value is associated with key \"Grapes\"?\n");
	if (tempval == NULL) {
		printf("There was no value associated with key \"Grapes\".\n");
	} else {
		printf("The value corresponding to \"Grapes\" is \"%s\"\n",
			tempval);
	}
	printf("Attempting to remove the value associated with \"/\"\n");
	dict = dictionary_remove(dict, "/");

	printf("Finally, destroying the dictionary object.\n");
	dict = dictionary_destroy(dict);
	printf("Test 1 complete.\n\n");
	/* Finish test 1 */

	/*
	 * Test 2: Empty dictionary operation
	 * The dictionary functions should have correct functionality
	 * and the program should not crash when the dictionary being
	 * used is initialized, but has no key-value pairs
	 */
	printf("Second test: empty dictionary object.\n");

	dict = dictionary_init(); /* Actually initialize the object */
	if (dict == NULL) {
		fprintf(stderr, "Unable to allocate memory for "
				"the dictionary.\n");
		return 1;
	}

	/* Try the print function */
	printf("Attempting to print an empty dictionary:\n");
	dictionary_print(dict);

	/* Try to retrieve a value from the dictionary */
	tempval = dictionary_value(dict, "Rock");
	printf("What value is associated with key \"Rock\"?\n");
	if (tempval == NULL) {
		printf("There was no value associated with key \"Rock\".\n");
	} else {
		printf("The value corresponding to \"Rock\" is \"%s\"\n",
			tempval);
	}

	/* Remove a value from the empty dictionary? */
	printf("Removing the value associated with \"Rock\":\n");
	dict = dictionary_remove(dict, "Rock");

	/* Print the dictionary after removing, then destroy it */
	dictionary_print(dict);
	dict = dictionary_destroy(dict);
	printf("Test 2 complete.\n\n");
	/* End Test 2 */

	/*
	 * Test 3: Normal dictionary operation
	 * The dictionary functions should still have correct functionality
	 * and the program should not crash during operation.
	 */
	printf("Test 3: Normal dictionary operation\n");

	dict = dictionary_init();
	if (dict == NULL) {
		fprintf(stderr, "Unable to allocate memory for "
				"the dictionary.\n");
		return 1;
	}

	/* Add some key-value pairs */
	printf("Attempting to add the following key-value pairs:\n"
      	       "Key: \"Bananas\"; Value: \"Monkeys\"\n"
	       "Key: \"Salads\"; Value: \"Potato\"\n"
	       "Key: \"Football\"; Value: \"Sport\"\n"
	       "Key: \"Water\"; Value: \"Ice\"\n");
	dict = dictionary_add(dict, "Bananas", "Monkeys");
	dict = dictionary_add(dict, "Salads", "Potato");
	dict = dictionary_add(dict, "Football", "Sport");
	dict = dictionary_add(dict, "Water", "Ice");

	printf("Printing the contents of the dictionary:\n");
	dictionary_print(dict);

	printf("Attemping to add a NULL key-value pair to the dictionary:\n");
	dict = dictionary_add(dict, NULL, NULL);
	dictionary_print(dict);

	/* Remove some key-value pairs from the center, head, and tail */
	printf("\nAfter removing \"Salads\":\n");
	dict = dictionary_remove(dict, "Salads");
	dictionary_print(dict);

	printf("\nAfter removing \"Bananas\":\n");
	dict = dictionary_remove(dict, "Bananas");
	dictionary_print(dict);

	printf("\nAfter removing \"Water\":\n");
	dict = dictionary_remove(dict, "Water");
	dictionary_print(dict);

	/* Finally, add a key-value pair for an existing key */
	printf("\nUpdating the value for key \"Football\":\n");
	dict = dictionary_add(dict, "Football", "Tailgating");
	dictionary_print(dict);

	dict = dictionary_destroy(dict);
	printf("Test 3 complete.\n\n");
	/* Test 3 complete */

	return 0;
}
