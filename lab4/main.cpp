#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "imageRead.h"
using namespace std;

BYTE **salt_and_pepper(BYTE **src, int width, int height, float depth)
{
    BYTE **result = new BYTE*[height];
    for (int i=0;i<height;i++)
        result[i] = new BYTE[width];

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            if(rand()/float(RAND_MAX)<depth)
            {
                if(rand()/float(RAND_MAX)<0.5)
                    result[i][j] = 0;
                else
                    result[i][j] = 255;
            }
            else
            {
                result[i][j] = src[i][j];
            }
        }
    }
    return result;
}

double gaussrand(float E, float V)
{
   static double V1, V2, S;
   static int phase = 0;
   double X;

   if ( phase == 0 ) {
       do {
           double U1 = (double)rand() / RAND_MAX;
           double U2 = (double)rand() / RAND_MAX;
            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

       X = V1 * sqrt(-2 * log(S) / S);
   } else
       X = V2 * sqrt(-2 * log(S) / S);

   phase = 1 - phase;
   X = X*V+E;
   return X;
}

BYTE **gauss(BYTE **src, int width, int height, float E, float V)
{
    BYTE **result = new BYTE*[height];
    for (int i=0;i<height;i++)
        result[i] = new BYTE[width];

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            float tmp = float(gaussrand(E, V))+src[i][j];
            //cout<<tmp<<endl;
            //system("pause");
            if(tmp>255)
                result[i][j]=255;
            else if(tmp<0)
                result[i][j] =0;
            else result[i][j] = tmp;

        }
    }
    return result;
}

BYTE **template_change(int **tem, BYTE **src, int width, int height)//3*3
{
    BYTE **result = new BYTE *[height];
    for (int i = 0;i<height;i++)
        result[i] = new BYTE[width];
    float temp_result[height][width];
    for (int i = 0;i<height;i++)
        for(int j =0; j<width;j++)
            temp_result[i][j]=0;

    int tem_sum = 0;
//将图像像素点分为9块处理，左上角点、上边缘点、右上角点、左边缘点、中间点、右边缘点、左下角点、下边缘点、右下角点
    for (int i = 1;i<3;i++)
        for (int j =1;j<3;j++)
        {
            tem_sum+=tem[i][j];
            temp_result[0][0]+=tem[i][j]*(float)src[i-1][j-1];
        }
    temp_result[0][0]/=tem_sum;


    for (int c = 1; c<width-1;c++)
    {
        tem_sum = 0;
        for (int i=1;i<3;i++)
            for(int j = 0;j<3;j++)
            {
                tem_sum+=tem[i][j];
                temp_result[0][c]+=tem[i][j]*(float)src[i-1][c+j-1];
            }
        temp_result[0][c]/=tem_sum;
    }
    tem_sum = 0;
    for (int i = 1;i<3;i++)
        for(int j=0;j<2;j++)
        {
            tem_sum+=tem[i][j];
            temp_result[0][width-1]+=tem[i][j]*(float)src[i-1][width+j-2];
        }
    temp_result[0][width-1]/=tem_sum;

    for (int r = 1; r<height-1;r++)
    {
        tem_sum = 0;
        for (int i=0;i<3;i++)
            for(int j = 1;j<3;j++)
            {
                tem_sum+=tem[i][j];
                temp_result[r][0]+=tem[i][j]*(float)src[r+i-1][j-1];
            }
        temp_result[r][0]/=tem_sum;
    }

    for (int r=1; r<height-1; r++)
        for (int c = 1; c<width-1; c++)
        {
            tem_sum=0;
            for (int i = 0;i<3;i++)
                for (int j =0;j<3;j++)
                {
                    tem_sum+=tem[i][j];
                    temp_result[r][c]+=tem[i][j]*(float)src[r+i-1][c+j-1];
                }
            temp_result[r][c]/=tem_sum;
        }

    for (int r = 1;r<height-1;r++)
    {
        tem_sum=0;
        for(int i=0;i<3;i++)
            for(int j=0;j<2;j++)
            {
                tem_sum +=tem[i][j];
                temp_result[r][width-1]+=tem[i][j]*(float)src[r+i-1][width+j-2];
            }
        temp_result[r][width-1]/=tem_sum;
    }

    tem_sum=0;
    for (int i = 0;i<2;i++)
        for (int j =1;j<3;j++)
        {
            tem_sum+=tem[i][j];
            temp_result[height-1][0]+=tem[i][j]*(float)src[height+i-2][j-1];
        }
    temp_result[height-1][0]/=tem_sum;

    for (int c = 1; c<width-1;c++)
    {
        tem_sum=0;
        for (int i=0;i<2;i++)
            for(int j = 0;j<3;j++)
            {
                tem_sum+=tem[i][j];
                temp_result[height-1][c]+=tem[i][j]*(float)src[height+i-2][c+j-1];
            }
        temp_result[height-1][c]/=tem_sum;
    }

    tem_sum=0;
    for (int i = 0;i<2;i++)
        for(int j=0;j<2;j++)
        {
            tem_sum+=tem[i][j];
            temp_result[height-1][width-1]+=tem[i][j]*(float)src[height+i-2][width+j-2];
        }
    temp_result[height-1][width-1]/=tem_sum;

    for (int i = 0;i<height;i++)
        for(int j = 0;j<width;j++)
            result[i][j] = intPRE(temp_result[i][j]);

    return result;
}

BYTE **median_filter(BYTE **src, int width, int height)
{
    BYTE **result = new BYTE *[height];
    for (int i = 0;i<height;i++)
        result[i] = new BYTE[width];

    BYTE tmp_matrix[3*3];

    for (int r=1; r<height-1; r++)
        for (int c = 1; c<width-1; c++)
        {
            for (int i = 0;i<3;i++)
                for (int j =0;j<3;j++)
                {
                    tmp_matrix[i*3+j]=src[r+i-1][c+j-1];
                }
            for (int i = 0;i<9;i++)
            {
                for(int j = 8;j>i;j--)
                {
                    if(tmp_matrix[j]<tmp_matrix[j-1])
                    {
                        BYTE ttmp = tmp_matrix[j-1];
                        tmp_matrix[j-1] = tmp_matrix[j];
                        tmp_matrix[j] = ttmp;
                    }
                }
            }
            result[r][c]=tmp_matrix[4];
        }

    return result;
}

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    srand((unsigned)time(NULL));
    rand();
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

    int **blur;

    int init_blur[9] = {1,1,1,1,1,1,1,1,1};

    blur = new int*[3];
    for (int i =0;i<3;i++)
    {
        blur[i] = new int [3];
        for(int j =0;j<3;j++)
        {
            blur[i][j]=init_blur[i*3+j];
        }
    }

    //BYTE **gray2 = salt_and_pepper(s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight, 0.01);
    BYTE **gray2 = gauss(s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight, 0, 100);
    //BYTE **gray2 = template_change(blur, s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight);
    //BYTE **gray2 = median_filter(s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight);
    for (int i=0;i<binfo.bmiHeader.biHeight;i++)
    {
        for (int j = 0;j<binfo.bmiHeader.biWidth;j++)
        {
            gray_data[i*binfo.bmiHeader.biWidth+j].blue = gray2[i][j];
            gray_data[i*binfo.bmiHeader.biWidth+j].green = gray2[i][j];
            gray_data[i*binfo.bmiHeader.biWidth+j].red = gray2[i][j];
        }
    }

    img_write("gauss.bmp", img_header, binfo, gray_data);


    return 0;
}
