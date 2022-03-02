#!/usr/bin/python3
import sys
import matplotlib.pyplot as plt

# Dictionary to hold word lengths
word_len = dict()
longest_word = ''


# Attempt to open the file and read the word lengths
try:
    with open('/usr/share/dict/words', 'r') as f:
        words = f.read().split()
        for word in words:
            size = len(word) - 1 # Adjust for the newline

            # Update the word-length count
            if size in word_len:
                word_len[size] += 1
            else:
                word_len[size] = 1
        longest_word = max(words, key=len)

# An error occurred.
except Exception as err:
    sys.exit(str(err))

# Print some info then plot the data
print('Word:',longest_word)
print('Length:',len(longest_word))

plt.bar(list(word_len.keys()), list(word_len.values()), width=1)
plt.grid()
plt.title('Word lengths')
plt.xlabel('Word length, unitless')
plt.ylabel('Number of Words, unitless')
plt.show()

