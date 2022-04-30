#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initializes a dictionary object.
 *
 * Allocates the memory for the dictionary on the heap, and
 * returns a pointer to the first (empty) dictionary key-value pair.
 */
struct dictionary *dictionary_init()
{
	struct dictionary *dict = malloc(sizeof(struct dictionary));

	if(dict == NULL)
		return NULL;

	/* Set all fields of the dict object to NULL */
	dict->key = NULL;
	dict->value = NULL;
	dict->next = NULL;
	dict->prev = NULL;

	return dict;
}

/* Copies the key and value into the dictionary node. */
int copy_pair(struct dictionary *node, char *key, char *value)
{
	/* Perform some cleanup before copying */
	if (node->key != NULL)
		free(node->key);
	if (node->value != NULL)
		free(node->value);

	/* Allocate memory for the key-value pair */
	node->key = malloc((strlen(key) + 1) * sizeof(char));
	if (node->key == NULL) {
		fprintf(stderr, "Unable to allocate memory for the key.\n");
		return -1;
	}
	node->value = malloc((strlen(value) + 1) * sizeof(char));
	if (node->value == NULL) {
		fprintf(stderr, "Unable to allocate memory for the value.\n");
		return -1;
	}

	/* Copy the key-value pair */
	strcpy(node->key, key);
	strcpy(node->value, value);

	return 0;
}

/*
 * Adds a key-value pair to the dictionary. If the passed key already
 * exists in the dictionary, then the value associated with it is updated.
 * Returns a pointer to the first entry in the dictionary.
 */
struct dictionary *dictionary_add(struct dictionary *dict, char *key, char *value)
{
	struct dictionary *head = dict;

	/* Error checking */
	if (dict == NULL) {
		fprintf(stderr, "Unable to add a key-value pair to an "
				"uninitialized dictionary.\n");
		return NULL; /* Sanity Check */
	}
	if (key == NULL || value == NULL) {
		fprintf(stderr, "Error: key-value pairs must not be NULL.\n");
		return head;
	}

	/* Check if the head node has a key-value pair */
	if (head->key == NULL) {
		copy_pair(head, key, value);
		return head;
	}

	/* Search the dictionary for a matching key */
	while (1) {
		if (strcmp(dict->key, key) == 0) {
			/* Update the value for the matching key */
			copy_pair(dict, key, value);
			return head;
		}

		/* Attempt to move to the next node */
		if (dict->next == NULL)
			break; /* At the end of the dictionary */
		dict = dict->next;
	}

	/* No matching key, add a new node */
	dict->next = dictionary_init();
	if (dict->next == NULL) {
		fprintf(stderr, "Unable to allocate memory for the dictionary.\n");
		return head;
	}
	copy_pair(dict->next, key, value);
	dict->next->prev = dict; /* The next node needs to point to this node */

	return head;
}

/*
 * Returns the value associated with the passed key.
 * Returns NULL if the key is not in the dictionary.
 */
char *dictionary_value(struct dictionary *dict, char *key)
{
	if (key == NULL || dict == NULL)
		return NULL; /* Sanity check */

	if (dict->key == NULL)
		return NULL; /* Another sanity check */

	/* Look for a matching key in the dictionary */
	while (dict != NULL) {
		if (strcmp(dict->key, key) == 0)
			return dict->value; /* A match was found */
		dict = dict->next;
	}

	return NULL; /* No match found */
}

/*
 * Remove the value and key associated with the passed key.
 * Returns a pointer to the first key-value pair.
 */
struct dictionary *dictionary_remove(struct dictionary *dict, char *key)
{
	struct dictionary *head = dict;

	if (key == NULL || dict == NULL)
		return dict; /* Sanity check */

	/* Search for the node with the matching key */
	while (dict != NULL) {
		/* Search for a match */
		if (dict->key != NULL && strcmp(dict->key, key) == 0) {
			/* Update the linked list previous and next */
			if (head->next == NULL) {
				/* Only one node and the keys match */
				free(head->key); /* Reset the node */
				free(head->value);
				head->key = NULL;
				head->value = NULL;
			} else if (dict == head) {
				/* Match in first, but not only node */
				head = head->next;
				head->prev = NULL;

				/* Delete the old node */
				free(dict->key);
				free(dict->value);
				free(dict);
			} else {
				/* Update the linked list pointers */
				dict->prev->next = dict->next;
				if (dict->next != NULL)
					dict->next->prev = dict->prev;

				/* Delete the old node */
				free(dict->key);
				free(dict->value);
				free(dict);
			}
			return head; /* No more work to be done */
		}
		dict = dict->next; /* Move to next node */
	}
	return head; /* No matching key found */
}

/*
 * Deallocates all memory associated with the dictionary object.
 * Returns a NULL pointer to a dictionary object.
 */
struct dictionary *dictionary_destroy(struct dictionary *dict)
{
	if (dict == NULL)
		return NULL; /* No work to be done */

	while (1) {
		/* Attempt to free the key-value pair */
		if (dict->key != NULL)
			free(dict->key);
		if (dict->value != NULL)
			free(dict->value);

		/* Move to the next node if necessary */
		if (dict->next != NULL) {
			dict = dict->next;
			free(dict->prev); /* Free the "current" node too */
		} else {
			/* No more nodes */
			free(dict);
			return NULL;
		}
	}
}

/*
 * Print the dictionary pointed to by dict.
 * Prints the key-value pair along with the next and prev
 * pointers.
 *
 * Each dictionary entry is printed on its own line.
 */
void dictionary_print(struct dictionary *dict)
{
	if (dict == NULL || dict->key == NULL || dict->value == NULL)
		return; /* Sanity check */

	while (dict != NULL) {
		printf("Key: \"%s\"; Value: \"%s\"; Previous: %p; Next: %p\n",
		       dict->key, dict->value, dict->prev, dict->next);
		dict = dict->next;
	}
}

