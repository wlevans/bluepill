#ifndef _RING_BUFFER_BYTE_H_
#define _RING_BUFFER_BYTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
	uint8_t * buffer;	// Memory buffer.
	size_t capacity;	// Number of elements stored in the ring buffer.
	size_t head;		// Holds current head (write) position.
	size_t tail;		// Holds current tail (read) position.
	bool full;			// True if the buffer is full; false otherwise.
} rbuf_t;

typedef enum
{
	RING_BUFFER_OK = 0x00,
	RING_BUFFER_FULL,
	RING_BUFFER_EMPTY,
	RING_BUFFER_INSUFFICIENT_SPACE
} RING_BUFFER_STATUS;

/*!
 * @brief Ring buffer initialization command.
 *
 * @param[in] *buffer Pointer to the buffer (in memory).
 * @param[in] capacity Capacity of the ring buffer (number of elements). Must be
 * a power of 2.
 *
 * @return Returns handle to ring buffer.
 */
rbuf_t rbuf_init(void * buffer, size_t capacity);

/*!
 * @brief Resets ring buffer.
 *
 * @param[in] rbuf Handle to ring buffer.
 *
 * @return void.
 */
void rbuf_reset(rbuf_t * rbuf);

/*!
 * @brief Tests to see if the ring buffer is full.
 *
 * @param[in] rbuf Handle to ring buffer.
 *
 * @return result True if ring buffer is full; false otherwise.
 */
bool rbuf_full(rbuf_t * rbuf);

/*!
 * @brief Tests to see if the ring buffer is empty.
 *
 * @param[in] rbuf Handle to ring buffer.
 *
 * @return result True if ring buffer is full; false otherwise.
 */
bool rbuf_empty(rbuf_t * rbuf);

/*!
 * @brief Adds (pushes) item to ring buffer.
 *
 * @param[in] rbuf Handle to ring buffer.
 * @param[in] *data Pointer to data to add (push) to ring buffer.
 * @param[in] len Lenght (number) of data.
 *
 * @return Ring buffer status.
 */
RING_BUFFER_STATUS rbuf_push(rbuf_t * rbuf, uint8_t * data, size_t len);

/*!
 * @brief Removes (pops) item from ring buffer.
 *
 * @param[in] rbuf Handle to ring buffer.
 * @param[out] *data Pointer to data to remove (pop) from ring buffer.
 * @param[in] len Lenght (number) of data.
 *
 * @return Number of items returned.
 */
size_t rbuf_pop(rbuf_t * rbuf, uint8_t * data, size_t len);

/*!
 * @brief Returns the number of items in the ring buffer.
 *
 * @param[in] rbuf Handle to ring buffer.
 *
 * @return size The current number of items in the ring buffer.
 */
size_t rbuf_size(rbuf_t * rbuf);

/*!
 * @brief Returns the capacity of the ring buffer.
 *
 * @param[in] rbuf Handle to ring buffer.
 *
 * @return capacity The number of elements the ring buffer can hold.
 */
size_t rbuf_capacity(rbuf_t * rbuf);


#ifdef __cplusplus
}
#endif

#endif  // _RING_BUFFER_BYTE_H_
