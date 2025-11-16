#include <cmath>
#include <random>
#include <vector>
#include <fstream>
#include <tuple>
#include <string>

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

double Generate(const double left, const double right) {
  return left + (right - left) * (distribution(generator));
}

double PersistentError(const double& true_val, const double& calc_val) {
  return abs(calc_val - true_val) / true_val;
}

double MonteCarlo(const int& N, const std::vector<Circle>& circles, const Point& left_down, const Point& right_up) {
  double M{};
  const double S = (right_up.x - left_down.x) * (right_up.y - left_down.y);
  for (int i = 0; i < N; i++) {
    Point p = {Generate(left_down.x, right_up.x), Generate(left_down.y, right_up.y)};
    if (circles[0].Enclosed(p) && circles[1].Enclosed(p) && circles[2].Enclosed(p)) {
      M++;
    }
  }
  return M / N * S;
}

void SaveSDtoCSV(const std::string& filename, const std::vector<std::tuple<int, double, std::string>>& data) {
  std::ofstream out(filename);
  out << "N,Value,RectType\n";
  for (auto& [N, value, type] : data) {
    out << N << "," << value << "," << type << "\n";
  }
}

void SaveEtoCSV(const std::string& filename, const std::vector<std::tuple<int, double, std::string>>& data) {
  std::ofstream out(filename);
  out << "N,Error,RectType\n";
  for (auto& [N, err, type] : data) {
    out << N << "," << err << "," << type << "\n";
  }
}

int main() {
  // подготавливаем данные
  // окружности
  std::vector<Circle> circles(3);
  circles[0] = {{1, 1}, 1};
  circles[1] = {{1.5, 2}, sqrt(5) / 2};
  circles[2] = {{2, 1.5}, sqrt(5) / 2};

  // прямоугольники
  // узкий
  Point tight_ld = {0.88, 0.88};
  Point tight_ru = {2, 2};
  // широкий
  Point wide_ld = {0, 0};
  Point wide_ru = {3.12, 3.12};

  // точная площадь
  double s_true = 0.25 * acos(-1.0) + 1.25 * asin(0.8) - 1;

  // количество точек
  std::vector<int> n_arr(199);
  int val = 100;
  for (int& i : n_arr) {
    i = val;
    val += 500;
  }

  // проводим тесты
  // площадь
  std::vector<std::tuple<int, double, std::string>> s_data(199 * 2);
  // погрешность
  std::vector<std::tuple<int, double, std::string>> e_data(199 * 2);

  int index_s = 0;
  int index_e = 0;
  for (int& i : n_arr) {
    double s_calc = MonteCarlo(i, circles, tight_ld, tight_ru);
    s_data[index_s++] = std::make_tuple(i, s_calc, "Узкий");
    e_data[index_e++] = std::make_tuple(i, PersistentError(s_true, s_calc), "Узкий");
  }
  for (int& i : n_arr) {
    double s_calc = MonteCarlo(i, circles, wide_ld, wide_ru);
    s_data[index_s++] = std::make_tuple(i, s_calc, "Широкий");
    e_data[index_e++] = std::make_tuple(i, PersistentError(s_true, s_calc), "Широкий");
  }

  // собираем для визуализации
  SaveSDtoCSV("area.csv", s_data);
  SaveEtoCSV("error.csv", e_data);

  return 0;
}
