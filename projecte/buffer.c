#include <buffer.h>

void INIT_BUFFER(struct circular_buffer *buffer)
{
	buffer->index_read = 0;
	buffer->index_write = 0;
	buffer->size = 0;
}

void push(struct circular_buffer *buffer, char value) {
	if (size < BUFFER_SIZE) {
		buffer->vect[buffer->index_write] = value;
		buffer->index_write = (buffer->index_write + 1) % BUFFER_SIZE;
		size++;
	}

}

char pop(struct circular_buffer *buffer) {
	if (size > 0) {
		int value = buffer->vect[buffer->index_read];
		buffer->index_read = (buffer->index_read + 1) % BUFFER_SIZE;
		size--;
		return value
	}
	return 0;
	
}

int empty(struct circular_buffer *buffer) {
	return buffer->size == 0;
}