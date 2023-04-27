#ifndef _LINUX_BUFFER_H
#define _LINUX_BUFFER_H

#define BUFFER_SIZE 100

struct circular_buffer {
	char vect[BUFFER_SIZE];
	int write_index;
	int read_index;
	int size;
};

/**
 * INIT_BUFFER - Initializes a circular buffer.
 * @buffer: the circular buffer to initialize
 */
void INIT_BUFFER(struct circular_buffer *buffer);

/**
 * push - add a new entry to the buffer
 * @buffer: the buffer to write on
 * @value: the value to write on
 *
 * Adds the value to the circular buffer.
 * Unless it is full.
 */
void push(struct circular_buffer *buffer, char value);

/**
 * pop - returns and removes an entry to the buffer
 * @buffer: the buffer to read from
 *
 * Removes and returns the value to the circular buffer.
 * If empty, it returns 0 and does not remove.
 */
char pop(struct circular_buffer *buffer);

/**
 * pop - returns if the buffer is empty
 * @buffer: the buffer to check
 *
 * Returns if the buffer is empty
 */
int empty(struct circular_buffer *buffer);

#endif /* _LINUX_BUFFER_H */
