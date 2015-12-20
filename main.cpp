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

typedef struct IMAGEYIQ{
    float Y;
    float I;
    float Q;
};

typedef struct IMAGEXYZ{
    float X;
    float Y;
    float Z;
};

typedef struct IMAGEHSI{
    float H;
    float S;
    float I;
};

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
        }
    }

    return ColorFg;
}

IMAGEYIQ *RGBtoYIQ(IMAGEDATA *P, int plength)
{
    float changeMatrix[3][3]={{0.299, 0.587, 0.114},
                              {0.596, -0.274, -0.322},
                              {0.211, -0.523, 0.312}};
    IMAGEYIQ *yiq = new IMAGEYIQ[plength];
    for (int i = 0; i<plength; i++)
    {
        yiq[i].Y = 0;
        yiq[i].Q = 0;
        yiq[i].I = 0;
        yiq[i].Y += changeMatrix[0][0]*P[i].red;
        yiq[i].Y += changeMatrix[0][1]*P[i].green;
        yiq[i].Y += changeMatrix[0][2]*P[i].blue;
        yiq[i].Q += changeMatrix[1][0]*P[i].red;
        yiq[i].Q += changeMatrix[1][1]*P[i].green;
        yiq[i].Q += changeMatrix[1][2]*P[i].blue;
        yiq[i].I += changeMatrix[2][0]*P[i].red;
        yiq[i].I += changeMatrix[2][1]*P[i].green;
        yiq[i].I += changeMatrix[2][2]*P[i].blue;
    }

    return yiq;
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
    for (int i=0;i<binfo.bmiHeader.biHeight;i++)
    {
        for (int j=0;j<binfo.bmiHeader.biWidth;j++)
        {
            cout<<hex<<(int)P[i*binfo.bmiHeader.biWidth+j].blue<<' ';
            cout<<hex<<(int)P[i*binfo.bmiHeader.biWidth+j].green<<' ';
            cout<<hex<<(int)P[i*binfo.bmiHeader.biWidth+j].red<<' ';
            cout<<endl;
        }
    }

    return 0;
}
