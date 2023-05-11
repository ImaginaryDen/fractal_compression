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


#include "extension.h"


static bool non_sensetive_strcmp(const char *str1, const char *str2) 
{	
	/* ONLY NON-NULL VALUES */
	if (str1 == NULL || str2 == NULL) {
		return false;
	}

	while (*str1 && *str2) {
		if (isalpha(*str1) && isalpha(*str2)) {
			if (tolower(*str1) != tolower(*str2)) {
				return false;
			}
		} else {
			if (*str1 != *str2) {
				return false;
			}
		}

		++str1; ++str2;
	}
	
	// returns true if both of them have the same length
	return !(*str1 || *str2);
}


static bool check_extension(const char *extension, const char *str) 
{
	const char *lastpoint = NULL; 
	
	while (*str) {
		if (*str == '.') {
			lastpoint = ++str;
		} else {
			++str;
		}
	}

	return non_sensetive_strcmp(extension, lastpoint);
}


bool FileExtBMP (const char* str)
{
	return check_extension("bmp", str);
}


bool FileExtAVI (const char* str)
{
	return check_extension("avi", str);
}