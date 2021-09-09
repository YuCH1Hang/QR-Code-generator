#include <stdio.h>
#include "qrcodegen.h"

#define UINT16    unsigned short
#define DWORD    unsigned int
#define WORD    short
#define LONG    int
#pragma pack(push,1)
// Bitmap File Header ( 14 Bytes )
typedef struct tagBITMAPFILEHEADER
{
    UINT16 bfType;        // same as BM in ASCII.
    DWORD bfSize;         // the size of the BMP file in bytes
    UINT16 bfReserved1;
    UINT16 bfReserved2;
    DWORD bfOffBits;    // starting address, of the byte where the bitmap image data (Pixel Array) can be found.
} BITMAPFILEHEADER, * PBITMAPFILEHEADER;

// DIB头描述bitmap的信息，包括大小、压缩类型、颜色格式等，DIB头的大小根据版本不同，大小也不同。
typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;             // 4, the size of this header (40 bytes)
    LONG biWidth;             // 4, the bitmap width in pixels (signed integer).
    LONG biHeight;             // 4, the bitmap height in pixels (signed integer).
    WORD biPlanes;             // 2, the number of color planes being used. Must be set to 1.
    WORD biBitCount;         // 2, the number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
    DWORD biCompression;     // 4, the compression method being used.
    DWORD biSizeImage;         // 4, the image size. This is the size of the raw bitmap data, and should not be confused with the file size.
    LONG biXPelsPerMeter;     // 4, the horizontal resolution of the image.
    LONG biYPelsPerMeter;     // 4, the vertical resolution of the image.
    DWORD biClrUsed;         // 4, the number of colors in the color palette, or 0 to default to 2^n.
    DWORD biClrImportant;    // 4, the number of important colors used, or 0 when every color is important; generally ignored.
} BITMAPINFOHEADER;

#pragma pack(pop)

/*---- Utilities ----*/

// Prints the given QR Code to the console.
 void printQr(const uint8_t qrcode[], const char* fn) {
    BITMAPFILEHEADER bmpHdr;
    BITMAPINFOHEADER bmpInfoHdr;
    bmpHdr.bfType = 'B' + ('M' << 8);
    bmpHdr.bfReserved1 = 
    bmpHdr.bfReserved2 = 0;
    bmpHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    FILE* fp = fopen(fn, "wb");

    int  w=-1;
    int  b = 0;
    if (fp) {

        int size = qrcodegen_getSize(qrcode);
        int border = 4;
        int width, height;
        width=height= size + (border << 1);
        const bitsPerPixel = 24;

        int nLineByteCnt = (((width * bitsPerPixel) + 31) >> 5) << 2;
        int npadding = nLineByteCnt - width * 3;
        bmpHdr.bfSize = nLineByteCnt * height  + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmpInfoHdr.biSize = 40;
        bmpInfoHdr.biWidth = width;
        bmpInfoHdr.biHeight = height;
        bmpInfoHdr.biPlanes = 1;
        bmpInfoHdr.biCompression = 0;
        bmpInfoHdr.biSizeImage = 0;
        bmpInfoHdr.biXPelsPerMeter = 0;
        bmpInfoHdr.biXPelsPerMeter = 0;
        bmpInfoHdr.biBitCount = 24;
        bmpInfoHdr.biClrUsed=
        bmpInfoHdr.biClrImportant = 0;
        fwrite((void*)&bmpHdr, 1, sizeof(bmpHdr), fp);
        fwrite((void*)&bmpInfoHdr, 1, sizeof(bmpInfoHdr), fp);
        for (int y = -border; y < size + border; y++) {
            for (int x = -border; x < size + border; x++) {
                bool c = qrcodegen_getModule(qrcode, x, y);
                if (c) fwrite((void*)&b, 1, 3, fp);
                else fwrite((void*)&w, 1, 3, fp);
            }
            fwrite((void*)&b, 1, npadding, fp);
        }
        fclose(fp);
    }
	
}
