#ifndef __FRACTALCOMPRESSION_H__
#define __FRACTALCOMPRESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include "bmplib.h"
#include "measure.h"


/**
 * Compress an image
 * @param image_filename - input path
 * @param archive_filename - output path
 * @param block_size - m_size of the block
 * @param quality - compresion m_quality in range [1..100]
 */
void
Compress(const char *image_filename,
         const char *archive_filename,
         const int block_size,
         const int quality);

/**
 * Decompress an image
 * @param archive_filename - input path
 * @param image_filename - output path
 */
void
Decompress(const char *archive_filename,
           const char *image_filename);


#endif