#ifndef __RINGBUFFER__
#define __RINGBUFFER__
#include <stdint.h>

typedef struct 
{
    uint8_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t size;  
    uint16_t elem_size;

}RINGBUFFER_t;


//ham buffer

void ringBuff_init(RINGBUFFER_t *ringbuff, void *buf, uint16_t size, uint16_t elem_size);
uint32_t ringbuff_write(RINGBUFFER_t *ringbuff, uint32_t len, void* data);
uint8_t ringbuff_pop(RINGBUFFER_t *ringBuff, void *out);

#endif /*__RINGBUFFER__*/
