#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "x86.h"
#include "traps.h"

#include "mouse.h"
#include "gui_first_try.h"

unsigned char
mouse_shape[Mouse_Shape_Height][Mouse_Shape_Height] =
{
  {2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0},
  {2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0},
  {2, 1, 1, 1, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0},
  {2, 1, 1, 2, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0},
  {2, 1, 2, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0, 0},
  {2, 2, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0}
};

static struct spinlock mouse_lock;
struct mouse_info_struct
{
  int x;
  int y;
};
static struct mouse_info_struct mouse_info;

struct mouse_dec_struct
{
  uchar buf[3];
  int phase;
  int delta_x;
  int delta_y;
  int button;
};
static struct mouse_dec_struct mouse_dec;

#define Mouse_Buf_Size 128
struct fifo8_buf
{
  uchar* buf;
  int p;
  int q;
  int size;
  int unuse_num;
  int flags;
};
static struct fifo8_buf mouse_buf;
uchar _mouse_buf[Mouse_Buf_Size];

void mouseWait(char type);
void mouseWrite(unsigned char _cmd);
uchar mouseRead();
void mouseInit();
void mouseHandle();
void mouseIntr();
int mouseInfoDecode(uchar code);

void fifoInit(struct fifo8_buf* fifo, int size, uchar* buf);
int fifoPut(struct fifo8_buf* fifo, uchar ch);
uchar fifoGet(struct fifo8_buf* fifo);

void fifoInit(struct fifo8_buf* fifo, int size, uchar* buf)
{
  fifo->size = size;
  fifo->buf = buf;
  fifo->unuse_num = size;
  fifo->flags = 0;
  fifo->p = fifo->q = 0;
}

int fifoPut(struct fifo8_buf* fifo, uchar ch)
{
  if (fifo->unuse_num == 0)
  {
    fifo->flags |= 1;
    return -1;
  }

  fifo->buf[fifo->p] = ch;
  fifo->p++;
  if (fifo->p == fifo->size)
    fifo->p = 0;

  fifo->unuse_num--;

  return 0;
}

uchar fifoGet(struct fifo8_buf* fifo)
{
  uchar data;
  if (fifo->unuse_num == fifo->size)
    return -1;

  data = fifo->buf[fifo->q];
  fifo->q++;
  if (fifo->q == fifo->size)
    fifo->q = 0;
  fifo->unuse_num++;

  return data;
}


void mouseWait(char type)
{
  int time_out = 100000;
  if (type == 0)//Data
  {
    for (;time_out > 0; time_out--)
      if ((inb(Port_KeyStatus) & 1) == 1)
        return;
  }
  else//Signal
  {
    for (;time_out > 0; time_out--)
      if ((inb(Port_KeyStatus) & 2) == 0)
        return;
  }
  return;
}

void mouseWrite(unsigned char _cmd)
{
  //Wait to be able to send a command
  mouseWait(1);
  //Tell the mouse we are sending a command
  outb(Port_KeyCmd, 0xD4);
  //Wait for the final part
  mouseWait(1);
  //Finally write
  outb(Port_KeyData, _cmd);
}

uchar mouseRead()
{
  //Get response from mouse
  mouseWait(0);
  return inb(Port_KeyData);
}

void mouseInit()
{
  //enable the auxiliary mouse device
  mouseWait(1);
  outb(Port_KeyCmd, 0xA8);

  //modify the "compaq status byte", to enable the interrupt
  mouseWait(1);
  outb(Port_KeyCmd, 0x20);

  //store status byte
  uchar status_byte;
  mouseWait(0);
  status_byte = (inb(Port_KeyData) | 2);//sets the bit 1 (value = 2), enabling the IRQ12

  // telling the controller that we will be sending a status byte
  mouseWait(1);
  outb(Port_KeyCmd, 0x60);

  //send the modified status byte
  mouseWait(1);
  outb(Port_KeyData, status_byte);

  //default settings,
  //Disables streaming, sets the packet rate to 100 per second,
  //and resolution to 4 pixels per mm.
  mouseWrite(0xF6);
  mouseRead();

  mouseWrite(0xFE);		//set Sample Rate
  mouseRead();
  mouseWrite(200);
  mouseRead();

  mouseWrite(0xE8);		//set Resolution
  mouseRead();
  mouseWrite(0x01);
  mouseRead();

  //Enable Packet Streaming
  mouseWrite(0xF4);
  mouseRead();

  fifoInit(&mouse_buf, Mouse_Buf_Size, _mouse_buf);
  mouse_dec.phase = 1;

  initlock(&mouse_lock, "mouse");
  mouse_info.x = 16;
  mouse_info.y = 16;


  picenable(IRQ_MOUSE);
  ioapicenable(IRQ_MOUSE, 0);

}

void mouseIntr()
{
  acquire(&mouse_lock);
  fifoPut(&mouse_buf, inb(Port_KeyData));
  cprintf("\nMove\n");
  //cprintf("Meaningful");
  release(&mouse_lock);
}

int mouseInfoDecode(uchar code)
{
  switch (mouse_dec.phase)
  {
    case 0:
      cprintf("code: %d", code);
      if ((int)code == 0xFA)
      {
          mouse_dec.phase = 1;
          cprintf("0XFA received");
      }
      return 0;
    case 1:
      if (((int)code & 0xc8) == 0x08)
      {
        mouse_dec.phase = 2;
        mouse_dec.buf[0] = code;
        //cprintf("Phase1 Complete");
      }
      return 0;
    case 2:
      mouse_dec.phase = 3;
      mouse_dec.buf[1] = code;
    case 3:
      mouse_dec.phase = 1;
      mouse_dec.buf[2] = code;
      mouse_dec.button = mouse_dec.buf[0] & 0x07;//0x07 means lowest three digits
      mouse_dec.delta_x = mouse_dec.buf[1];
      mouse_dec.delta_y = mouse_dec.buf[2];

      if ((mouse_dec.buf[0] & 0x10) != 0)
        mouse_dec.delta_x |= 0xFFFFFF00;
      if ((mouse_dec.buf[0] & 0x20) != 0)
        mouse_dec.delta_y |= 0xFFFFFF00;

      mouse_dec.delta_y = -mouse_dec.delta_y;
      return 1;
    default:
      ;//Nothing
  }
  return -1;
}

void mouseHandle()
{
  acquire(&mouse_lock);
  if (mouse_buf.size == mouse_buf.unuse_num)
    ;
  else
  {
    uchar i = fifoGet(&mouse_buf);
    if (mouseInfoDecode(i) != 0)
    {
      cprintf("\nmouse_dec: %d %d %d\n", mouse_dec.buf[0],
        mouse_dec.buf[1], mouse_dec.buf[2]);
      if ((mouse_dec.button & 0x01) != 0)
        printInfo("MouseLeft");
      if ((mouse_dec.button & 0x02) != 0)
        printInfo("MouseRight");
      if ((mouse_dec.button & 0x04) != 0)
        printInfo("MouseMiddle");

      cprintf("\nmouse_pos before: %d %d\n", mouse_info.x, mouse_info.y);
      mouse_info.x += mouse_dec.delta_x;
      mouse_info.y += mouse_dec.delta_y;
      cprintf("\nmouse_pos after: %d %d\n", mouse_info.x, mouse_info.y);

      if (mouse_info.x < 0)
        mouse_info.x = 0;
      if (mouse_info.x + Mouse_Shape_Width > Screen_Width)
        mouse_info.x = Screen_Width - Mouse_Shape_Width - 1;
      if (mouse_info.y < 0)
        mouse_info.y = 0;
      if (mouse_info.y + Mouse_Shape_Height > Screen_Height)
        mouse_info.y = Screen_Height - Mouse_Shape_Height - 1;

      drawCursor(mouse_info.x, mouse_info.y);
      cprintf("\nmouse_pos: %d %d\n", mouse_info.x, mouse_info.y);
    }
  }
  release(&mouse_lock);
}
