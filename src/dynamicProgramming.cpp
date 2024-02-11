#include "algorithms.hpp"

#include <bitset>

void DynamicProgramming::run() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	const int matrixSize = matrix->size;
	int result = INT_MAX, tempResult = 0;
	// Reserve memory
	std::unordered_map<int, Cache> insideTempMap;
	cachedPathsNew.resize(matrixSize - 1, insideTempMap);
	for (auto& a : cachedPathsNew)
		a.reserve(pow(2, matrixSize - 2));
	std::vector<short> tempOrder;

	// Visit each vertex except for start one (0)
	for (int i = 1; i < matrixSize; i++) {
		tempResult = dynamicHelperFunction((1 << matrixSize) - 1 - (int)pow(2, i), i, &tempOrder);

		// If found result is better than current one, set as current best
		if (tempResult + matrix->mat[i][0] < result) {
			vertexOrder = tempOrder;
			result = tempResult + matrix->mat[i][0];
		}
	}

	// Result is in reverse - fix here
	std::reverse(vertexOrder.begin(), vertexOrder.end());
	vertexOrder.pop_back();
	pathLength = result;
	vertexOrder = vertexOrder;

	runningTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);

	cachedPathsNew.clear();
}

int DynamicProgramming::dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder) {
	// Check mask
	// If bit 0 and bit previousVertex is 1, we visited every other vertex
	if (maskCode == 1) {
		vertexOrder->push_back(0);
		vertexOrder->push_back(currentVertex);
		return matrix->mat[0][currentVertex];
	}

	// Check cache
	// If visited before, grab precalculated result
	std::unordered_map<int, Cache>::iterator cacheHit = cachedPathsNew[currentVertex - 1].find(maskCode);
	if (cacheHit != cachedPathsNew[currentVertex - 1].end()) {
		*vertexOrder = cacheHit->second.path;
		return cacheHit->second.pathLength;
	}

	// Manual calc
	// If no cache - divide into smaller problems
	int result = INT_MAX, tempResult;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>& toMatrix = (matrix->mat);
	std::vector<short> resultOrder, tempOrder;

	for (int i = 0; i < matrixSize; i++) {
		// maskCode detects loops, don't check start (0) and same vertex
		if (i != currentVertex && i != 0 && (maskCode & (1 << i))) {

			tempResult = dynamicHelperFunction(maskCode & (~(1 << i)), i, &tempOrder);
			if (tempResult + toMatrix[i][currentVertex] < result) {
				result = tempResult + toMatrix[i][currentVertex];
				resultOrder = tempOrder;
				resultOrder.push_back(currentVertex);
			}
		}
	}

	// If there isn't a smaller result, get result
	*vertexOrder = resultOrder;

	// Check if we save result to cache
	// Too long chains of vertices -> don't save into Cache
	std::bitset<32> countBit(maskCode);
	if (countBit.count() > matrixSize - 2) return result;

	cachedPathsNew[currentVertex - 1].insert({ maskCode, Cache(resultOrder, result) });

	return result;
}