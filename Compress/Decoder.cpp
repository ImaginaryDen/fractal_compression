//
// Created by Денис on 08.04.2023.
//

#include "Decoder.h"
#include <sstream>
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

	for (auto &transform : transforms.transforms)
		for (auto &t : transform.second)
		{
			int channel = t.getChanel(transforms.width);
			++num_iter;
			uint8_t* origImage = m_img.r_channel;
			if (channel == 2) {
				origImage = m_img.g_channel;
			}
			else if (channel == 3) {
				origImage = m_img.b_channel;
			}

			t.Execute(origImage, m_img.width, origImage, transforms.width, false, transform.first.getX(transforms.width), transform.first.getY(transforms.width));
			if (num_iter % 1000 != 0 || !phase)
				continue;
			std::stringstream ss;
			ss << "intermediate/" << "_phase_" << phase << "_iter_" << num_iter << ".bmp";
			SaveImageWithMatrix(&m_img, ss.str().c_str());

			// Apple each transform at a time to this channel
//			iter = transforms->ch[channel-1].begin();
//			for (size_t num_iter = 0; iter != transforms->ch[channel-1].end(); iter++, num_iter++)
//			{
//				iter[0]->Execute(origImage, m_img.width, origImage, m_img.width, false, 0, 0);
//
//				if (num_iter % 1000 != 0 || !phase)
//					continue;
//				std::stringstream ss;
//				ss << "intermediate/" << "test_file_channel_" << channel << "_phase_" << phase << "_iter_"
//				<< (int)(iter - transforms->ch[channel-1].begin()) << ".bmp";
//				SaveImageWithMatrix(&m_img, ss.str().c_str());
//			}
		}
}