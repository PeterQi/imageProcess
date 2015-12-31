#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <math.h>

using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef struct tagBITMAPFILEHEADER {//位图文件头
    WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
 } BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{//位图信息头
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

typedef struct tagRGBQUAD {//调色板
		BYTE rgbBlue;
		BYTE rgbGreen;
		BYTE rgbRed;
		BYTE rgbReserved;
 } RGBQUAD;

typedef struct tagBITMAPINFO {//位图信息头+调色板
		BITMAPINFOHEADER bmiHeader; 	RGBQUAD bmiColors[];
} BITMAPINFO, *PBITMAPINFO;

typedef struct tagIMAGEDATA{//RGB像素点
    BYTE blue;
    BYTE green;
    BYTE red;
} IMAGEDATA;

typedef struct IMAGEYIQ{//YIQ像素点
    BYTE Y;
    BYTE I;
    BYTE Q;
};

typedef struct IMAGEXYZ{//XYZ像素点
    BYTE X;
    BYTE Y;
    BYTE Z;
};

typedef struct IMAGEHSI{//HSI像素点
    BYTE H;
    BYTE S;
    BYTE I;
};

int img_header_read(FILE *fp, BITMAPFILEHEADER &img_file_header)//读取位图文件头并输出
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

bool img_info_read(FILE *fp, BITMAPINFO &img_info)//读取位图信息头+调色板并输出位图信息头
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

        for(int i = 0; i<(1<<img_info.bmiHeader.biBitCount); i++)//没测试
        {
            fread((char *)&img_info.bmiColors[i].rgbBlue, 1, sizeof(BYTE), fp);
            fread((char *)&img_info.bmiColors[i].rgbGreen, 1, sizeof(BYTE), fp);
            fread((char *)&img_info.bmiColors[i].rgbRed, 1, sizeof(BYTE), fp);
        }
    }

    return ColorFg;
}

float intPRE(float a)//将浮点数加0.5以供转整形时四舍五入
{
    if (a>0)
    {
        a+=0.5;
    }
    else
    {
        a-=0.5;
    }
    return a;
}

IMAGEYIQ *RGBtoYIQ(IMAGEDATA *P, int plength)//RGB像素点转换为YIQ像素点
{
    float changeMatrix[3][3]={{0.299, 0.587, 0.114},
                              {0.596, -0.274, -0.322},
                              {0.211, -0.523, 0.312}};
    IMAGEYIQ *yiq = new IMAGEYIQ[plength];
    float Y, Q, I;
    for (int i = 0; i<plength; i++)
    {
        Y = 0;
        I = 0;
        Q = 0;
        Y += changeMatrix[0][0]*P[i].red;
        Y += changeMatrix[0][1]*P[i].green;
        Y += changeMatrix[0][2]*P[i].blue;
        I += changeMatrix[1][0]*P[i].red;
        I += changeMatrix[1][1]*P[i].green;
        I += changeMatrix[1][2]*P[i].blue;
        Q += changeMatrix[2][0]*P[i].red;
        Q += changeMatrix[2][1]*P[i].green;
        Q += changeMatrix[2][2]*P[i].blue;

        yiq[i].Y = (char) intPRE(Y);
        yiq[i].Q = (char) intPRE(Q);
        yiq[i].I = (char) intPRE(I);
    }

    return yiq;
}

float min_RGB(float R, float G, float B)//求三个浮点数的最小值
{
    float re = R;
    if (G<R)
    {
        re = G;
    }
    if (B<re)
    {
        re = B;
    }
    return re;
}

IMAGEHSI *RGBtoHSI(IMAGEDATA *P, int plength)//RGB像素点转换为HSI像素点
{
    IMAGEHSI *hsi = new IMAGEHSI[plength];
    float H, S, I, R, G, B;
    for (int i = 0; i<plength; i++)
    {
        R = (float)P[i].red;
        G = (float)P[i].green;
        B = (float)P[i].blue;
        I = 1.0/3.0*(R+G+B);
        S = 1.0-3.0/(R+G+B)*min_RGB(R, G, B);
        H = (float)acos(((R-G)+(R-B))/2.0/((R-G)*(R-G)+(R-B)*(float)sqrt(G-B)));

        hsi[i].H = (char) intPRE(H);
        hsi[i].S = (char) intPRE(S);
        hsi[i].I = (char) intPRE(I);
    }

    return hsi;
}

IMAGEXYZ *RGBtoXYZ(IMAGEDATA *P, int plength)//RGB像素点转换为XYZ像素点
{
    float changeMatrix[3][3]={{0.490, 0.310, 0.200},
                              {0.177, 0.813, 0.011},
                              {0.000, 0.010, 0.990}};
    IMAGEXYZ *xyz = new IMAGEXYZ[plength];
    float X, Y, Z;
    for (int i = 0; i<plength; i++)
    {
        X = 0;
        Y = 0;
        Z = 0;
        X += changeMatrix[0][0]*P[i].red;
        X += changeMatrix[0][1]*P[i].green;
        X += changeMatrix[0][2]*P[i].blue;
        Y += changeMatrix[1][0]*P[i].red;
        Y += changeMatrix[1][1]*P[i].green;
        Y += changeMatrix[1][2]*P[i].blue;
        Z += changeMatrix[2][0]*P[i].red;
        Z += changeMatrix[2][1]*P[i].green;
        Z += changeMatrix[2][2]*P[i].blue;

        xyz[i].X = (char) intPRE(X);
        xyz[i].Y = (char) intPRE(Y);
        xyz[i].Z = (char) intPRE(Z);
    }

    return xyz;
}

int img_read(char *file_name, BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEDATA *Pixels)//读位图文件函数，将位图文件头存入img_file_header，位图信息头+调色板存入img_info，像素信息存入Pixels
{
    FILE *fp = fopen(file_name, "rb");
    img_header_read(fp, img_file_header);
    bool Colorfg;
    Colorfg = img_info_read(fp, img_info);
    int length = img_info.bmiHeader.biWidth*img_info.bmiHeader.biHeight;
    if (!Colorfg)
    {
        fread(Pixels, sizeof(IMAGEDATA), length, fp);
        /*for(int i = 0; i<100; i++)
        {
            cout<<hex<<(int)Pixels[i].blue<<' ';
            cout<<hex<<(int)Pixels[i].green<<' ';
            cout<<hex<<(int)Pixels[i].red<<' ';
            cout<<endl;
        }*/
    }


    fclose(fp);
}

int img_write(BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEDATA *Pixels)//写RGB图像，这里使用单红色图像
{
    BITMAPFILEHEADER *img_header=&img_file_header;
    BITMAPINFO *binfo=&img_info;
    ofstream Rimg("Rimg.bmp", ios::binary);//写入文件Rimg.bmp

    Rimg.write((char*)&img_header->bfType, sizeof(WORD));//写文件头信息
    Rimg.write((char*)&img_header->bfSize, sizeof(DWORD));
    Rimg.write((char*)&img_header->bfReserved1, sizeof(WORD));
    Rimg.write((char*)&img_header->bfReserved2, sizeof(WORD));
    Rimg.write((char*)&img_header->bfOffBits, sizeof(DWORD));
    Rimg.write((char*)binfo, sizeof(img_info));//写位图信息头+调色板

    for (int i=0;i<binfo->bmiHeader.biHeight;i++)
    {
        for (int j=0;j<binfo->bmiHeader.biWidth;j++)
        {
            char tmp = 0;

            Rimg.write(&tmp, sizeof(BYTE));//每一个像素点地写，green和blue为0
            Rimg.write(&tmp, sizeof(BYTE));
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].red, sizeof(BYTE));
        }
    }//可以调换位置，将red改为blue或green，输出单蓝色或单绿色图像

    //Rimg.write((char*)P, sizeof(P));
    Rimg.close();
    return 0;
}

int img_write_yiq(BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEYIQ *Pixels)//写YIQ图像
{
    BITMAPFILEHEADER *img_header=&img_file_header;
    BITMAPINFO *binfo=&img_info;
    ofstream Rimg("YIQimg.bmp", ios::binary);
    Rimg.write((char*)&img_header->bfType, sizeof(WORD));//写文件头信息
    Rimg.write((char*)&img_header->bfSize, sizeof(DWORD));
    Rimg.write((char*)&img_header->bfReserved1, sizeof(WORD));
    Rimg.write((char*)&img_header->bfReserved2, sizeof(WORD));
    Rimg.write((char*)&img_header->bfOffBits, sizeof(DWORD));
    Rimg.write((char*)binfo, sizeof(img_info));//写位图信息头+调色板
    for (int i=0;i<binfo->bmiHeader.biHeight;i++)
    {
        for (int j=0;j<binfo->bmiHeader.biWidth;j++)
        {
            char tmp = 0;
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].Y, sizeof(BYTE));//写像素点
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].I, sizeof(BYTE));
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].Q, sizeof(BYTE));
        }
    }

    //Rimg.write((char*)P, sizeof(P));
    Rimg.close();
    return 0;
}

int img_write_hsi(BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEHSI *Pixels)
{
    BITMAPFILEHEADER *img_header=&img_file_header;
    BITMAPINFO *binfo=&img_info;
    ofstream Rimg("HSIimg.bmp", ios::binary);
    Rimg.write((char*)&img_header->bfType, sizeof(WORD));//写文件头信息
    Rimg.write((char*)&img_header->bfSize, sizeof(DWORD));
    Rimg.write((char*)&img_header->bfReserved1, sizeof(WORD));
    Rimg.write((char*)&img_header->bfReserved2, sizeof(WORD));
    Rimg.write((char*)&img_header->bfOffBits, sizeof(DWORD));
    Rimg.write((char*)binfo, sizeof(img_info));//写位图信息头+调色板
    for (int i=0;i<binfo->bmiHeader.biHeight;i++)
    {
        for (int j=0;j<binfo->bmiHeader.biWidth;j++)
        {
            char tmp = 0;
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].H, sizeof(BYTE));//写像素点
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].S, sizeof(BYTE));
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].I, sizeof(BYTE));
        }
    }

    //Rimg.write((char*)P, sizeof(P));
    Rimg.close();
    return 0;
}

int img_write_xyz(BITMAPFILEHEADER &img_file_header, BITMAPINFO &img_info, IMAGEXYZ *Pixels)
{
    BITMAPFILEHEADER *img_header=&img_file_header;
    BITMAPINFO *binfo=&img_info;
    ofstream Rimg("XYZimg.bmp", ios::binary);
    Rimg.write((char*)&img_header->bfType, sizeof(WORD));//写文件头信息
    Rimg.write((char*)&img_header->bfSize, sizeof(DWORD));
    Rimg.write((char*)&img_header->bfReserved1, sizeof(WORD));
    Rimg.write((char*)&img_header->bfReserved2, sizeof(WORD));
    Rimg.write((char*)&img_header->bfOffBits, sizeof(DWORD));
    Rimg.write((char*)binfo, sizeof(img_info));//写位图信息头+调色板
    for (int i=0;i<binfo->bmiHeader.biHeight;i++)
    {
        for (int j=0;j<binfo->bmiHeader.biWidth;j++)
        {
            char tmp = 0;
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].X, sizeof(BYTE));//写像素点
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].Y, sizeof(BYTE));
            Rimg.write((char*)&Pixels[i*binfo->bmiHeader.biWidth+j].Z, sizeof(BYTE));
        }
    }

    //Rimg.write((char*)P, sizeof(P));
    Rimg.close();
    return 0;
}
