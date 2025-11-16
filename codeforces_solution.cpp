#include <chrono>
#include <random>
#include <vector>
#include <iostream>
#include <iomanip>

std::random_device rd;
std::mt19937 generator(rd());
std::uniform_real_distribution<double> distribution(0, 1);

struct Point {
  double x;
  double y;
};

struct Circle {
  Point center;
  double radius;
  [[nodiscard]] bool Enclosed(Point p) const {
    return (center.x - p.x) * (center.x - p.x) + (center.y - p.y) * (center.y - p.y) <= radius * radius;
  }
};

double Generate(double left, double right) {
  return left + (right - left) * (distribution(generator));
}

std::pair<Point, Point> MakeWideRectangle(const std::vector<Circle>& circles) {
  std::vector<Point> points;
  points.reserve(12);
  for (const auto& c : circles) {
    points.push_back(Point{c.center.x + c.radius, c.center.y});
    points.push_back(Point{c.center.x - c.radius, c.center.y});
    points.push_back(Point{c.center.x, c.center.y + c.radius});
    points.push_back(Point{c.center.x, c.center.y - c.radius});
  }
  Point r_up{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};
  Point l_down{std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
  for (const auto& p : points) {
    r_up.x = std::max(r_up.x, p.x);
    r_up.y = std::max(r_up.y, p.y);
    l_down.x = std::min(l_down.x, p.x);
    l_down.y = std::min(l_down.y, p.y);
  }
  return {l_down, r_up};
}

int main() {
  double x, y, r;
  std::vector<Circle> circles(3);
  for (int i{}; i < 3; i++) {
    std::cin >> x >> y >> r;
    const Point center = {x, y};
    circles[i] = {center, r};
  }

  if (circles[0].radius == 0 || circles[1].radius == 0 || circles[2].radius == 0) {
    std::cout << 0.0;
    return 0;
  }
  auto [fst, snd] = MakeWideRectangle(circles);
  const double S = (snd.x - fst.x) * (snd.y - fst.y);
  int N = 2000000;
  double M{};

  for (int i = 0; i < N; i++) {
    Point p = {Generate(fst.x, snd.x), Generate(fst.y, snd.y)};
    if (circles[0].Enclosed(p) && circles[1].Enclosed(p) && circles[2].Enclosed(p)) {
      M++;
    }
  }
  const double result = M / N * S;
  std::cout << result;
  return 0;
}