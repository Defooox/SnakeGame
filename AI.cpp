#include "AI.h"

Direction AI::getNextDirection(const std::vector<Point>& snakeBody,
	const Point& apple) {
	std::unordered_set<Point, PointHash> obstacles;
	for (const auto& segment : snakeBody) {
		obstacles.insert(segment);
	}

	Point start(snakeBody[0].x, snakeBody[0].y);
	Point target(apple.x, apple.y);

	std::vector<Direction> path = bfs(start, target, obstacles);

	if (!path.empty()) {
		return path[0];
	}

	// Если путь не найден, просто продолжаем двигаться в текущем направлении
	return Direction::Right;  // Здесь можно выбрать любое направление по
	// умолчанию
}



bool AI::isValid(const Point& p,
	const std::unordered_set<Point, PointHash>& obstacles) {
	return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height &&
		obstacles.find(p) == obstacles.end();
}

std::vector<Direction> AI::bfs(
	const Point& start, const Point& target,
	const std::unordered_set<Point, PointHash>& obstacles) {
	std::queue<Point> queue;
	std::unordered_map<Point, Direction, PointHash> cameFrom;
	std::unordered_set<Point, PointHash> visited;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		Point current = queue.front();
		queue.pop();

		if (current == target) {
			std::vector<Direction> path;
			while (current != start) {
				Direction dir = cameFrom[current];
				path.push_back(dir);
				switch (dir) {
				case Direction::Up:
					current.y++;
					break;
				case Direction::Down:
					current.y--;
					break;
				case Direction::Left:
					current.x++;
					break;
				case Direction::Right:
					current.x--;
					break;
				}
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		std::vector<std::pair<Point, Direction>> neighbors = {
			{Point(current.x, current.y - 1), Direction::Up},
			{Point(current.x, current.y + 1), Direction::Down},
			{Point(current.x - 1, current.y), Direction::Left},
			{Point(current.x + 1, current.y), Direction::Right} };

		for (const auto& neighbor : neighbors) {
			if (isValid(neighbor.first, obstacles) &&
				visited.find(neighbor.first) == visited.end()) {
				queue.push(neighbor.first);
				visited.insert(neighbor.first);
				cameFrom[neighbor.first] = neighbor.second;
			}
		}
	}

	return {};
}