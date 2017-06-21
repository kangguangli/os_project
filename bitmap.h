#ifndef BITMAP_H
#define BITMAP_H

struct bitmap_header_struct
{
    unsigned short    type;//说明文件的类型，该值必需是0x4D42，也就是字符'BM'，否则表示根本不是BMP
    unsigned int      size;// 	说明该位图文件的大小，用字节为单位
    unsigned short   reserved1;//保留，必须设置为0
    unsigned short   reserved2;//保留，必须设置为0
    unsigned int     offset;//说明从文件头开始到实际的图象数据之间的字节的偏移量。
                            //这个参数是非常有用的，因为位图信息头和调色板的长度会
                            //根据不同情况而变化，所以你可以用这个偏移值迅速的从文件中读取到位数据。
} __attribute__ ((packed));

struct bitmap_info_header_struct{ // bmih
    unsigned int  size;//所需要的字节数
    int   width;//图象的宽度
    int   height;//图象的高度,该值是一个正数，说明图像是倒向的
    unsigned short planes;//bmp图片的平面属，显然显示器只有一个平面，所以恒等于1
    unsigned short bitCount;//比特数/像素，其值为1、4、8、16、24、或32。
    unsigned int compression;//说明图像数据压缩的类型
    unsigned int size_image;//
    int xppm;//水平分辨率
    int yppm;//垂直分辨率
    unsigned int color_used;//位图实际使用的彩色表中的颜色索引数
    unsigned int color_important;
} __attribute__ ((packed));

struct bitmap_file_struct
{
      struct bitmap_header_struct bitmap_header;
      struct bitmap_info_header_struct bitmap_info_header;
      unsigned char* buffer;
};




#endif
