#!/usr/bin/python3
import sys


# Dictionary to hold word lengths
word_len = dict()
longest_word = ''
maxlen = 0

# Attempt to open the file and read the word lengths
try:
    with open('/usr/share/dict/words', 'r') as f:
        for word in f.readlines():
            size = len(word)-1 # Adjust for the newline

            if size in word_len:
                word_len[size] += 1
            else:
                word_len[size] = 1

            # Update the maximum word length
            if size > maxlen:
                maxlen = size
                longest_word = word[0:-1]

# An error occurred.
except Exception as err:
    print(error)
    sys.exit(str(err))

print('Word:',longest_word)
print('Length:',maxlen)
