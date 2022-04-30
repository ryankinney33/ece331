#ifndef ECE331_DICTIONARY_H
#define ECE331_DICTIONARY_H

struct dictionary {
	char *key; /* The key corresponding to value */
	char *value; /* The value */
	struct dictionary *next; /* Next node in the doubly linked list */
	struct dictionary *prev; /* Previous node in the double linked list */
};


/*
 * Initializes a dictionary object.
 *
 * Allocates the memory for the dictionary on the heap, and
 * returns a pointer to the first (empty) dictionary key-value pair.
 */
struct dictionary *dictionary_init();

/*
 * Adds a key-value pair to the dictionary. If the passed key already
 * exists in the dictionary, then the value associated with it is updated.
 * Returns a pointer to the first entry in the dictionary.
 */
struct dictionary *dictionary_add(struct dictionary *dict, char *key, char *value);

/*
 * Returns the value associated with the passed key.
 * Returns NULL if the key is not in the dictionary.
 */
char *dictionary_value(struct dictionary *dict, char *key);

/*
 * Remove the value and key associated with the passed key.
 * Returns a pointer to the first key-value pair.
 */
struct dictionary *dictionary_remove(struct dictionary *dict, char *key);

/*
 * Deallocates all memory associated with the dictionary object.
 * Returns a NULL pointer to a dictionary object.
 */
struct dictionary *dictionary_destroy(struct dictionary *dict);

/*
 * Print the dictionary pointed to by dict.
 * Prints the key-value pair along with the next and prev
 * pointers.
 *
 * Each dictionary entry is printed on its own line.
 */
void dictionary_print(struct dictionary *dict);

#endif /* ECE331_DICTIONARY_H */
