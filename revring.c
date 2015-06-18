#include <stdint.h>
#include <assert.h>

typedef struct revring_s{
	/* Pointer to buffer data */
	uint8_t* buffer;

	/* size of buffer */
	unsigned size;

	/* Indication of where to place incoming data. */
	unsigned head;

	/* Indication of where to read incoming data. */
	unsigned tail;
} revring;

/* Initialize buffer. */
void revring_init(revring* rr, uint8_t* buffer, unsigned size){
	rr->buffer = buffer;
	rr->size = size;
	rr->head = size;
	rr->tail = size;
}

/* Test if buffer is empty. */
unsigned revring_empty(const revring* rr){
	return rr->head == rr->tail;
}

/* Test if buffer is full. */
unsigned revring_full(const revring* rr){
	return rr->head == 0;
}

void revring_add_byte(revring* rr, uint8_t byte){
	assert(!revring_full(rr));

	register unsigned head_tmp = rr->head;

	/* Place byte at position preceding head. */
	--head_tmp;
	rr->buffer[head_tmp] = byte;

	/* If byte just went to position 0, wrap next head index around. */
	if(0 == head_tmp)
		head_tmp = rr->size;

	/* If next head index caught up to tail, then buffer is full. */
	if(rr->tail == head_tmp)
		head_tmp = 0;

	/* Update head index. */
	rr->head = head_tmp;
}

uint8_t revring_remove_byte(revring* rr){
	assert(!revring_empty(rr));

	register unsigned tail_tmp = rr->tail;

	/* Read byte preceding tail. */
	--tail_tmp;
	const uint8_t byte = rr->buffer[tail_tmp];

	/* Wrap next tail index around if byte was at 0. */
	if(0 == tail_tmp)
		tail_tmp = rr->size;

	/* If buffer was full before...it is no longer.
	Point head to where tail is currently. */
	if(revring_full(rr))
		rr->head = rr->tail;

	/* Update tail index. */
	rr->tail = tail_tmp;
	return byte;
}

#define RTEST_SIZE 8

int main(int argc, const char* argv[]){
	char buffer[RTEST_SIZE];
	revring rtest;
	revring_init(&rtest, buffer, RTEST_SIZE);
	unsigned i;

	assert(revring_empty(&rtest));

	static const uint8_t test1[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
	for(i = 0; i < 8; ++i){
		assert(!revring_full(&rtest));
		revring_add_byte(&rtest, test1[i]);
	}

	assert(revring_full(&rtest));

	for(i = 0; i < 8; ++i){
		assert(!revring_empty(&rtest));
		uint8_t x = revring_remove_byte(&rtest);
		assert(x == test1[i]);
		assert(!revring_full(&rtest));
	}

	assert(revring_empty(&rtest));

	return 0;
}
