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


#include "measure.h" // includes all required headers


double ImagePSNR(Image *first, Image *second)
{
	if (!(first && second)) {
		cout << "Error: One of (Image *) pointers equals NULL" << endl;
		return -1.0;
	}


	if (first->info_header.width  != second->info_header.width ||
		first->info_header.height != second->info_header.height) {
		cout << "Error: Images must have same m_size!" << endl;
	return -1.0;
	}

	uint32_t width  = first->info_header.width;
	uint32_t height = first->info_header.height;

	ImageMatrix *first_matrix = get_image_matrix(first, YUV);
	ImageMatrix *second_matrix = get_image_matrix(second, YUV);

	double YMSE = ChannelMSE(first_matrix->y_channel, second_matrix->y_channel, width, height);
	double UMSE = ChannelMSE(first_matrix->u_channel, second_matrix->u_channel, width, height);
	double VMSE = ChannelMSE(first_matrix->v_channel, second_matrix->v_channel, width, height);

	double YPSNR   = CountPSNR(YMSE);
	double UPSNR   = CountPSNR(UMSE);
	double VPSNR   = CountPSNR(VMSE);
	double YUVPSNR = CountPSNR((YMSE + VMSE + UMSE) / 3.0);

	cout << "Average YUV PSNR : " << YUVPSNR << endl;
	cout << "Average Y   PSNR : " << YPSNR << endl;
	cout << "Average U   PSNR : " << UPSNR << endl;
	cout << "Average V   PSNR : " << VPSNR << endl;

	delete_image_matrix(first_matrix);
	delete_image_matrix(second_matrix);
	return YUVPSNR;
}

	
// ================================= utilities ======================================
// count mean squared error between first_channel and second_channel
static double ChannelMSE (uint8_t *first_channel, uint8_t *second_channel, uint32_t width, uint32_t height)
{
	double MSEstore = 0.0, add = 0.0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			add = abs(first_channel[i * height + j] - second_channel[i * height + j]);
			MSEstore += add * add;
		}
	}

	MSEstore = MSEstore / (width * height);
	return MSEstore;
}

inline double CountPSNR (double MSE)
{
	if (MSE) {
		return 20 * log10(255 * 255) - 10 * log10(MSE);
	} else {
		return 0.0;
	}
}
