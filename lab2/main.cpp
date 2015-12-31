#include <iostream>
#include "imageRead.h"
#include <complex.h>
#include "highgui.h"

using namespace std;

complex <float>*DFT_one(BYTE *src, int plength, int N)
{
    int re_len;
    complex <float>xubu(0.0, 1.0);
    bool divisible = false;
    if(plength%N==0)
    {
        re_len = plength;
        divisible = true;
    }
    else
    {
        re_len = (plength/N+1)*N;
    }
    complex <float>*DFT_result = new complex<float>[re_len];
    for (int k=0;k < plength/N;k++)
    {
        for (int u=0;u < N;u++)
        {
            DFT_result[k*N+u] = 0;
            for (int i=0; i < N; i++)
            {
                DFT_result[k*N+u]+=(complex<float>)src[k*N+i]*exp(-xubu*(complex<float>)(2*M_PI*u*i/N));
            }
            DFT_result[k*N+u]/=N;
        }
    }
    if(!divisible)
    {
        BYTE last_f[N];
        int base = plength/N*N;
        for (int u=0;u < N;u++)
        {
            if (u+base>=plength)
            {
                last_f[u] = 0;
            }
            else
            {
                last_f[u] = src[u+base];
            }
        }
        for (int u = 0;u < N;u++)
        {
            DFT_result[u+base] = 0;
            for (int i = 0; i < N; i++)
            {
                DFT_result[u+base]+=(complex<float>)last_f[i]*(complex<float>)exp(-xubu*(complex<float>)(2*M_PI*u*i/N));
            }
            DFT_result[u+base]/=N;
        }
    }
    return DFT_result;
}

complex <float>**DFT_two(BYTE **src, int width, int height, int N)
{
    complex <float>xubu(0.0, 1.0);
    complex <float>**DFT_result = new complex<float>*[height];
    for (int i = 0;i<height;i++)
    {
        DFT_result[i] = new complex<float>[width];
    }
    for (int k=0;k < height/N;k++)
    {
        for (int m = 0;m < width/N;m++)
        {
            for (int u=0;u < N;u++)
            {
                for (int v = 0; v < N;v++)
                {
                    DFT_result[k*N+u][m*N+v] = 0;
                    for (int x = 0; x < N; x++)
                    {
                        for (int y = 0; y < N; y++)
                        {
                            DFT_result[k*N+u][m*N+v]+=(complex<float>)src[k*N+x][m*N+y]*exp(-xubu*(complex<float>)(2*M_PI*(u*x+v*y)/N));
                        }
                    }
                    DFT_result[k*N+u][m*N+v]/=N;
                }
            }
        }
    }
    return DFT_result;
}

BYTE *to_gray_one(complex <float>*src, int plength)
{
    double scope[plength];
    double max_sc = 0;
    for (int i = 0;i < plength; i++)
    {
        scope[i] = abs(src[i]);
        if (scope[i]>max_sc)
        {
            max_sc = scope[i];
        }
    }
    BYTE *gray = new BYTE[plength];
    for (int i = 0;i < plength; i++)
    {
        double tmp = scope[i]/max_sc*255.0;
        gray[i] = (char)intPRE((float)tmp);
    }
    return gray;
}

BYTE *to_gray_two(complex <float>**src, int width, int height)
{
    float scope[height][width];
    float max_sc = 0;
    for (int i = 0;i < height; i++)
    {
        for (int j = 0;j < width;j++)
        {
            scope[i][j] = abs(src[i][j]);
            if (scope[i][j]>max_sc)
            {
                max_sc = scope[i][j];
            }
        }
    }
    int plength = width*height;
    BYTE *gray = new BYTE[plength];
    for (int i = 0;i < height; i++)
    {
        for (int j = 0;j < width; j++)
        {
            double tmp = scope[i][j]/max_sc*255.0;
            gray[i*width+j] = (char)intPRE((float)tmp);
        }
    }
    return gray;
}

void test()
{
    IplImage *img = cvLoadImage("2.png");
    cvNamedWindow("example1", CV_WINDOW_AUTOSIZE);
    cvShowImage("example1", img);
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow("example1");
}

float **DCT(BYTE **src, int width, int height, int N)
{
    float **DCT_result = new float*[height];
    for (int i = 0;i<height;i++)
    {
        DCT_result[i] = new float[width];
    }

    for (int k=0;k < height/N;k++)
    {
        for (int m = 0;m < width/N;m++)
        {
            DCT_result[k*N][m*N] = 0;
            for (int x = 0;x < N; x++)
            {
                for (int y = 0;y < N; y++)
                {
                    DCT_result[k*N][m*N]+=src[k*N+x][m*N+y];
                }
            }
            DCT_result[k*N][m*N]/=N;
            for (int u=0;u < N;u++)
            {
                for (int v = 0; v < N;v++)
                {
                    if (u==0&&v==0)
                        continue;
                    DCT_result[k*N+u][m*N+v] = 0;
                    for (int x = 0; x < N; x++)
                    {
                        for (int y = 0; y < N; y++)
                        {
                            DCT_result[k*N+u][m*N+v]+=(float)src[k*N+x][m*N+y]*cos(2*x+1)*u*M_PI*cos(2*y+1)*v*M_PI;
                        }
                    }
                    DCT_result[k*N+u][m*N+v]=DCT_result[k*N+u][m*N+v]/2/N/N/N;
                }
            }
        }
    }
    return DCT_result;
}

void zigzag()
{
    int N=8;
    int a[N][N];
    int base;
    int aim;
    int b[N*N];
    for (int i = 0; i<N;i++)
    {
        for (int j=0;j<N;j++)
        {
            a[i][j]=i*N+j;
            cout<<a[i][j]<<',';
            if(i+j<N)
            {
                base = (1+i+j)*(i+j)/2;
                aim = (i+j)%2;
                if(aim)
                {
                    b[base+i]=a[i][j];
                }
                else
                {
                    b[base+j]=a[i][j];
                }
            }
            else
            {
                base = (1+N)*N/2;
                base+=(N-1+N-1-(i+j-N-1))*(i+j-N)/2;
                aim = (i+j)%2;
                if(aim)
                {
                    b[base+N-1-j] = a[i][j];
                }
                else
                {
                    b[base+N-1-i] = a[i][j];
                }
            }


        }
        cout<<endl;
    }
    for (int i = 0;i < N*N;i++)
    {
        cout<<b[i]<<' ';
    }



}

BYTE **inverse_DCT(float **src, int width, int height, int N, int level)
{
    float changed_src[height][width];

}

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    img_read("lena.bmp", img_header, binfo, P);//¶ÁÎ»Í¼ÎÄ¼þ
    int imglength = binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight;
    int N = 8;

    IMAGEYCrCb *ycrcb;
    IMAGEDATA gray_data[imglength];
    ycrcb = RGBtoYCrCb(P, imglength);
//    for (int i = 0;i < imglength; i++)
//    {
//        gray_data[i].blue = ycrcb[i].Y;
//        gray_data[i].green = ycrcb[i].Y;
//        gray_data[i].red = ycrcb[i].Y;
//    }
//    img_write("img.bmp", img_header, binfo, gray_data);

    complex <float>**dft_ori;
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

    dft_ori = DFT_two(s_Y, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight, N);

    BYTE *gray = to_gray_two(dft_ori, binfo.bmiHeader.biWidth, binfo.bmiHeader.biHeight);
    for (int i = 0;i < imglength; i++)
    {
        gray_data[i].blue = gray[i];
        gray_data[i].green = gray[i];
        gray_data[i].red = gray[i];
    }
    img_write("img2.bmp", img_header, binfo, gray_data);
    //IMAGEw
    //img_write_ycrcb(img_header, binfo, ycrcb);

    return 0;
}
