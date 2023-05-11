//
// Created by Денис on 08.04.2023.
//

#ifndef MYFRACTALCOMPRESSION_IFSTRANSFORM_H
#define MYFRACTALCOMPRESSION_IFSTRANSFORM_H

#include <vector>
#include <map>
#include <cstdint>

class IFSTransform;

struct DomainBlock{
	uint16_t m_from_range;
	static const int fix_size = 2;
	DomainBlock() = default;
	DomainBlock(int x, int y, int width) {setXY(width, x, y);};

	int getX(int width) const { return calculateByte(width, 1);};
	int getY(int width) const { return calculateByte(width, 2);};

	bool operator ==(const DomainBlock &other) const;
	bool operator <(const DomainBlock &other) const;

	void setXY(int width, int x, int y);
	// std::vector<IFSTransform> transform;
private:
	int calculateByte(int width, int byte) const;

};

class Transforms
{
public:
	Transforms()= default;

	void safeData(const char *fileName);
	void loadData(const char *fileName);

	~Transforms()= default;

public:
	int16_t width;
	std::map<DomainBlock, std::vector<IFSTransform>> transforms;
};

class IFSTransform {
public:

	enum SYM {
		SYM_NONE = 0, SYM_R90, SYM_R180, SYM_R270, SYM_HFLIP,
		SYM_VFLIP, SYM_FDFLIP, SYM_RDFLIP, SYM_MAX
	};

	IFSTransform() = default;
	IFSTransform(int16_t toX, int16_t toY, uint16_t size, float scale, int16_t offset, int16_t width,
				 int16_t chanel);

	void Execute(uint8_t *src, uint32_t srcWidth, uint8_t *dest, int destWidth, bool downsampled, int fromX,
				 int fromY);

	int getX(int width) const;
	int getY(int width) const;
	int getSize(int width) const;
	int getChanel(int width) const;
	int getOffset(int width) const;

static uint8_t * DownSample(uint8_t* src, uint32_t srcWidth, int startX, int startY, uint32_t targetSize);

private:
	int calculateByte(int width, int byte) const;

//	bool isScanlineOrder();
//	bool isPositiveX();
//	bool isPositiveY();

	float m_scale{};
private:
	uint16_t m_rang;
	uint16_t m_offset{};
};

#endif //MYFRACTALCOMPRESSION_IFSTRANSFORM_H
