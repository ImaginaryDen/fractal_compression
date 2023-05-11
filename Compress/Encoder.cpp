//
// Created by Денис on 08.04.2023.
//

#include "Encoder.h"
#include "Decoder.h"
#include <stdexcept>
#include <iostream>

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

Transforms Encoder::Encode(Image *source) {
	m_img.width = source->info_header.width;
	m_img.height = source->info_header.height;
	m_img.colorspace = ColorSpace::RGB;

	Transforms transforms;
	transforms.width = m_img.width;

	for (int channel = 1; channel <= 3; channel++)
	{
		// Load image into a local copy
		m_img.r_channel = new uint8_t[m_img.width * m_img.height];
		getChannel(source, channel, m_img.r_channel);
		SaveImageWithMatrix(&m_img, "Encode_r_channel.bmp");
		// Make second channel the downsampled version of the image.
		m_img.b_channel = IFSTransform::DownSample(m_img.r_channel, m_img.width, 0, 0, m_img.width / 2);
		ImageMatrix copy;

		memset(&copy, 0, sizeof(copy));
		copy.width = m_img.width / 2;
		copy.height = m_img.width / 2;

		// Go through all the range blocks

		for (int y = 0; y < m_img.height; y += m_blockSize)
		{
			for (int x = 0; x < m_img.width; x += m_blockSize)
				findMatchesFor(transforms, channel , x, y, m_blockSize);
			std::cout << y << std::endl;
		}

		delete []m_img.r_channel;
		m_img.r_channel = nullptr;
		delete []m_img.b_channel;
		m_img.b_channel = nullptr;
	}

	return std::move(transforms);
}

void Encoder::findMatchesFor(Transforms& transforms, int channel, int toX, int toY, int blockSize)
{
	int bestX = 0;
	int bestY = 0;
	int bestOffset = 0;
	IFSTransform::SYM bestSymmetry = IFSTransform::SYM_NONE;
	double bestScale = 0;
	double bestError = 1e9;

	uint8_t buffer[blockSize * blockSize];

	// Get average pixel for the range block
	int rangeAvg = GetAveragePixel(m_img.r_channel, m_img.width, toX, toY, blockSize);

	// Go through all the downsampled domain blocks
	for (int y = 0; y < m_img.height; y += blockSize * 2)
	{
		for (int x = 0; x < m_img.width; x += blockSize * 2)
		{
			for (int symmetry = 0; symmetry < IFSTransform::SYM_MAX; symmetry++)
			{
				auto symmetryEnum = (IFSTransform::SYM)symmetry;
				IFSTransform ifs = IFSTransform(0, 0, blockSize/*, symmetryEnum*/, 1.0, 0, blockSize, channel);
				ifs.Execute(m_img.b_channel, m_img.width / 2, buffer, blockSize, true, x / 2, y / 2);

				// Get average pixel for the downsampled domain block
				int domainAvg = GetAveragePixel(buffer, blockSize, 0, 0, blockSize);

				// Get scale and offset
				double scale = GetScaleFactor(m_img.r_channel, m_img.width, toX, toY, domainAvg,
											  buffer, blockSize, 0, 0, rangeAvg, blockSize);
				int offset = (int)(rangeAvg - scale * (double)domainAvg);

				// Get error and compare to best error so far
				double error = GetError(buffer, blockSize, 0, 0, domainAvg,
										m_img.r_channel, m_img.width, toX, toY, rangeAvg, blockSize, scale);

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
		findMatchesFor(transforms, channel, toX, toY, blockSize);
		findMatchesFor(transforms, channel, toX + blockSize, toY, blockSize);
		findMatchesFor(transforms, channel, toX, toY + blockSize, blockSize);
		findMatchesFor(transforms, channel, toX + blockSize, toY + blockSize, blockSize);
	}
	else
	{
		// Use this transformation
		transforms.transforms[DomainBlock(bestX, bestY, m_img.width)].push_back(
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
