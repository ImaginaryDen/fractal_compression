//
// Created by Денис on 08.04.2023.
//

#include "Decoder.h"
#include <sstream>
#include <iostream>
Decoder::Decoder(int width, int height)
{
	m_img.width = width;
	m_img.height = height;

	m_img.r_channel = new uint8_t[width * height];
	m_img.g_channel = new uint8_t[width * height];
	m_img.b_channel = new uint8_t[width * height];

	// Initialize to grey image
	for (int i = 0; i < m_img.width * m_img.height; i++)
	{
		m_img.r_channel[i] = 127;
		m_img.g_channel[i] = 127;
		m_img.b_channel[i] = 127;
	}
}

void Decoder::Decode(Transforms &transforms, int phase)
{
	size_t num_iter = 0;
	uint8_t* origImage;

	for (auto &transform : transforms.transforms)
		for (auto &t : transform.second)
		{
			switch (t.getChanel(transforms.width))
			{
				case 1:
					origImage = m_img.r_channel;
					break;
				case 2:
					origImage = m_img.g_channel;
					break;
				case 3:
					origImage = m_img.b_channel;
					break;
				default:
					throw std::runtime_error("Error not real chanel");
			}
			t.Execute(origImage, m_img.width, origImage, transforms.width, false,
					  transform.first.getX(transforms.width), transform.first.getY(transforms.width));
		}
}