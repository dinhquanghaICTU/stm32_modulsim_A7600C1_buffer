#include    "ringBuffer.h"
#include    <string.h>

void ringBuff_init(RINGBUFFER_t *ringbuff, void *buf, uint16_t size, uint16_t elem_size){
    ringbuff->buffer= (uint8_t  *) buf;
    ringbuff->size= size;
    ringbuff->elem_size= elem_size;
    ringbuff->head= 0;
    ringbuff->tail= 0;
}

uint32_t ringbuff_write(RINGBUFFER_t *ringbuff, uint32_t len, void* data){
    uint32_t i= 0;
    while (i < len)
    {
        uint32_t nextHead= (ringbuff->head + 1)%ringbuff->size;
        if(nextHead == ringbuff->tail){
            break;
        }
        ((uint8_t*)ringbuff->buffer)[ringbuff->head] = ((uint8_t*)data)[i];

        ringbuff->head =nextHead;
        i++;

    }
    return i;
}

uint8_t ringbuff_pop(RINGBUFFER_t *ringBuff, void *out)
{
    if(ringBuff->head == ringBuff->tail)
        return 0; // empty

    *(uint8_t*)out = ringBuff->buffer[ringBuff->tail];

    ringBuff->tail = (ringBuff->tail + 1) % ringBuff->size;
    return 1;
}
