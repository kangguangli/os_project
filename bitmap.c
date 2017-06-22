#include "types.h"

#include "bitmap.h"
#include "windows.h"
#include "user.h"

#include "integer.h"
#include "tjpgd.h"

void readBitmap24(char* img_name, struct bitmap_file_struct* img)
{
  int img_file = open(img_name, 0);
  if (img_file < 0)
  {
    printf(1, "\n open failed: %s", img_name);
    return;
  }

  if (img->buffer != 0)
  {
    printf(1, "wrong");
    return;
  }

  read(img_file, &img->bitmap_header, sizeof(struct bitmap_header_struct));
  read(img_file, &img->bitmap_info_header, sizeof(struct bitmap_info_header_struct));

  img->buffer =
    malloc(img->bitmap_info_header.height * img->bitmap_info_header.width * 3);
  if (img->bitmap_info_header.bitCount == 24)
    for (int i = 0; i < img->bitmap_info_header.height; i++)
      read(img_file, img->buffer + i * img->bitmap_info_header.width * 3,
        img->bitmap_info_header.width * 3);
  else
  {
    printf(1, "24bit is needed");
  }

  close(img_file);
}

// typedef struct {
//     int fp;      // 用于输入函数的文件指针
//     BYTE *fbuf;    //用于输出函数的帧缓冲区的指针
//     UINT wfbuf;    //帧缓冲区的图像宽度[像素]
// } IODEV;
//
//
// UINT in_func (JDEC* jd, BYTE* buff, UINT nbyte)
// {
//   IODEV *dev = (IODEV*)jd->device;   // Device identifier for the session (5th argument of jd_prepare function)
//
//   if (buff)
//   {
//     //从输入流读取一字节
//     return (UINT)read(dev->fp, buff, 1);
//   } else {
//     // 从输入流移除一字节
//     return 0;//return seek(dev->fp, nbyte, SEEK_CUR) ? 0 : nbyte;
//   }
// }
//
// UINT out_func (JDEC* jd, void* bitmap, JRECT* rect)
// {
//     IODEV *dev = (IODEV*)jd->device;
//     BYTE *src, *dst;
//     UINT y, bws, bwd;
//     /* 输出进度 */
//     // if (rect->left == 0) {
//     //     printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
//     // }
//
//     /* 拷贝解码的RGB矩形范围到帧缓冲区(假设RGB888配置) */
//     src = (BYTE*)bitmap;
//     dst = dev->fbuf + 3 * (rect->top * dev->wfbuf + rect->left);  /* 目标矩形的左上 */
//     bws = 3 * (rect->right - rect->left + 1);     /* 源矩形的宽度[字节] */
//     bwd = 3 * dev->wfbuf;                         /* 帧缓冲区宽度[字节] */
//     for (y = rect->top; y <= rect->bottom; y++) {
//         memmove(dst, src, bws);   /* 拷贝一行 */
//         src += bws; dst += bwd;  /* 定位下一行 */
//     }
//
//     return 1;    /* 继续解码 */
// }
//
// int decode()
// {
//   void *work;       /* 指向解码工作区域 */
//   JDEC jdec;        /* 解码对象 */
//   JRESULT res;      /* TJpgDec API的返回值 */
//   IODEV devid;      /* 用户定义设备标识 */
//
//   /* 打开一个JPEG文件 */
//   //if (argc < 2) return -1;
//   char* filename = "desktop.jpg";
//   devid.fp = open(filename, 0);
//   if (!devid.fp) return -1;
//
//   /* 分配一个用于TJpgDec的工作区域 */
//   work = malloc(3100);
//
//   /* 准备解码 */
//   res = jd_prepare(&jdec, in_func, work, 3100, &devid);
//   if (res == JDR_OK) {
//     /* 准备好解码。图像信息有效 */
//     //printf("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);
//
//     devid.fbuf = malloc(3 * jdec.width * jdec.height); /* 输出图像的帧缓冲区(假设RGB888配置) */
//     devid.wfbuf = jdec.width;
//
//     res = jd_decomp(&jdec, out_func, 0);   /* 开始1/1缩放解码 */
//     if (res == JDR_OK) {
//       /* 解码成功。你在这里已经解码图像到帧缓冲区 */
//       printf(1, "OK  \n");
//
//     } else {
//       ;//printf("Failed to decompress: rc=%d\n", res);
//     }
//
//     free(devid.fbuf);    /* 释放帧缓冲区 */
//
//   } else {
//     ;//printf("Failed to prepare: rc=%d\n", res);
//   }
//
//   free(work);             /* 释放工作区域 */
//
//   close(devid.fp);       /* 关闭JPEG文件 */
//
//   return res;
// }
