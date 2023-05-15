//
// Created by Денис on 08.04.2023.
//

#include "IFSTransform.h"
#include "../src/bmplib.h"
#include <iostream>
#include <zlib.h>
#include <sstream>

void IFSTransform::Execute(uint8_t *src, uint32_t srcWidth, uint8_t *dest, int destWidth, bool downsampled, int fromX,
						   int fromY)
{
	int dX = 1;
	int dY = 1;
	bool inOrder = true; //isScanlineOrder();

	if (!downsampled)
	{
		uint8_t* newSrc = DownSample(src, srcWidth, fromX, fromY, getSize(destWidth));
		src = newSrc;
		srcWidth = getSize(destWidth);
		fromX = fromY = 0;
	}

//	if (!isPositiveX())
//	{
//		fromX += m_size - 1;
//		dX = -1;
//	}
//
//	if (!isPositiveY())
//	{
//		fromY += m_size - 1;
//		dY = -1;
//	}

	int startX = fromX;
	int startY = fromY;

	const int const_x = getX(destWidth);
	const int size_x = getX(destWidth) + getSize(destWidth);

	for (int toY = getY(destWidth), size_y = getY(destWidth) + getSize(destWidth); toY < size_y; toY++)
	{
		for (int toX = const_x; toX < size_x; toX++)
		{

			int pixel = src[fromY * srcWidth + fromX];
			pixel = (int)(m_scale * pixel) + getOffset(destWidth);

			if (pixel < 0)
				pixel = 0;
			if (pixel > 255)
				pixel = 255;

			dest[toY * destWidth + toX] = pixel;

			if (inOrder)
				fromX += dX;
			else
				fromY += dY;
		}

		if (inOrder)
		{
			fromX = startX;
			fromY += dY;
		}
		else
		{
			fromY = startY;
			fromX += dX;
		}
	}

	if (!downsampled)
	{
		delete []src;
		src = nullptr;
	}
}


//bool IFSTransform::isScanlineOrder()
//{
//	return (
//			m_symmetry == SYM_NONE ||
//			m_symmetry == SYM_R180 ||
//			m_symmetry == SYM_HFLIP ||
//			m_symmetry == SYM_VFLIP
//	);
//}
//
//bool IFSTransform::isPositiveX()
//{
//	return (
//			m_symmetry == SYM_NONE ||
//			m_symmetry == SYM_R90 ||
//			m_symmetry == SYM_VFLIP ||
//			m_symmetry == SYM_RDFLIP
//	);
//}
//
//bool IFSTransform::isPositiveY()
//{
//	return (
//			m_symmetry == SYM_NONE ||
//			m_symmetry == SYM_R270 ||
//			m_symmetry == SYM_HFLIP ||
//			m_symmetry == SYM_RDFLIP
//	);
//}

uint8_t *IFSTransform::DownSample(uint8_t *src, uint32_t srcWidth, int startX, int startY, uint32_t targetSize)
{
	auto* dest = new uint8_t[targetSize * targetSize];
	int destX = 0;
	int destY = 0;

	for (int y = startY; y < startY + targetSize * 2; y += 2)
	{
		for (int x = startX; x < startX + targetSize * 2; x += 2)
		{
			// Perform simple 2x2 average
			int pixel = 0;
			pixel += src[y * srcWidth + x];
			pixel += src[y * srcWidth + (x + 1)];
			pixel += src[(y + 1) * srcWidth + x];
			pixel += src[(y + 1) * srcWidth + (x + 1)];
			pixel /= 4;

			dest[destY * targetSize + destX] = pixel;
			destX++;
		}
		destY++;
		destX = 0;
	}

	return dest;
}

IFSTransform::IFSTransform(int16_t toX, int16_t toY, uint16_t size, float scale, int16_t offset, int16_t width,
						   int16_t chanel) : m_scale(scale)
{
	m_offset = 0;
	m_offset += chanel << (sizeof(m_offset) * 8 - 2);
	m_offset += size << (sizeof(m_offset) * 8 - 6);

	int16_t copy_offset = offset;
	if (offset < 0)
	{
		m_offset += 0x0200;
		copy_offset = abs(offset);
	}
	m_offset += copy_offset;

	int count = width;
	int x_count{}, y_count{};

	m_rang = 0;
	m_rang += (uint16_t)(toX >> 1) << 8;
	m_rang += (uint16_t)(toY >> 1);

//	if (toX != getX(width))
//		throw std::runtime_error("ERROR");
//	if (toY != getY(width))
//		throw std::runtime_error("ERROR");
//	if (size != getSize(width))
//		throw std::runtime_error("ERROR");
//	if (chanel != getChanel(width))
//		throw std::runtime_error("ERROR");
//	if (offset != getOffset(width))
//		throw std::runtime_error("ERROR");
}

int IFSTransform::getX(int width) const{
	return (m_rang & 0xff00) >> (8 - 1);
}

int IFSTransform::getY(int width) const{
	return  (m_rang & 0x00ff) << 1;
}

int IFSTransform::getSize(int width) const{
	return (m_offset & 15 << (sizeof(m_offset) * 8 - 6)) >> (sizeof(m_offset) * 8 - 6);
}

int IFSTransform::calculateByte(int width, int byte) const{
	int max_path = log2(width);
	int size = max_path - log2(getSize(width));
	int result = 0;

	if (size < 0)
		throw std::runtime_error("ERROR");

	for(size_t i = 0; i < size; ++i)
		result += ((m_rang >> i * 2) & byte) ? pow(2, max_path - size + i) : 0;

	return result;
}

int IFSTransform::getChanel(int width) const{
	return (m_offset & 15 << (sizeof(m_offset) * 8 - 2)) >> (sizeof(m_offset) * 8 - 2);
}

int IFSTransform::getOffset(int width) const{
	if (m_offset & 0x0200)
		return -((uint16_t)(m_offset << 7) >> 7);
	return (uint16_t)(m_offset << 6) >> 6;
}

void Transforms::safeData(const char *fileName) {
	FILE *output = fopen(fileName, "wb");

	WriteBytes(output, &width, sizeof(width));

	for (auto &items : transforms)
	{
		WriteBytes(output, &(items.first), sizeof(items.first));
		uint8_t size = items.second.size();
		WriteBytes(output, &size, sizeof(size));
		for (auto &item : items.second)
			WriteBytes(output, &item, sizeof(item));
	}
	fclose(output);
}

void Transforms::loadData(const char *fileName) {
	FILE *input = fopen(fileName, "rb");

	fseek(input, 0, SEEK_END);
	size_t fileSize = ftell(input);
	fseek(input, 0, SEEK_SET);

	ReadBytes(input, &width, sizeof(width));
	DomainBlock temp;
	std::vector<IFSTransform> temp_vector;
	while(ReadBytes(input, &temp, sizeof(temp)))
	{
		uint8_t size = 0;
		ReadBytes(input, &size, sizeof(size));
		temp_vector.resize(size);
		ReadBytes(input, temp_vector.data(), sizeof(IFSTransform) * size);

		transforms[temp] = temp_vector;
	}
	fclose(input);
}

int DomainBlock::calculateByte(int width, int byte) const{
	int max_path = log2(width);
	int size = max_path - log2(fix_size);
	int result = 0;

	if (size < 0)
		throw std::runtime_error("ERROR");

	for(size_t i = 0; i < size; ++i)
		result += ((m_from_range >> i * 2) & byte) ? pow(2, max_path - size + i) : 0;

	return result;
}

void DomainBlock::setXY(int width, int x, int y)
{
	int count = width;
	int x_count{}, y_count{};

	while(fix_size != count)
	{
		m_from_range <<= 2;
		count >>= 1;
		if (x >= count + x_count){
			++m_from_range;
			x_count += count;
		}
		if (y >= count + y_count){
			m_from_range += 2;
			y_count += count;
		}
	}

//	if (x != getX(width))
//		throw std::runtime_error("ERROR");
//	if (y != getY(width))
//		throw std::runtime_error("ERROR");
}

bool DomainBlock::operator==(const DomainBlock &other) const{
	return m_from_range == other.m_from_range;
}

bool DomainBlock::operator<(const DomainBlock &other) const {
	return m_from_range < other.m_from_range;
}
