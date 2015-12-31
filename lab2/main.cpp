#include <iostream>
#include "imageRead.h"
#include <complex.h>


using namespace std;

complex *DFT(BYTE *src, int plength, int N)
{
    int re_len;
    complex xubu(0.0, 1.0);
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
    complex *DFT_result = new complex[re_len];
    for (int k=0;k < plength/N;k++)
    {
        for (int u=0;u < N;u++)
        {
            DFT_result[k*N+u] = 0;
            for (int i=0; i < N; i++)
            {
                DFT_result[k*N+u]+=src[k*N+i]*exp(-xubu*2*M_PI*u*i/N);
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
                last_f = 0;
            }
            else
            {
                last_f = src[u+base];
            }
        }
        for (int u = 0;u < N;u++)
        {
            DFT_result[u+base] = 0;
            for (int i = 0; i < N; i++)
            {
                DFT_result[u+base]+=last_f[i]*exp(-xubu*2*M_PI*u*i/N);
            }
            DFT_result[u+base]/=N;
        }
    }
    return DFT_result;
}

BYTE *to_gray(complex *src, int plength)
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

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    img_read("3.bmp", img_header, binfo, P);//¶ÁÎ»Í¼ÎÄ¼þ
    int imglength = binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight;
    int N = 8;
    //img_write(img_head, binfo, P);

    //IMAGEYIQ *yiq;
    //yiq = RGBtoYIQ(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    //img_write_yiq(img_header, binfo, yiq);

    //IMAGEHSI *hsi;
    //hsi = RGBtoHSI(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    //img_write_hsi(img_header, binfo, hsi);

    //IMAGEXYZ *xyz;
    //xyz = RGBtoXYZ(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    //img_write_xyz(img_header, binfo, xyz);

    IMAGEYCrCb *ycrcb;
    ycrcb = RGBtoYCrCb(P, imglength);
    complex *dft_ori;
    BYTE s_Y[imglength];
    for (int i = 0;i < imglength; i++)
    {
        s_Y[i] = ycrcb[i].Y;
    }
    dft_ori = DFT(s_Y, imglength, N);
    int re_len;
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
    BYTE gray = to_gray(dft_ori, re_len);
    IMAGEw
    //img_write_ycrcb(img_header, binfo, ycrcb);

    return 0;
}
