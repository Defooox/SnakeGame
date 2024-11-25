#ifndef AI_H
#define AI_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

enum class Direction { Up, Down, Left, Right };

struct Point {
  int x, y;
  Point(int x, int y) : x(x), y(y) {}

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Point& other) const { return !(*this == other); }
};

struct PointHash {
  std::size_t operator()(const Point& p) const {
    return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
  }
};

class AI {
 public:
  AI(int width, int height) : width(width), height(height) {}

  Direction getNextDirection(const std::vector<Point>& snakeBody,
                             const Point& apple);

 private:
  int width, height;

  bool isValid(const Point& p,
               const std::unordered_set<Point, PointHash>& obstacles);
  std::vector<Direction> bfs(
      const Point& start, const Point& target,
      const std::unordered_set<Point, PointHash>& obstacles);
};

#endif  // AI_H