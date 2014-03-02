#include <string.h>

#include "perf-studio.h"
#include "ringbuffer.h"

#define MAX_SIZE 262144

struct ring_buffer {
	unsigned char *buffer;
	unsigned int size;
	unsigned int in;
	unsigned int out;
};


struct ring_buffer *ring_buffer_new(unsigned int size)
{
	unsigned int real_size = 1;
	struct ring_buffer *buffer;

	/* Find the next power of two for size */
	while (real_size < size && real_size < MAX_SIZE)
		real_size = real_size << 1;

	if (real_size > MAX_SIZE)
		return NULL;

	buffer = g_try_new(struct ring_buffer, 1);
	if (buffer == NULL)
		return NULL;

	buffer->buffer = g_try_new(unsigned char, real_size);
	if (buffer->buffer == NULL) {
		g_free(buffer);
		return NULL;
	}

	buffer->size = real_size;
	buffer->in = 0;
	buffer->out = 0;

	return buffer;
}


int ring_buffer_write(struct ring_buffer *buf, const void *data,
		unsigned int len)
{
	unsigned int end;
	unsigned int offset;
	const unsigned char *d = data; /* Needed to satisfy non-gcc compilers */

	/* Determine how much we can actually write */
	len = MIN(len, buf->size - buf->in + buf->out);

	/* Determine how much to write before wrapping */
	offset = buf->in % buf->size;
	end = MIN(len, buf->size - offset);
	memcpy(buf->buffer+offset, d, end);

	/* Now put the remainder on the beginning of the buffer */
	memcpy(buf->buffer, d + end, len - end);

	buf->in += len;

	return len;
}


unsigned char *ring_buffer_write_ptr(struct ring_buffer *buf,
		unsigned int offset)
{
	return buf->buffer + (buf->in + offset) % buf->size;
}


int ring_buffer_avail_no_wrap(struct ring_buffer *buf)
{
	unsigned int offset = buf->in % buf->size;
	unsigned int len = buf->size - buf->in + buf->out;

	return MIN(len, buf->size - offset);
}


int ring_buffer_write_advance(struct ring_buffer *buf, unsigned int len)
{
	len = MIN(len, buf->size - buf->in + buf->out);
	buf->in += len;

	return len;
}


int ring_buffer_read(struct ring_buffer *buf, void *data, unsigned int len)
{
	unsigned int end;
	unsigned int offset;
	unsigned char *d = data;

	len = MIN(len, buf->in - buf->out);

	/* Grab data from buffer starting at offset until the end */
	offset = buf->out % buf->size;
	end = MIN(len, buf->size - offset);
	memcpy(d, buf->buffer + offset, end);

	/* Now grab remainder from the beginning */
	memcpy(d + end, buf->buffer, len - end);

	buf->out += len;

	if (buf->out == buf->in)
		buf->out = buf->in = 0;

	return len;
}

int ring_buffer_read_at(struct ring_buffer *buf, void *data, unsigned int len, unsigned int at)
{
	unsigned int end;
	unsigned int offset;
	unsigned char *d = data;

	len = MIN(len, buf->in - (buf->out + at));

	/* Grab data from buffer starting at offset until the end */
	offset = (buf->out + at) % buf->size;
	end = MIN(len, buf->size - offset);
	memcpy(d, buf->buffer + offset, end);

	/* Now grab remainder from the beginning */
	memcpy(d + end, buf->buffer, len - end);

	if ((buf->out + len + at) == buf->in)
		return 0;

	return len;
}


int ring_buffer_drain(struct ring_buffer *buf, unsigned int len)
{
	len = MIN(len, buf->in - buf->out);

	buf->out += len;

	if (buf->out == buf->in)
		buf->out = buf->in = 0;

	return len;
}


int ring_buffer_len_no_wrap(struct ring_buffer *buf)
{
	unsigned int offset = buf->out % buf->size;
	unsigned int len = buf->in - buf->out;

	return MIN(len, buf->size - offset);
}


unsigned char *ring_buffer_read_ptr(struct ring_buffer *buf,
		unsigned int offset)
{
	return buf->buffer + (buf->out + offset) % buf->size;
}


int ring_buffer_len(struct ring_buffer *buf)
{
	if (buf == NULL)
		return -1;

	return buf->in - buf->out;
}


void ring_buffer_reset(struct ring_buffer *buf)
{
	if (buf == NULL)
		return;

	buf->in = 0;
	buf->out = 0;
}


int ring_buffer_avail(struct ring_buffer *buf)
{
	if (buf == NULL)
		return -1;

	return buf->size - buf->in + buf->out;
}


int ring_buffer_capacity(struct ring_buffer *buf)
{
	if (buf == NULL)
		return -1;

	return buf->size;
}


void ring_buffer_free(struct ring_buffer *buf)
{
	if (buf == NULL)
		return;

	g_free(buf->buffer);
	g_free(buf);
}