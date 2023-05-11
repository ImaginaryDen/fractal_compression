//
// Created by Денис on 08.04.2023.
//

#include "Encoder.h"
#include "Decoder.h"
#include <stdexcept>
#include <iostream>
#include <thread>

void getChannel(Image *source, int channel, uint8_t * buffer)
{
	size_t width = source->info_header.width;
	size_t height = source->info_header.height;

	for (size_t i = 0; i < width; ++i)
		for (size_t j = 0; j < height; ++j)
		{
			uint8_t pixel;
			switch (channel) {
				case 1:
					pixel = source->pixels[i][j].r;
					break;
				case 2:
					pixel = source->pixels[i][j].g;
					break;
				case 3:
					pixel = source->pixels[i][j].b;
					break;
				default:
					throw std::runtime_error("Not real channel");
			}
			buffer[i * width + j] = pixel;
		}
}

void Encoder::fractalCompressionSingleChannel(int channel, Image *origin_img, Transforms &transforms)
{
	ImageMatrix img;

	memset(&img, 0, sizeof(img));

	img.width = origin_img->info_header.width;
	img.height = origin_img->info_header.height;
	img.colorspace = ColorSpace::RGB;

	img.r_channel = new uint8_t[img.width * img.height];
	getChannel(origin_img, channel, img.r_channel);
	SaveImageWithMatrix(&img, "Encode_r_channel.bmp");
	// Make second channel the downsampled version of the image.
	img.b_channel = IFSTransform::DownSample(img.r_channel, img.width, 0, 0, img.width / 2);

	for (int y = 0; y < img.height; y += m_blockSize)
	{
		for (int x = 0; x < img.width; x += m_blockSize)
			findMatchesFor(img, transforms, channel, x, y, m_blockSize);
		std::cout << y << std::endl;
	}

	delete []img.r_channel;
	delete []img.b_channel;
}

Transforms Encoder::Encode(Image *source) {
	Transforms transforms;
	transforms.width = source->info_header.width;

	const int numThreads = 3;
	std::thread threads[numThreads];

	for (int i = 0; i < numThreads; ++i)
		threads[i] = std::thread(&Encoder::fractalCompressionSingleChannel, this, i + 1, source, std::ref(transforms));

	for (int i = 0; i < numThreads; ++i)
		threads[i].join();

	return transforms;
}

void Encoder::findMatchesFor(const ImageMatrix &img, Transforms &transforms, int channel, int toX, int toY, int blockSize)
{
	int bestX = 0;
	int bestY = 0;
	int bestOffset = 0;
	IFSTransform::SYM bestSymmetry = IFSTransform::SYM_NONE;
	double bestScale = 0;
	double bestError = 1e9;

	uint8_t buffer[blockSize * blockSize];

	// Get average pixel for the range block
	int rangeAvg = GetAveragePixel(img.r_channel, img.width, toX, toY, blockSize);

	// Go through all the downsampled domain blocks
	for (int y = 0; y < img.height; y += blockSize * 2)
	{
		for (int x = 0; x < img.width; x += blockSize * 2)
		{
			for (int symmetry = 0; symmetry < IFSTransform::SYM_MAX; symmetry++)
			{
				auto symmetryEnum = (IFSTransform::SYM)symmetry;
				IFSTransform ifs = IFSTransform(0, 0, blockSize/*, symmetryEnum*/, 1.0, 0, blockSize, channel);
				ifs.Execute(img.b_channel, img.width / 2, buffer, blockSize, true, x / 2, y / 2);

				// Get average pixel for the downsampled domain block
				int domainAvg = GetAveragePixel(buffer, blockSize, 0, 0, blockSize);

				// Get scale and offset
				double scale = GetScaleFactor(img.r_channel, img.width, toX, toY, domainAvg,
											  buffer, blockSize, 0, 0, rangeAvg, blockSize);
				int offset = (int)(rangeAvg - scale * (double)domainAvg);

				// Get error and compare to best error so far
				double error = GetError(buffer, blockSize, 0, 0, domainAvg,
										img.r_channel, img.width, toX, toY, rangeAvg, blockSize, scale);

				if (error < bestError)
				{
					bestError = error;
					bestX = x;
					bestY = y;
					bestSymmetry = symmetryEnum;
					bestScale = scale;
					bestOffset = offset;
				}

				if (!m_symmetry)
					break;
			}
		}
	}

	if (blockSize > 2 && bestError >= m_quality)
	{
		// Recurse into the four corners of the current block.
		blockSize /= 2;
		findMatchesFor(img, transforms, channel, toX, toY, blockSize);
		findMatchesFor(img, transforms, channel, toX + blockSize, toY, blockSize);
		findMatchesFor(img, transforms, channel, toX, toY + blockSize, blockSize);
		findMatchesFor(img, transforms, channel, toX + blockSize, toY + blockSize, blockSize);
	}
	else
	{
		std::lock_guard<std::mutex> lockGuard(mtx);
		// Use this transformation
		transforms.transforms[DomainBlock(bestX, bestY, img.width)].push_back(
				IFSTransform(
					toX, toY,
					blockSize,
					/*bestSymmetry,*/
					bestScale,
					bestOffset, transforms.width, channel)
		);
	}
}

double Encoder::GetScaleFactor(const uint8_t *domainData, uint32_t domainWidth, int domainX, int domainY, int domainAvg,
							   const uint8_t *rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg, int size) {
	long top = 0;
	long bottom = 0;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domainData[(domainY + y) * domainWidth + (domainX + x)] - domainAvg);
			int range = (rangeData[(rangeY + y) * rangeWidth + (rangeX + x)] - rangeAvg);

			// According to the formula we want (R*D) / (D*D)
			top += range * domain;
			bottom += domain * domain;

			if (bottom < 0)
			{
				printf("Error: Overflow occured during scaling %d %d %d %d\n",
					   y, domainWidth, bottom, top);
				exit(-1);
			}
		}
	}

	if (bottom == 0)
	{
		top = 0;
		bottom = 1;
	}

	return ((double)top) / ((double)bottom);
}

int Encoder::GetAveragePixel(const uint8_t *domainData, int domainWidth, int domainX, int domainY, int size) {
	long top = 0;
	long bottom = (size * size);

	// Simple average of all pixels.
	for (int y = domainY; y < domainY + size; y++)
	{
		for (int x = domainX; x < domainX + size; x++)
		{
			top += domainData[y * domainWidth + x];

			if (top < 0)
			{
				printf("Error: Accumulator rolled over averaging pixels.\n");
				exit(-1);
			}
		}
	}

	return (top / bottom);
}

double
Encoder::GetError(const uint8_t *domainData, int domainWidth, int domainX, int domainY, int domainAvg,
				  const uint8_t *rangeData, uint32_t rangeWidth, int rangeX, int rangeY, int rangeAvg,
				  int size, double scale) {
	double top = 0;
	double bottom = size * size;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domainData[(domainY + y) * domainWidth + (domainX + x)] - domainAvg);
			int range = (rangeData[(rangeY + y) * rangeWidth + (rangeX + x)] - rangeAvg);
			int diff = (int)(scale * (double)domain) - range;

			// According to the formula we want (DIFF*DIFF)/(SIZE*SIZE)
			top += (diff * diff);

			if (top < 0)
			{
				printf("Error: Overflow occured during error %lf\n", top);
				exit(-1);
			}
		}
	}

	return (top / bottom);
}


