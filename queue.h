#ifndef QUEUE_H
#define QUEUE_H

typedef unsigned int fifo_type;

struct fifo_buf
{
  fifo_type* buf;
  int p;
  int q;
  int size;
  int unuse_num;
  int flags;
};


#endif
