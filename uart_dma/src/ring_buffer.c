#include "ring_buffer.h"

rbuf_t rbuf_init(void * buffer, size_t capacity)
{
	// Size  must be a power of two.
	assert((capacity != 0) && ((capacity & (capacity - 1)) == 0));
	// Ring buffer cannot be null.
	assert(NULL != buffer);

	// Create ringbuffer handle to be returned to user.
	rbuf_t rbuf;

	// Initialize ring buffer.
	rbuf.buffer = buffer;
	rbuf.capacity = capacity;
	rbuf.head = 0;
	rbuf.tail = 0;
	rbuf.full = false;

	return rbuf;
}

void rbuf_reset(rbuf_t * rbuf)
{
	// Ring buffer handle cannot be NULL..
	assert(rbuf);
	rbuf->head = 0;
	rbuf->tail = 0;
	rbuf->full = false;

	return;
}

bool rbuf_full(rbuf_t * rbuf)
{
	// Ring buffer handle cannot be NULL.
	assert(rbuf);

	return rbuf->full;
}

bool rbuf_empty(rbuf_t * rbuf)
{
	// Ring buffer handle cannot be NULL.
	assert(rbuf);

	return (!rbuf->full && (rbuf->head == rbuf->tail));
}

RING_BUFFER_STATUS rbuf_push(rbuf_t * rbuf, uint8_t * data, size_t len)
{
	// Delcare local variable(s).
	size_t count = 0;

	// Ring buffer handle cannot be NULL.
	assert(rbuf);
	// Ring buffer cannot be NULL.
	assert(rbuf->buffer);

	// Make sure the ring buffer is not full.
	if(rbuf_full(rbuf))
	{
		return RING_BUFFER_FULL;
	}
	
	// Make sure there the ring buffer has room.
	else if(rbuf->capacity - rbuf_size(rbuf) < len)
	{
		return RING_BUFFER_INSUFFICIENT_SPACE;
	}
	
	// Add data to ring buffer.
	while(count < len)
	{
		rbuf->buffer[rbuf->head] = data[count++];
		rbuf->head = (rbuf->head + 1) & (rbuf->capacity - 1);
	}

	// Determine if ring buffer is full.
	rbuf->full = (rbuf->head == rbuf->tail);

	return RING_BUFFER_OK;
}

size_t rbuf_pop(rbuf_t * rbuf, uint8_t * data, size_t len)
{
	// Delcare local variable(s).
	size_t count = 0;

	// Ring buffer handle cannot be NULL.
	assert(rbuf);
	// Ring buffer cannot be NULL.
	assert(rbuf->buffer);
	// Item pointer cannot be NULL.
	assert(data);

	// Remove data from buffer.
	while(!rbuf_empty(rbuf) && count < len)
	{
		data[count++] = rbuf->buffer[rbuf->tail];
		rbuf->tail = (rbuf->tail + 1) & (rbuf->capacity - 1);		
	}
	
	// Ring buffer cannot be full if we just removed data.
	rbuf->full = false;
	return count;
}

size_t rbuf_size(rbuf_t * rbuf)
{
	// Ring buffer handle cannot be NULL.
	assert(rbuf);

	// Declare return value.
	size_t retval = rbuf->capacity;

	// Determine ring value size.
	if(!rbuf_full(rbuf))
	{
		if(rbuf->head >= rbuf->tail)
		{
			retval =  (rbuf->head - rbuf->tail);
		}
		else
		{
			retval =  rbuf->capacity - ((rbuf->tail - rbuf->head));
		}
	}

	return retval;
}

size_t rbuf_capacity(rbuf_t * rbuf)
{
	// Ring buffer handle cannot be NULL.
	assert(rbuf);

	return (rbuf->capacity);
}
