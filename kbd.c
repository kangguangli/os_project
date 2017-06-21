#include "types.h"
#include "x86.h"
#include "defs.h"
#include "kbd.h"
#include "traps.h"
#include "spinlock.h"

#include "message.h"
#include "windows.h"

static struct spinlock kbd_lock;

void
kbdintr(void)
{
  //cprintf("\n kbd irq \n");
  acquire(&kbd_lock);

  uint st = inb(KBSTATP);
  //0x01 means 0x64 first bit, 0 = empty, 1 = full
  if((st & KBS_DIB) == 0 || (st & 0x20) != 0)
  {
    ;
    //cprintf("\nWrong kbd\n");
  }
  else
  {
    uint data = inb(KBDATAP);
    //cprintf("\n kbd irq \n");

    fifoPut(&device_buf, data + Keyboard_Offset);
    //consoleintr(kbdgetc);
    deviceMessageProc();
  }
  release(&kbd_lock);
}

void kbdHandle(uint data)
{
  static uint shift;
  static uchar *charcode[4] = {
    normalmap, shiftmap, ctlmap, ctlmap
  };

  struct message msg;


  if(data == 0xE0){
    shift |= E0ESC;
    //return 0;
  } else if(data & 0x80){
    // Key released
    data = (shift & E0ESC ? data : data & 0x7F);
    shift &= ~(shiftcode[data] | E0ESC);
    //return 0;
  } else if(shift & E0ESC){
    // Last character was an E0 escape; or with 0x80
    data |= 0x80;
    shift &= ~E0ESC;
  }
  shift |= shiftcode[data];
  shift ^= togglecode[data];

  msg.type = WM_KEYDOWN;
  msg.params[0] = shift;
  msg.params[1] = normalmap[data];

  uint c = charcode[shift & (CTL | SHIFT)][data];
  if(shift & CAPSLOCK){
    if('a' <= c && c <= 'z')
      c += 'A' - 'a';
    else if('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }

  messageHandle(msg);
}

void kbdInit()
{
  initlock(&kbd_lock, "kbd");

  picenable(IRQ_KBD);
  ioapicenable(IRQ_KBD, 0);
}
