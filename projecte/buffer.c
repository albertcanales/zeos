#include <buffer.h>

void INIT_BUFFER(struct circular_buffer *buffer)
{
	buffer->read_index = 0;
	buffer->write_index = 0;
	buffer->size = 0;
}

void buffer_push(struct circular_buffer *buffer, char value) {
	if (buffer->size < BUFFER_SIZE) {
		buffer->vect[buffer->write_index] = value;
		buffer->write_index = (buffer->write_index + 1) % BUFFER_SIZE;
		buffer->size++;
	}
}

char buffer_pop(struct circular_buffer *buffer) {
	if (buffer->size > 0) {
		int value = buffer->vect[buffer->read_index];
		buffer->read_index = (buffer->read_index + 1) % BUFFER_SIZE;
		buffer->size--;
		return value;
	}
	return 0;
}

int buffer_empty(struct circular_buffer *buffer) {
	return buffer->size == 0;
}