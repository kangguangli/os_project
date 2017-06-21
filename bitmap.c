#include "types.h"

#include "bitmap.h"
#include "windows.h"
#include "user.h"

void readBitmap24(char* img_name, struct bitmap_file_struct* img)
{
  return;
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
      read(img_file, img->buffer + i * img->bitmap_info_header.width,
        img->bitmap_info_header.width * 3);
  else
  {
    printf(1, "24bit is needed");
  }

  close(img_file);

}
