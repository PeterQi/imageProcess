#include <iostream>
#include "imageRead.h"
using namespace std;


BYTE **template_change(int **tem, BYTE **src, int width, int height)//3*3
{
    BYTE **result = new BYTE *[height];
    for (int i = 0;i<height;i++)
        result[i] = new BYTE[width];
    float temp_result[height][width];
    for (int i = 0;i<height;i++)
        for(int j =0; j<width;j++)
            temp_result[i][j]=0;

    int tem00 = 0;
    tem00+=tem[0][0];
    tem00+=tem[0][1];
    tem00+=tem[1][0];
    tem00+=tem[1][1];
    int tem01 = 0;
    tem01+=tem[0][1];
    tem01+=tem[1][1];
    tem01+=tem[0][2];
    tem01+=tem[1][2];
    int tem10 = 0;
    tem10+=tem[1][0];
    tem10+=tem[1][1];
    tem10+=tem[2][0];
    tem10+=tem[2][1];
    int tem11 = 0;
    tem11+=tem[1][1];
    tem11+=tem[1][2];
    tem11+=tem[2][1];
    tem11+=tem[2][2];
    int tem_sum = 0;

    for (int i = 1;i<3;i++)
        for (int j =1;j<3;j++)
            temp_result[0][0]+=tem[i][j]*(float)src[i-1][j-1];

    for (int c = 1; c<width-1;c++)
        for (int i=1;i<3;i++)
            for(int j = 0;j<3;j++)
                temp_result[0][c]+=tem[i][j]*(float)src[i-1][c+j-1];

    for (int i = 1;i<3;i++)
        for(int j=0;j<2;j++)
            temp_result[0][width-1]+=tem[i][j]*(float)src[i-1][width+j-2];

    for (int r = 1; r<height-1;r++)
        for (int i=0;i<3;i++)
            for(int j = 1;j<3;j++)
                temp_result[r][0]+=tem[i][j]*(float)src[r+i-1][j-1];

    for (int r=1; r<height-1; r++)
        for (int c = 1; c<width-1; c++)
            for (int i = 0;i<3;i++)
                for (int j =0;j<3;j++)
                    temp_result[r][c]+=tem[i][j]*(float)src[r+i-1][c+j-1];

    for (int r = 1;r<height-1;r++)
        for(int i=0;i<3;i++)
            for(int j=0;j<2;j++)
                temp_result[r][width-1]+=tem[i][j]*(float)src[r+i-1][width+j-2];

    for (int i = 0;i<2;i++)
        for (int j =1;j<3;j++)
            temp_result[height-1][0]+=tem[i][j]*(float)src[height+i-2][j-1];

    for (int c = 1; c<width-1;c++)
        for (int i=0;i<2;i++)
            for(int j = 0;j<3;j++)
                temp_result[height-1][c]+=tem[i][j]*(float)src[height+i-2][c+j-1];

    for (int i = 0;i<2;i++)
        for(int j=0;j<2;j++)
            temp_result[height-1][width-1]+=tem[i][j]*(float)src[height+i-2][width+j-2];

    for (int i = 0;i<height;i++)
        for(int j = 0;j<width;j++)
            result[i][j] = intPRE(temp_result[i][j]);

    return result;
}

int *histogram(BYTE **src, int start_row, int start_col, int end_row, int end_col)
{
    int *hi = new int [256];
    for (int i =0 ;i < 256;i++)
        hi[i] = 0;
    for(int i = start_row;i < end_row; i++)
    {
        for (int j = start_col;j < end_col;j++)
        {
            hi[(int)src[i][j]]++;
        }
    }
    return hi;
}

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    img_read("lena.bmp", img_header, binfo, P);
    int imglength = binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight;
    int N = 8;

    IMAGEYCrCb *ycrcb;
    IMAGEDATA gray_data[imglength];
    ycrcb = RGBtoYCrCb(P, imglength);

    BYTE **s_Y;

    s_Y = new BYTE*[binfo.bmiHeader.biHeight];
    for (int i = 0;i < binfo.bmiHeader.biHeight; i++)
    {
        s_Y[i] = new BYTE[binfo.bmiHeader.biWidth];
        for (int j = 0; j < binfo.bmiHeader.biWidth; j++)
        {
            s_Y[i][j] = ycrcb[i*binfo.bmiHeader.biWidth+j].Y;
        }
    }

    int **Prewitt_r, **Prewitt_c, **Sobel_r, **Sobel_c, **Laplacian;

    int init_Prewitt_r[9] = {1,1,1,0,0,0,-1,-1,-1};
    int init_Prewitt_c[9] = {1,0,-1,1,0,-1,1,0,-1};
    int init_Sobel_r[9] = {1,2,1,0,0,0,-1,-2,-1};
    int init_Sobel_c[9] = {1,0,-1,2,0,-2,1,0,-1};
    int init_Laplacian[9] = {0,-1,0,-1,4,-1,0,-1,0};

    Prewitt_r = new int*[3];
    Prewitt_c = new int*[3];
    Sobel_r = new int *[3];
    Sobel_c = new int *[3];
    Laplacian = new int *[3];
    for (int i =0;i<3;i++)
    {
        Prewitt_r[i] = new int [3];
        Prewitt_c[i] = new int [3];
        Sobel_r[i] = new int [3];
        Sobel_c[i] = new int [3];
        Laplacian[i] = new int [3];
        for(int j =0;j<3;j++)
        {
            Prewitt_r[i][j]=init_Prewitt_r[i*3+j];
            Prewitt_c[i][j]=init_Prewitt_c[i*3+j];
            Sobel_r[i][j]=init_Sobel_r[i*3+j];
            Sobel_c[i][j]=init_Sobel_c[i*3+j];
            Laplacian[i][j]=init_Laplacian[i*3+j];
        }
    }

    cout<<dec<<"Row:"<<0<<'~'<<binfo.bmiHeader.biWidth-1<<','<<"Column:"<<0<<'~'<<binfo.bmiHeader.biHeight-1<<endl;
    cout<<"Please input start row, start column, end row, end column:"<<endl;
    int start_row, start_col, end_row, end_col;
    cin>>start_row>>start_col>>end_row>>end_col;
    int *histo = histogram(s_Y, start_row, start_col, end_row+1, end_col+1);
    cout<<"Gray count"<<endl;
    for (int i =0;i<256;i++)
    {
        cout<<dec<<i<<' '<<histo[i]<<endl;
    }
    return 0;

    BYTE **gray2 = template_change(Laplacian, s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight);
    for (int i=0;i<binfo.bmiHeader.biHeight;i++)
    {
        for (int j = 0;j<binfo.bmiHeader.biWidth;j++)
        {
            gray_data[i*binfo.bmiHeader.biWidth+j].blue = gray2[i][j];
            gray_data[i*binfo.bmiHeader.biWidth+j].green = gray2[i][j];
            gray_data[i*binfo.bmiHeader.biWidth+j].red = gray2[i][j];
        }
    }

    img_write("Laplacian.bmp", img_header, binfo, gray_data);


    return 0;
}
