#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
using std::cout; 
using std::endl;


Model *model = NULL;
const int width = 600;
const int height = 600;

// 定义颜色常量
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

// 计算点P在三角形ABC中的重心坐标
Vec3f barycentric(Vec2i A, Vec2i B, Vec2i C, Vec2i P) {
  Vec3f s[2];
  // 构造两个向量，用于叉乘求重心坐标
  for (int i=2; i--; ) {
    s[i][0] = C[i]-A[i];
    s[i][1] = B[i]-A[i];
    s[i][2] = A[i]-P[i];
  }
  Vec3f u = cross(s[0], s[1]); // 叉乘得到重心坐标分量
  if (std::abs(u[2])>1e-2)
    // 返回重心坐标（归一化）
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
  // 如果三角形退化，返回无效坐标
  return Vec3f(-1,1,1); 
}

// 填充三角形，pts为屏幕坐标的三个顶点
void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
  // 计算三角形的包围盒（最小矩形）
  Vec2i bboxmin(image.get_width()-1, image.get_height()-1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(image.get_width()-1, image.get_height()-1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j])); // 更新最小值
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j])); // 更新最大值
    }
  }
  Vec2i P;
  // 遍历包围盒内所有像素点
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P); // 计算重心坐标
      // 如果点在三角形内（重心坐标都为正）则填充颜色
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ) continue;
      image.set(P.x, P.y, color);
    }
  }
}

// 世界坐标转屏幕坐标（归一化到[-1,1]后映射到屏幕像素）
Vec2i world2screen(Vec3f v){
  return Vec2i(int((v.x+1.)*width/2.), int((v.y+1.)*height/2.));
}

int main(int argc, char** argv){
  // 加载模型文件，如果有参数则用参数，否则用默认模型
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  TGAImage image(width, height, TGAImage::RGB); // 创建图片对象
  srand(time(NULL)); // 初始化随机数种子

  // 遍历模型所有面
  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i); // 获取当前面的顶点索引
    Vec3f world_coords[3]; // 三角形顶点的世界坐标
    Vec2i screen_coords[3]; // 三角形顶点的屏幕坐标
    for (int j = 0; j < 3; j++) {
      world_coords[j] = model->vert(face[j]); // 获取顶点世界坐标
      screen_coords[j] = world2screen(world_coords[j]); // 转换为屏幕坐标
    }

    // 随机生成颜色
    int r = std::rand() % 255;
    int g = std::rand() % 255;
    int b = std::rand() % 255;

    triangle(screen_coords, image, TGAColor(r,g,b,255)); // 绘制三角形
  }

  image.flip_vertically(); // 翻转图片，使原点在左下角
  image.write_tga_file("output.jpg"); // 保存图片
  return 0;
}
