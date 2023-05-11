//
// Created by Денис on 08.04.2023.
//

#ifndef MYFRACTALCOMPRESSION_ENCODER_H
#define MYFRACTALCOMPRESSION_ENCODER_H

#include "../src/bmplib.h"
#include "IFSTransform.h"
#include <thread>
#include <mutex>
#include <functional>

class Encoder {
public:
	Encoder(int quality, int blockSize, bool symmetry = true)
	: m_quality(quality), m_blockSize(blockSize), m_symmetry(symmetry)
	{};

	Transforms Encode(Image* source);

protected:
	void findMatchesFor(const ImageMatrix &img, Transforms &transforms, int channel, int toX, int toY, int blockSize);

	// These functions are helpers
	static int GetAveragePixel(const uint8_t* domainData, int domainWidth, int domainX, int domainY, int size);

	static double GetScaleFactor(
			const uint8_t* domainData, uint32_t domainWidth, int domainX, int domainY, int domainAvg,
			const uint8_t* rangeData, int rangeWidth, int rangeX, int rangeY, int rangeAvg,
			int size);

	static double GetError(
			const uint8_t* domainData, int domainWidth, int domainX, int domainY, int domainAvg,
			const uint8_t* rangeData, uint32_t rangeWidth, int rangeX, int rangeY, int rangeAvg,
			int size, double scale);

	void fractalCompressionSingleChannel(int channel, Image *origin_img, Transforms &transforms);

protected:
	int m_quality;
	int m_blockSize;
	bool m_symmetry;

	std::mutex mtx;
};


#endif //MYFRACTALCOMPRESSION_ENCODER_H
