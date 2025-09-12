#include "tgaimage.h"
#include <cstdlib>  // 添加头文件
#include <ctime>    // 添加头文件

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main (int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);

    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 初始化随机数种子

    for (size_t i = 0; i < 100; i++)
    {
        int x = std::rand() % 100; 
        int y = std::rand() % 100;  
        image.set(x, y, red);
    }

    image.flip_vertically();
    image.write_tga_file("output.png");
}