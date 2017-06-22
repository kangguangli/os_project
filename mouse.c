#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "x86.h"
#include "traps.h"

#include "mouse.h"
#include "gui.h"
#include "message.h"
#include "queue.h"
#include "windows.h"

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

struct mouse_info_struct mouse_info;

static struct mouse_dec_struct mouse_dec;

#define Buf_Size 128
struct fifo_buf device_buf;
fifo_type _device_buf[Buf_Size];

void mouseWait(char type);
void mouseWrite(unsigned char _cmd);
uchar mouseRead();

void mouseInit();
void mouseHandle();
void mouseIntr();
int mouseInfoDecode(uint code);


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

  fifoInit(&device_buf, Buf_Size, _device_buf);
  mouse_dec.phase = 1;
  mouse_dec.click_flag = 0;

  initlock(&mouse_lock, "mouse");
  mouse_info.x = 16;
  mouse_info.y = 16;


  picenable(IRQ_MOUSE);
  ioapicenable(IRQ_MOUSE, 0);

}

void mouseIntr()
{
  acquire(&mouse_lock);
  uint st = inb(Port_KeyStatus);
  //0x01 means 0x64 first bit, 0 = empty, 1 = full
  if((st & 0x20) == 0)
  {
    //cprintf("\nWrong\n");
    ;
  }
  else
  {
    uint data = (uint)(inb(Port_KeyData) + Mouse_Offset);
    //cprintf("\n mouse data: %x\n", data);
    fifoPut(&device_buf, data);

    //uint data1 = fifoGet(&device_buf);
    deviceMessageProc();
    //mouseHandle(0, data1 - Mouse_Offset);
  }
  release(&mouse_lock);
}

int mouseInfoDecode(uint code)
{
  switch (mouse_dec.phase)
  {
    case 0:
      //cprintf("code: %d", code);
      if (code == 0xFA)
      {
          mouse_dec.phase = 1;
          //cprintf("0XFA received");
      }
      return 0;
    case 1:
      if ((code & 0xc8) == 0x08)
      {
        mouse_dec.phase = 2;
        mouse_dec.buf[0] = code;
        ////cprintf("Phase1 Complete");
      }
      return 0;
    case 2:
      mouse_dec.phase = 3;
      mouse_dec.buf[1] = code;
      return 0;
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

void mouseHandle(uint ticks, uint data)
{
  //  uint i = fifoGet(&mouse_buf);
  if (mouseInfoDecode(data) != 0)
  {
    //cprintf("\nmouse_dec: %d %d %d\n", mouse_dec.buf[0],
    //mouse_dec.buf[1], mouse_dec.buf[2]);
    struct message msg;
    msg.type = WM_MOUSEMOVE;

    if ((mouse_dec.button & 0x01) != 0)
    {
      msg.type = WM_LBTNDOWN;
      if (!mouse_dec.click_flag)
      {
        mouse_dec.click_flag = 1;
        mouse_dec.last_clcik = ticks;
      }
      else
      {

        if (ticks - mouse_dec.last_clcik < Double_Click_Intervals)
        {
          cprintf("\nmouse double: %d", ticks - mouse_dec.last_clcik);
          msg.type = WM_LDOUBLE;
        }
        mouse_dec.click_flag = 0;
      }
    }  //mouse_info.btn = 0;

    //printInfo("MouseLeft");
    if ((mouse_dec.button & 0x02) != 0)
      msg.type = WM_RBTNDOWN;
    //mouse_info.btn = 1;
    //printInfo("MouseRight");
    if ((mouse_dec.button & 0x04) != 0)
      msg.type = WM_MBTNDOWN;
    //mouse_info.btn = 2;
    //printInfo("MouseMiddle");

    //cprintf("\nmouse_pos before: %d %d\n", mouse_info.x, mouse_info.y);
    drawScreenToScreen(video_info.cache2, video_info.video_memory, mouse_info.x,
      mouse_info.y, Mouse_Shape_Width, Mouse_Shape_Height, mouse_info.x,
        mouse_info.y);
    // drawContentToContent(video_info.cache2, video_info.video_memory, 0,
    //   0, video_info.screen_width, video_info.screen_height, 0,
    //     0, video_info.screen_width, video_info.screen_height);
    mouse_info.x += mouse_dec.delta_x;
    mouse_info.y += mouse_dec.delta_y;
    //cprintf("\nmouse_pos after: %d %d\n", mouse_info.x, mouse_info.y);

    if (mouse_info.x < 0)
      mouse_info.x = 0;
    if (mouse_info.x + Mouse_Shape_Width > video_info.screen_width)
      mouse_info.x = video_info.screen_width - Mouse_Shape_Width - 1;
    if (mouse_info.y < 0)
      mouse_info.y = 0;
    if (mouse_info.y + Mouse_Shape_Height > video_info.screen_height)
      mouse_info.y = video_info.screen_height - Mouse_Shape_Height - 1;

    msg.params[0] = mouse_info.x;
    msg.params[1] = mouse_info.y;

    drawCursor(video_info.video_memory, mouse_info.x, mouse_info.y);
    messageHandle(msg);

    //drawContentToContent(video_info.cache2, video_info.memory, )
    //cprintf("\nmouse_pos: %d %d\n", mouse_info.x, mouse_info.y);
  }
}
