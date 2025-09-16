// 头文件包含，分别是TGA图片处理、模型加载、输入输出、时间和随机数相关
#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm> // 用于std::max和std::min
using std::cout; using std::endl;

// 定义模型指针
Model *model = NULL;
// 定义输出图片的宽高
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
  // u[2]为三角形面积的两倍，若为0则三角形退化
  if (std::abs(u[2])>1e-2)
    // 返回重心坐标（归一化）
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
  // 如果三角形退化，返回无效坐标
  return Vec3f(-1,1,1); // 负坐标会被光栅化器丢弃
}

void triangle(Vec2i *pts, Vec3f *world, TGAImage &image, std::vector<float> &zbuffer, TGAColor color) {
  Vec2i bboxmin(image.get_width()-1, image.get_height()-1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(image.get_width()-1, image.get_height()-1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec2i P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ) continue;
      // 插值计算当前像素的z值
      float z = world[0].z * bc_screen.x + world[1].z * bc_screen.y + world[2].z * bc_screen.z;
      int idx = P.x + P.y * image.get_width();
      if (zbuffer[idx] < z) { // 只有更近的像素才更新
        zbuffer[idx] = z;
        image.set(P.x, P.y, color);
      }
    }
  }
}

// 世界坐标转屏幕坐标（归一化到[-1,1]后映射到屏幕像素）
Vec2i world2screen(Vec3f v){
  return Vec2i(int((v.x+1.)*width/2.), int((v.y+1.)*height/2.));
}

int main(int argc, char** argv){
  // ...existing code...
  TGAImage image(width, height, TGAImage::RGB); // 创建图片对象
  std::vector<float> zbuffer(width * height, -std::numeric_limits<float>::max()); // 初始化z-buffer，初始值为最远
  Vec3f light(0, 0, -1); // 定义光照方向（z轴负方向）

  // 遍历模型所有面
  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i); // 获取当前面的顶点索引
    Vec3f world_coords[3]; // 三角形顶点的世界坐标
    Vec2i screen_coords[3]; // 三角形顶点的屏幕坐标
    for (int j = 0; j < 3; j++) {
      world_coords[j] = model->vert(face[j]); // 获取顶点世界坐标
      screen_coords[j] = world2screen(world_coords[j]); // 转换为屏幕坐标
    }

    // 计算三角形的法线
    Vec3f norm = cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]);
    norm.normalize(); // 单位化法线
    float intensity = light*norm; // 计算光照强度（点乘）
    if (intensity > 0) { // 只有正面朝向光源才绘制
      // 使用z-buffer的三角形填充
      triangle(screen_coords, world_coords, image, zbuffer, TGAColor(intensity*255,intensity*255,intensity*255,255));
    }
  }

  image.flip_vertically(); // 翻转图片，使原点在左下角
  image.write_tga_file("output.jpg"); // 保存图片
  return 0;
}
