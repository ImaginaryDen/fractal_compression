#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>

#include "fcompression.h"
#include "extension.h"
#include "bmplib.h"
#include "measure.h"


enum AppMode
{
    COMPRESSION,
    DECOMPRESSION,
    PSNR,
    UNKNOWN
};


int main(int argc, char **argv)
{
    try
    {
        AppMode mode = UNKNOWN;
        int quality = 50;
        int block_size = 8;
        std::string image_filename;
        std::string archive_filename;

        const char *first_image;
        const char *second_image;

        Image *first, *second;

        for (int i = 1; i < argc; i++)
        {
            if (!strcmp(argv[i], "-c"))
            {
                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");
                image_filename = std::string(argv[i]);

                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");
                archive_filename = std::string(argv[i]);

                mode = COMPRESSION;

            }
            else if (!strcmp(argv[i], "-d"))
            {
                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");
                archive_filename = std::string(argv[i]);

                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");
                image_filename = std::string(argv[i]);

                mode = DECOMPRESSION;

            }
            else if (!strcmp(argv[i], "-psnr")) 
            {
                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");
                first_image = argv[i];

                i++;
                if (!argv[i]) 
                    throw std::runtime_error("Not enough arguments");
                second_image = argv[i];

                mode = PSNR;
            }
            else if (!strcmp(argv[i], "-s"))
            {
                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");

                block_size = atoi(argv[i]);
                if (block_size < 1)
                    throw std::runtime_error("Block m_size should be positive");
            }
            else if (!strcmp(argv[i], "-q"))
            {
                i++;
                if (!argv[i])
                    throw std::runtime_error("Not enough arguments");

                quality = atoi(argv[i]);
                if (quality < 1 || quality > 100)
                    throw std::runtime_error("Quality should be in range [1..100]");
            }
            else
            {
                throw std::runtime_error("Unknown command line option");
            }
        }

        switch(mode)
        {
            case COMPRESSION:
                std::cout << "Compression mode" << std::endl;
                if (FileExtBMP(image_filename.c_str())) {
                    Compress(image_filename.c_str(), archive_filename.c_str(),
                             block_size, quality);
                    std::cout << "Done" << std::endl;
                } else {
                    throw std::runtime_error("Format isn't supported");
                }
                break;
            case DECOMPRESSION:
                std::cout << "Decompression mode" << std::endl;
                if (FileExtBMP(image_filename.c_str())) {
                    Decompress(archive_filename.c_str(), image_filename.c_str());
                    std::cout << "Done" << std::endl;
                } else {
                    throw std::runtime_error("Output file must have BMP extension");
                }
                break;

            case PSNR:
                std::cout << "PSNR metric:" << std::endl;
                if (!(FileExtBMP(first_image) && (FileExtBMP(second_image))))
                    throw std::runtime_error("Images must have .bmp extension");

                first = load_image(first_image);
                second = load_image(second_image);

                if (!(first && second)) 
                    throw std::runtime_error("Failed to load images");

                ImagePSNR(first, second);
                delete_image(first); delete_image(second);
                break;

            default:
                std::cout << "Usage:\n -c input.bmp output.fr [-s block_size] "
                             "[-q qualitty]\n -d input.fr output.bmp\n"
                             " -psnr first_image.bmp second_image.bmp" <<
                             std::endl;
                break;
        }
    }
    catch(const std::runtime_error &error)
    {
        std::cerr << "Error: " << error.what() << std::endl;
    }
    return 0;
}
