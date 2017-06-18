#include "queue.h"

#include "types.h"
#include "defs.h"

void fifoInit(struct fifo_buf* fifo, int size, fifo_type* buf);
int fifoPut(struct fifo_buf* fifo, fifo_type data);
fifo_type fifoGet(struct fifo_buf* fifo);

void fifoInit(struct fifo_buf* fifo, int size, fifo_type* buf)
{
  fifo->size = size;
  fifo->buf = buf;
  fifo->unuse_num = size;
  fifo->flags = 0;
  fifo->p = fifo->q = 0;
}

int fifoPut(struct fifo_buf* fifo, fifo_type data)
{
  if (fifo->unuse_num == 0)
  {
    fifo->flags |= 1;
    return -1;
  }

  fifo->buf[fifo->p] = data;
  fifo->p++;
  if (fifo->p == fifo->size)
    fifo->p = 0;

  fifo->unuse_num--;


  return 0;
}

fifo_type fifoGet(struct fifo_buf* fifo)
{
  fifo_type data;
  if (fifo->unuse_num == fifo->size)
    return -1;

  data = fifo->buf[fifo->q];
  fifo->q++;
  if (fifo->q == fifo->size)
    fifo->q = 0;
  fifo->unuse_num++;
  

  return data;
}
