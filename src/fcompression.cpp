#include "fcompression.h"
#include "../Compress/Encoder.h"
#include "../Compress/Decoder.h"


/// Compress an image
void
Compress(const char *image_filename,
         const char *archive_filename,
         const int block_size,
         const int quality)
{
    // Load the input image
    Image *image = load_image(image_filename);
	if (!image)
		exit(-1);
    const int32_t width = image->info_header.width;
    const int32_t height = image->info_header.height;

    if ((width != 256 && width != 512) || width != height)
        throw std::runtime_error("Input image should be "
                                 "256x256 or 512x512 pixels");

	Encoder encoder(quality, block_size, false);
	auto transforms = encoder.Encode(image);
	transforms.safeData(archive_filename);
}


/// Decompress an image
void
Decompress(const char *archive_filename,
           const char *image_filename)
{
	Transforms transforms;
	transforms.loadData(archive_filename);
	Decoder decoder(transforms.width, transforms.width);
	for (int phase = 1; phase <= 5; phase++)
		decoder.Decode(transforms, phase);

	SaveImageWithMatrix(&decoder.m_img, image_filename);

}
