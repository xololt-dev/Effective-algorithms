#include "algorithms.hpp"

#include <numeric>

void Algorithm::displayResults() {
	std::cout << "Dlugosc sciezki: " << pathLength << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : this->vertexOrder) std::cout << a << " ";
	std::cout << "0\nCzas trwania algorytmu: " << runningTime.count() << "s\n";
}

int AdvancedAlgorithm::getPathDelta() {
	int lowest = INT_MAX, highest = 0;

	for (std::vector<std::vector<int>>::iterator i = matrix->mat.begin(); i != matrix->mat.end(); i++) {
		for (auto j = (*i).begin(); j != (*i).end(); j++) {
			if (*j < lowest)
				lowest = *j;
			else if (*j > highest)
				highest = *j;
		}
	}

	return highest - lowest;
}

std::tuple<int, int> AdvancedAlgorithm::generateRandomTwoPositions(int lowerBound, int higherBound, bool correctOrder) {
	// generate two positions
	std::uniform_int_distribution<> distribution(lowerBound, higherBound);

	int indexOne = distribution(gen), indexTwo = distribution(gen);

	while (indexOne == indexTwo)
		indexTwo = distribution(gen);

	if (!correctOrder)
		return std::make_tuple(indexOne, indexTwo);

	if (indexTwo < indexOne) {
		int temp = indexOne;
		indexOne = indexTwo;
		indexTwo = temp;
	}

	return std::make_tuple(indexOne, indexTwo);
}

std::tuple<std::vector<short>, int> AdvancedAlgorithm::generateInitialSolution() {
	// Greedy method
	std::vector<short> possibleVertices, returnVector;
	int matrixSize = matrix->size, returnLength = 0;
	returnVector.reserve(matrixSize);
	for (int i = 1; i < matrixSize; i++)
		possibleVertices.push_back(i);

	int currentVertex = 0;

	while (possibleVertices.size()) {
		int value = INT_MAX, lowestIndex = 0;
		// znajdz najkrotsza mozliwa sciezke
		for (int i = 1; i < matrixSize; i++) {
			if (matrix->mat[i][currentVertex] < value
				&& (std::find(possibleVertices.begin(), possibleVertices.end(), i) != std::end(possibleVertices))) {
				value = matrix->mat[i][currentVertex];
				lowestIndex = i;
			}
		}

		// dodaj do generowanego rozwiazania
		returnVector.push_back(lowestIndex);
		returnLength += value;

		// zmien "obecny" wierzcholek
		currentVertex = lowestIndex;

		// usun z mozliwych do wybrania wierzcholkow
		possibleVertices.erase(
			std::find(possibleVertices.begin(), possibleVertices.end(), lowestIndex)
		);
	}

	returnLength += matrix->mat[0][currentVertex];

	return std::make_tuple(returnVector, returnLength);
}

std::tuple<std::vector<short>, int> AdvancedAlgorithm::generateNewSolution(int notAllowedSecondary) {
	// Greedy method
	std::vector<short> possibleVertices, returnVector;
	int matrixSize = matrix->size, returnLength = INT_MAX;
	returnVector.reserve(matrixSize);
	for (int i = 1; i < matrixSize; i++)
		possibleVertices.push_back(i);

	std::uniform_int_distribution<> u(1, matrixSize - 1);

	// zmien "obecny" wierzcholek
	int currentVertex = notAllowedSecondary;
	do {
		currentVertex = u(rd);
	} while (currentVertex == notAllowedSecondary);
	returnVector.push_back(currentVertex);
	returnLength = matrix->mat[currentVertex][0];

	// usun z mozliwych do wybrania wierzcholkow
	possibleVertices.erase(
		std::find(possibleVertices.begin(), possibleVertices.end(), currentVertex)
	);

	while (possibleVertices.size()) {
		int value = INT_MAX, lowestIndex = 0;
		// znajdz najkrotsza mozliwa sciezke
		for (int i = 1; i < matrixSize; i++) {
			if (matrix->mat[i][currentVertex] < value
				&& (std::find(possibleVertices.begin(), possibleVertices.end(), i) != std::end(possibleVertices))) {
				value = matrix->mat[i][currentVertex];
				lowestIndex = i;
			}
		}

		// dodaj do generowanego rozwiazania
		returnVector.push_back(lowestIndex);
		returnLength += value;

		// zmien "obecny" wierzcholek
		currentVertex = lowestIndex;

		// usun z mozliwych do wybrania wierzcholkow
		possibleVertices.erase(
			std::find(possibleVertices.begin(), possibleVertices.end(), lowestIndex)
		);
	}

	returnLength += matrix->mat[0][currentVertex];

	return std::make_tuple(returnVector, returnLength);
}

std::vector<short> AdvancedAlgorithm::inverse(std::vector<short>* currentOrder, int firstPosition, int secondPosition) {
	std::vector<short> returnVector(*currentOrder);
	int vectorSize = currentOrder->size();

	std::tuple<int, int> t;
	// generate two positions
	if (!firstPosition && !secondPosition)
		t = generateRandomTwoPositions(0, vectorSize - 1);
	else t = std::make_tuple(firstPosition, secondPosition);

	std::vector<short>::iterator low = returnVector.begin(), high = returnVector.begin();
	std::advance(low, std::get<0>(t));
	std::advance(high, std::get<1>(t) + 1);
	std::reverse(low, high);

	return returnVector;
}

std::vector<short> AdvancedAlgorithm::swap(std::vector<short>* currentOrder, int firstPosition, int secondPosition) {
	std::vector<short> returnVector(*currentOrder);
	int vectorSize = currentOrder->size();

	std::tuple<int, int> t;
	// generate two positions
	if (!firstPosition && !secondPosition)
		t = generateRandomTwoPositions(0, vectorSize - 1);
	else t = std::make_tuple(firstPosition, secondPosition);

	// swap positions
	std::vector<short>::iterator low = returnVector.begin(), high = returnVector.begin();

	std::advance(low, std::get<0>(t));
	std::advance(high, std::get<1>(t));
	std::swap(low, high);

	return returnVector;
}

std::vector<short> AdvancedAlgorithm::insert(std::vector<short>* currentOrder, int firstPosition, int secondPosition) {
	std::vector<short> returnVector(*currentOrder);
	int vectorSize = currentOrder->size();

	std::tuple<int, int> t;
	// generate two positions
	if (!firstPosition && !secondPosition)
		t = generateRandomTwoPositions(0, vectorSize - 1, false);
	else t = std::make_tuple(firstPosition, secondPosition);

	// insert
	if (std::get<0>(t) > std::get<1>(t)) {
		std::rotate(returnVector.rend() - std::get<0>(t) - 1,
			returnVector.rend() - std::get<0>(t),
			returnVector.rend() - std::get<1>(t));
	}
	else {
		std::rotate(returnVector.begin() + std::get<0>(t),
			returnVector.begin() + std::get<0>(t) + 1,
			returnVector.begin() + std::get<1>(t) + 1);
	}

	return returnVector;
}

std::vector<short> AdvancedAlgorithm::insertSub(std::vector<short>* currentOrder) {
	std::vector<short> returnVector(*currentOrder);
	int vectorSize = currentOrder->size();
	std::vector<short>::iterator beginCurrent = currentOrder->begin();

	// generate two positions
	std::tuple<int, int> t = generateRandomTwoPositions(0, vectorSize - 1);
	std::uniform_int_distribution<> distribution(0, vectorSize - 1);
	int indexThree = distribution(gen);

	// insert group
	if (indexThree > std::get<1>(t)) {
		if (std::get<0>(t) == 0) {
			returnVector.insert(returnVector.begin(), beginCurrent + std::get<1>(t) + 1, beginCurrent + indexThree + 1);
			returnVector.insert(returnVector.begin() + (indexThree - std::get<1>(t)), beginCurrent + std::get<0>(t), beginCurrent + std::get<1>(t) + 1);
		}
		else {
			returnVector.insert(returnVector.begin(), beginCurrent, beginCurrent + std::get<0>(t));
			returnVector.insert(returnVector.end(), beginCurrent + std::get<1>(t) + 1, beginCurrent + indexThree + 1);
			returnVector.insert(returnVector.end(), beginCurrent + std::get<0>(t), beginCurrent + std::get<1>(t) + 1);
		}
	}
	else if (indexThree < std::get<0>(t)) {
		returnVector.insert(returnVector.begin(), beginCurrent, beginCurrent + indexThree);
		returnVector.insert(returnVector.end(), beginCurrent + std::get<0>(t), beginCurrent + std::get<1>(t) + 1);
		returnVector.insert(returnVector.end(), beginCurrent + indexThree, beginCurrent + std::get<0>(t));
		returnVector.insert(returnVector.end(), beginCurrent + std::get<1>(t) + 1, currentOrder->end());
	}
	else {
		returnVector.insert(returnVector.begin(), beginCurrent, beginCurrent + std::get<0>(t));
		returnVector.insert(returnVector.end(), beginCurrent + std::get<1>(t) + 1, beginCurrent + std::min(std::get<1>(t) + (std::get<1>(t) - std::get<0>(t)), (int)currentOrder->size()));
		returnVector.insert(returnVector.end(), beginCurrent + std::get<0>(t), beginCurrent + std::get<1>(t) + 1);
	}

	return returnVector;
}