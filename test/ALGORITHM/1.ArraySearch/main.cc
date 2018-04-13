#include <iostream>
#include <queue> /* BFS */
#include <stack> /* DFS */

static int MaxCount = 21;
static bool MyArray[499][499] = {0, };
static std::queue<std::pair<int, int>> queue; /* for BFS */
static std::stack<std::pair<int, int>> stack; /* for DFS */
static int ValidTotal = 0;

int sumEachDigit (int num) {
	if (num == 0)
		return 0;
	int sum = 0;
	int num_tmp = num;
	while (num_tmp > 0) {
		sum += num_tmp % 10;
		num_tmp = num_tmp / 10;
	}
	return sum;
}

int limitSize(int max) {
	int i = 0;
	for (; sumEachDigit(i) <= max; ++i) {
	}
	return i;
}

void CalculateValidCoordinateDFS (int x, int y, int max) {
	stack.push(std::make_pair(x,y));
	while (!stack.empty()) {
		int xpos = stack.top().first;
		int ypos = stack.top().second;
		stack.pop();
		bool ret = ((sumEachDigit(xpos) + sumEachDigit(ypos)) <= max);
		if (MyArray[xpos][ypos]) {
			continue;
		}
		MyArray[xpos][ypos] = ret;
		if (ret) {
			ValidTotal += 1;
			stack.push(std::make_pair(xpos+1,ypos));
			stack.push(std::make_pair(xpos,ypos+1));
			fprintf(stderr, "(%d , %d) : valid coordinate\n", xpos, ypos);
		}
		else
			fprintf(stderr, "(%d , %d) : invalid coordinate\n", xpos, ypos);
	}
}

void CalculateValidCoordinateBFS (int x, int y, int max) {
	queue.push(std::make_pair(x,y));
	while (!queue.empty()) {
		int xpos = queue.front().first;
		int ypos = queue.front().second;
		queue.pop();
		bool ret = ((sumEachDigit(xpos) + sumEachDigit(ypos)) <= max);
		if (MyArray[xpos][ypos]) {
			continue;
		}
		MyArray[xpos][ypos] = ret;
		if (ret) {
			ValidTotal += 1;
			queue.push(std::make_pair(xpos+1,ypos));
			queue.push(std::make_pair(xpos,ypos+1));
			fprintf(stderr, "(%d , %d) : valid coordinate\n", xpos, ypos);
		}
		else
			fprintf(stderr, "(%d , %d) : invalid coordinate\n", xpos, ypos);
	}
}

int main() {
	int x = 0, y = 0; /* start point */
	int max_test = 30;
	int limit = limitSize(max_test);
	CalculateValidCoordinateDFS(x,y,MaxCount);
	fprintf(stdout, "최종 count : %d\n", ValidTotal);
	fprintf(stdout, "보정 count : %d\n", ValidTotal * 4 - 498 * 4 - 3);
	fprintf(stdout, "max : %d, limit length : %d\n", max_test, limit);
	return 0;
}

