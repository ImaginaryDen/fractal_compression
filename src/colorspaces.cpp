////////////////////////////////////////////////////////////////////////
//  PSNR-Measure -- Graphics & Media Lab PSNR measurement program
//
//  Copyright (C) 2004, Moscow State University
//  Graphics & Media Lab / Video Group:
//  http://www.compression.ru/video/
//
//  Author: Sergey Putilin <sputilin@graphics.cs.msu.su>
//  For support write to: <C-CourseSupport@compression.ru>
//  
//  This file is part of video filters project.
//  For conditions of distribution and use, see the accompanying
//  README file.
//
//  was modified for UNIX and prettified for your eyes || XX.11.2018 [J]
////////////////////////////////////////////////////////////////////////


#include "colorspaces.h"


uint8_t GetYFromRGB (uint8_t r, uint8_t g, uint8_t b)
{
  	return round((double)r * 0.299 + (double)g * 0.587 + (double)b * 0.114);
}


uint8_t GetUFromRGB (uint8_t r, uint8_t g, uint8_t b)
{
    return round((double)r * (-0.14713) + (double)g * (-0.28886) + (double)b * 0.436) + 128;
}


uint8_t GetVFromRGB (uint8_t r, uint8_t g, uint8_t b)
{
    return round((double)r * 0.615 + (double)g * (-0.51499) + (double)b * (-0.10001)) + 128;
}


uint8_t GetRFromYUV (uint8_t y, uint8_t u, uint8_t v)
{
    return round((double)y + 1.13983 * ((double)v - 128.0));
}


uint8_t GetGFromYUV (uint8_t y, uint8_t u, uint8_t v)
{
    return round((double)y - 0.39465 * ((double)u - 128.0) - 0.58060 * ((double)v - 128.0));
}


uint8_t GetBFromYUV (uint8_t y, uint8_t u, uint8_t v)
{
    return round((double)y + 2.03211 * ((double)u - 128.0));
}