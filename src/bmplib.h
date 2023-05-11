/*
   Sample program to read a limited number of BMP file types
   Write out lots of diagnostics
   Write out a 24 bit RAW image file

   SourceCode was published at: http://paulbourke.net/dataformats/bmp/parse.c

   modified on XX.11.18 [J]
*/


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "colorspaces.h"

#define LE 0x4D42


typedef struct Header {
   uint16_t type;                       /* Magic identifier            */
   uint32_t size;                       /* File m_size in bytes          */
   uint16_t reserved1, reserved2;
   uint32_t offset;                     /* Offset to image data, bytes */
} Header;


typedef struct InfoHeader {
   uint32_t size;                       /* Header m_size in bytes      */
   uint32_t width, height;              /* Width and height of image */
   uint16_t planes;                     /* Number of colour planes   */
   uint16_t bits;                       /* Bits per RGBPixel            */
   uint32_t compression;                /* Compression type          */
   uint32_t imagesize;                  /* Image m_size in bytes       */
   uint32_t xresolution, yresolution;   /* Pixels per meter          */
   uint32_t ncolors;                    /* Number of colours         */
   uint32_t important_colors;           /* Important colours         */
} InfoHeader;


typedef struct RGBPixel {
  uint8_t r, g, b;
  uint8_t junk;
} RGBPixel;


typedef struct Image {
    RGBPixel **pixels;
    Header header;
    InfoHeader info_header;
} Image;


enum ColorSpace {
  RGB,
  YUV,
};


typedef struct ImageMatrix {
  uint32_t width, height;
  ColorSpace colorspace;

  uint8_t *r_channel;
  uint8_t *g_channel;
  uint8_t *b_channel;

  uint8_t *y_channel;
  uint8_t *u_channel;
  uint8_t *v_channel;
} ImageMatrix;


Image  *load_image        (const char *filename);
bool    save_image        (const char *filename, Image *image);
bool    draw_solid_image  (const char *filename, uint32_t height, uint32_t width, 
                                                 uint8_t color[3]);

ImageMatrix  *get_image_matrix  (Image *base_image, ColorSpace colorspace);   

bool ReadBytes       (FILE *input,  void *buffer, size_t size, bool swap = false);
bool WriteBytes      (FILE *output, const void *buffer, size_t size, bool swap = false);

       void delete_image        (Image *image);
       void delete_image_matrix (ImageMatrix *matrix);
static void delete_pixels       (RGBPixel **pixels, int32_t count); 


bool ReadHeader      (FILE *input, Header *buffer);
bool ReadInfoHeader  (FILE *input, InfoHeader *buffer);
bool WriteHeader     (FILE *input, Header *header);
bool WriteInfoHeader (FILE *input, InfoHeader *info_header);

bool SaveImageWithMatrix(ImageMatrix *img, const char *file_name);
