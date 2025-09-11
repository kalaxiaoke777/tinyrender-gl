#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line (int startX, int startY, int endX, int endY, TGAImage &image, TGAColor color)
{
    float t = (endY - startY)/(float)(endX - startX);
    for (size_t x = 0; x < endX; x++)
    {
        int y = startY + t * (x - startX);
        image.set(x, y, color);
    }
    
}

int main (int argc, char** argv) {
    size_t count = 100;
    TGAImage image(100, 100, TGAImage::RGB);
    line(20, 13, 80, 90, image, red);
    image.flip_vertically(); // i want to have the origin at the left bottom corner
    image.write_tga_file("output.png");
}