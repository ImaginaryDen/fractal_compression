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


#pragma once

#include <math.h>
#include <inttypes.h>


uint8_t  GetYFromRGB     (uint8_t r, uint8_t g, uint8_t b);
uint8_t  GetUFromRGB     (uint8_t r, uint8_t g, uint8_t b);
uint8_t  GetVFromRGB     (uint8_t r, uint8_t g, uint8_t b);

uint8_t  GetRFromYUV     (uint8_t y, uint8_t u, uint8_t v);
uint8_t  GetGFromYUV     (uint8_t y, uint8_t u, uint8_t v);
uint8_t  GetBFromYUV     (uint8_t y, uint8_t u, uint8_t v);