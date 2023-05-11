//
// Created by Денис on 08.04.2023.
//

#ifndef MYFRACTALCOMPRESSION_DECODER_H
#define MYFRACTALCOMPRESSION_DECODER_H

#include "IFSTransform.h"
#include "../src/bmplib.h"

class Decoder {
public:
	Decoder(int width, int height);

	void Decode(Transforms &transforms, int phase = 0);

	ImageMatrix m_img;
};


#endif //MYFRACTALCOMPRESSION_DECODER_H
