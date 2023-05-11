////////////////////////////////////////////////////////////////////
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
//  was modified for UNIX and prettified for your eyes || XX.11.2018
////////////////////////////////////////////////////////////////////


#pragma once


#include <iostream>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "colorspaces.h"
#include "bmplib.h"


using std::cout;
using std::cin;
using std::endl;


double  ImagePSNR   (Image *first, Image *second);


// ========= utilities =========
static double ChannelMSE    (uint8_t *first_channel, uint8_t *second_channel,
							 uint32_t width, uint32_t height);
inline double CountPSNR     (double MSE);