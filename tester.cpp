#include <iostream>
#include "src/extension.h"
#include "src/colorspaces.h"
#include "src/bmplib.h"
#include "src/measure.h"


#define SEPARATOR cout << endl << endl; 


using std::cout;
using std::cin;
using std::endl;


void test_extension_h(void);


const char *file_names[] = {
    "some_file.bmp", 
    "second.file.BmP",
    "skad.BMp.",
    "some_file.AvI",
    "AVI",
    "A.v.I",
    "",
    "BMp.aVi"
};


int main(int argc, char const *argv[])
{
    test_extension_h(); SEPARATOR

    uint8_t color[3] = {124, 32, 56}; draw_solid_image("images/out.bmp", 512, 512, color);
    FILE *f = fopen("images/out.bmp", "rb"); if (!f) puts("Failed to open image!\n");
    Header header; if (!ReadHeader(f, &header)) puts("Header load failed!\n");
    InfoHeader info_header; if(!ReadInfoHeader(f, &info_header)) puts("InfoHeader load failed!\n");
    fclose(f); SEPARATOR

    Image *image = load_image("images/out.bmp"); SEPARATOR

    ImageMatrix *matrix = get_image_matrix(image, YUV);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            printf("(%u, %u, %u) ", matrix->y_channel[i][j], matrix->u_channel[i][j], matrix->v_channel[i][j]);
        } putchar('\n');
    } SEPARATOR

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            uint8_t r = GetRFromYUV(matrix->y_channel[i][j], matrix->u_channel[i][j], matrix->v_channel[i][j]);
            uint8_t g = GetGFromYUV(matrix->y_channel[i][j], matrix->u_channel[i][j], matrix->v_channel[i][j]);
            uint8_t b = GetBFromYUV(matrix->y_channel[i][j], matrix->u_channel[i][j], matrix->v_channel[i][j]);
            printf("(%u, %u, %u) ", r, g, b);
        } putchar('\n');
    } SEPARATOR

    Image *first  = load_image("images/PSNR-example-base.bmp"); SEPARATOR
    Image *second = load_image("images/PSNR-example-comp-90.bmp"); SEPARATOR

    cout << "Total PSNR: " << ImagePSNR(first, second) << endl;
    return 0;
}


void test_extension_h(void) 
{
    cout << "==========================================================" << endl;
    for (int i = 0; i < sizeof(file_names) / sizeof(const char *); ++i) {
        cout << "test #" << i << " for string: \"" << file_names[i] << "\":" << endl;
        cout << "\tFileExtBMP returns: " << FileExtBMP(file_names[i]) << endl;
        cout << "\tFileExtAVI returns: " << FileExtAVI(file_names[i]) << endl;
        cout << "==========================================================" << endl;
    }
}