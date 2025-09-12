#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line (int startX, int startY, int endX, int endY, TGAImage &image, TGAColor color)
{
    bool steep = false;
    // 如果斜率大于1，则交换x,y坐标
    if (std::abs(startX-endX)<std::abs(startY-endY)) {
        std::swap(startX, startY);
        std::swap(endX, endY);
        steep = true;
    }
    // 确保startX小于endX
    if (startX > endX) {
        std::swap(startX, endX);
        std::swap(startY, endY);
    }
    // 计算出斜率T
    float t = (endY - startY)/(float)(endX - startX);
    for (size_t x = startX; x <= endX; x++)
    {
        // 根据T不断绘制出Y
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