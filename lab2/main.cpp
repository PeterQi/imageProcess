#include <iostream>
#include "imageRead.h"

using namespace std;

int main()
{
    BITMAPFILEHEADER img_header;
    BITMAPINFO binfo;
    IMAGEDATA P[100000];
    img_read("3.bmp", img_header, binfo, P);//¶ÁÎ»Í¼ÎÄ¼þ

    //img_write(img_head, binfo, P);

    //IMAGEYIQ *yiq;
    //yiq = RGBtoYIQ(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    //img_write_yiq(img_header, binfo, yiq);

    IMAGEHSI *hsi;
    hsi = RGBtoHSI(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    img_write_hsi(img_header, binfo, hsi);

    //IMAGEXYZ *xyz;
    //xyz = RGBtoXYZ(P, binfo.bmiHeader.biWidth*binfo.bmiHeader.biHeight);
    //img_write_xyz(img_header, binfo, xyz);

    return 0;
}
