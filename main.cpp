#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
 } BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
		DWORD biSize;
		LONG biWidth;
		LONG biHeight;
		WORD biPlanes;
		WORD biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG biXPelsPerMeter;
		LONG biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
 } BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
		BYTE rgbBlue;
		BYTE rgbGreen;
		BYTE rgbRed;
		BYTE rgbReserved;
 } RGBQUAD;

typedef struct tagBITMAPINFO {
		BITMAPINFOHEADER bmiHeader; 	RGBQUAD bmiColors[];
} BITMAPINFO, *PBITMAPINFO;

typedef struct tagIMAGEDATA{
    BYTE blue;
    BYTE green;
    BYTE red;
} IMAGEDATA;

int img_header_read(FILE *fp, BITMAPFILEHEADER &img_file_header)
{
    PBITMAPFILEHEADER phead = &img_file_header;

    fread(&phead->bfType, 1, sizeof(WORD), fp);
    fread(&phead->bfSize, 1, sizeof(DWORD), fp);
    fread(&phead->bfReserved1, 1, sizeof(WORD), fp);
    fread(&phead->bfReserved2, 1, sizeof(WORD), fp);
    fread(&phead->bfOffBits, 1, sizeof(DWORD), fp);

    cout <<"bfType:"<<hex<<phead->bfType<<endl;
    cout <<"bfSize:"<<hex<<phead->bfSize<<endl;
    cout <<"bfReserved1:"<<hex<<phead->bfReserved1<<endl;
    cout <<"bfReserved2:"<<hex<<phead->bfReserved2<<endl;
    cout <<"bfOffBits:"<<hex<<phead->bfOffBits<<endl;

    return 1;
}

bool img_info_read(FILE *fp, BITMAPINFO &img_info)
{
    fread(&img_info.bmiHeader, 1, sizeof(img_info.bmiHeader), fp);
    cout<<"biSize:"<<hex<<img_info.bmiHeader.biSize<<endl;
    cout<<"biWidth:"<<dec<<img_info.bmiHeader.biWidth<<endl;
    cout<<"biHeight:"<<dec<<img_info.bmiHeader.biHeight<<endl;
    cout<<"biPlanes:"<<hex<<img_info.bmiHeader.biPlanes<<endl;
    cout<<"bitCount:"<<hex<<img_info.bmiHeader.biBitCount<<endl;
    cout<<"biCompression:"<<hex<<img_info.bmiHeader.biCompression<<endl;
    cout<<"biSizeImage:"<<hex<<img_info.bmiHeader.biSizeImage<<endl;
    cout<<"biXPelsPerMeter:"<<hex<<img_info.bmiHeader.biXPelsPerMeter<<endl;
    cout<<"biYPelsPerMeter:"<<hex<<img_info.bmiHeader.biYPelsPerMeter<<endl;
    cout<<"biClrUserd:"<<hex<<img_info.bmiHeader.biClrUsed<<endl;
    cout<<"biClrImportant:"<<hex<<img_info.bmiHeader.biClrImportant<<endl;

    bool ColorFg = false;
    if (img_info.bmiHeader.biBitCount<=8)
    {
        ColorFg = true;

        for(int i = 0; i<(1<<img_info.bmiHeader.biBitCount); i++)//Ã»²âÊÔ
        {
            fread((char *)&img_info.bmiColors[i].rgbBlue, 1, sizeof(BYTE), fp);
            fread((char *)&img_info.bmiColors[i].rgbGreen, 1, sizeof(BYTE), fp);
            fread((char *)&img_info.bmiColors[i].rgbRed, 1, sizeof(BYTE), fp);
            cout<<hex<<(int)img_info.bmiColors[i].rgbBlue<<endl;
            cout<<hex<<(int)img_info.bmiColors[i].rgbGreen<<endl;
            cout<<hex<<(int)img_info.bmiColors[i].rgbRed<<endl;
        }
    }

    return ColorFg;
}


int img_read(char *file_name, BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEDATA *Pixels)
{
    FILE *fp = fopen(file_name, "rb");
    img_header_read(fp, img_file_header);
    bool Colorfg;
    Colorfg = img_info_read(fp, img_info);
    int length = img_info.bmiHeader.biWidth*img_info.bmiHeader.biHeight;
    if (!Colorfg)
    {
        fread(Pixels, sizeof(IMAGEDATA), length, fp);
        for(int i = 0; i<100; i++)
        {
            cout<<hex<<(int)Pixels[i].blue<<' ';
            cout<<hex<<(int)Pixels[i].green<<' ';
            cout<<hex<<(int)Pixels[i].red<<' ';
            cout<<endl;
        }
    }


    fclose(fp);
}

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    img_read("3.bmp", img_header, binfo, P);
    for (int i=0;i<10;i++)
    {
        cout<<hex<<(int)P[i].blue<<endl;
    }
    return 0;
}
