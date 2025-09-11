#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main (int argc, char** argv) {
    float x = 10.3;
    float y = 10.3;
    TGAImage image(100, 100, TGAImage::RGB);
    image.set(x, y, red);
    image.flip_vertically(); // i want to have the origin at the left bottom corner
    image.write_tga_file("output.png");
}