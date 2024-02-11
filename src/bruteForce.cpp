#include "algorithms.hpp"

#include <numeric>
#include <thread>

void BruteForce::run() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	if (multithread) {
		std::vector<std::vector<short>> orders; // results dump
		std::vector<int> pathLengths; // results dump
		int matrixSize = matrix->size;

		// Create base vector
		std::vector<short> permutationVector(matrixSize - 1);
		std::iota(permutationVector.begin(), permutationVector.end(), 1);
		// permutationVector.reserve(matrixSize);
		// for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
		int previousVertex = 1;

		std::vector<std::thread*> vectorOfThreadsInFlight;
		vectorOfThreadsInFlight.reserve(matrixSize - 1);
		orders.resize(matrixSize - 1);
		pathLengths.resize(matrixSize - 1, INT_MAX);
		std::vector<std::vector<short>>::iterator ordersIterator = orders.begin();
		std::vector<int>::iterator pathLengthsIterator = pathLengths.begin();

		// Create threads for sets of permutations
		for (int i = 0; i < permutationVector.size() - 1; i++) {
			vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
				&*ordersIterator,
				&*pathLengthsIterator,
				permutationVector,
				permutationVector.front(),
				matrix
			)
			);

			ordersIterator++;
			pathLengthsIterator++;

			std::swap(permutationVector[0], permutationVector[i + 1]);
		}
		vectorOfThreadsInFlight.push_back(new std::thread(bruteHelperMultithread,
			&*ordersIterator,
			&*pathLengthsIterator,
			permutationVector,
			permutationVector.front(),
			matrix
		)
		);

		for (auto a : vectorOfThreadsInFlight) a->join();

		// Check to ensure threads all finish
		bool notFinished = 0;
		do {
			notFinished = 0;
			for (auto& a : pathLengths) if (a == INT_MAX) {
				notFinished = 1;
				break;
			}
		} while (notFinished);

		// Find shortest path
		int place = 0;
		int currentPlace = 0;
		int minimum = INT_MAX;
		for (auto& a : pathLengths) {
			if (a < minimum) {
				place = currentPlace;
				minimum = a;
			}
			currentPlace++;
		}

		pathLength = minimum;
		ordersIterator = orders.begin();
		std::advance(ordersIterator, place);
		vertexOrder = *ordersIterator;

		runningTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now);

		for (auto& a : vectorOfThreadsInFlight)
			delete a;
	}
	else {
		pathLength = bruteHelperFunction(&vertexOrder);
		runningTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);
	}
}

int BruteForce::bruteHelperFunction(std::vector<short>* orderQueue) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<short> permutationVector(matrixSize - 1);
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<short>::iterator permutationIterator;
	std::vector<int>::iterator innerIter = (*outerIter).begin();

	std::iota(permutationVector.begin(), permutationVector.end(), 1);
	/*
	permutationVector.reserve(matrixSize);

	for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
	*/
	
	// Analize till permutations present
	do {
		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = permutationVector.begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
			// First iter:
			// outerIter = first vertex permutationVector
			// inner = previousVertex (source -> 0)

			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);

			innerIter = (*outerIter).begin();
			std::advance(innerIter, previousVertex);
			currentPath += *innerIter;

			if (currentPath > shortestPath) goto ZA_DUZO;

			previousVertex = *permutationIterator;
		}

		outerIter = pointerMat->begin();
		innerIter = (*outerIter).begin();
		std::advance(innerIter, previousVertex);
		currentPath += *innerIter;

	ZA_DUZO:
		if (currentPath < shortestPath) {
			shortestPath = currentPath;
			*orderQueue = permutationVector;
		}
	} while (std::next_permutation(permutationVector.begin(), permutationVector.end()));

	return shortestPath;
}

void BruteForce::bruteHelperMultithread(std::vector<short>* orderQueue, int* pathLength, std::vector<short> permutation, int permutationNumber, Matrix* matrix) {
	int shortestPath = INT_MAX;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>* pointerMat = &(matrix->mat);
	std::vector<std::vector<int>>::iterator outerIter = pointerMat->begin();
	std::vector<short>::iterator permutationIterator;
	std::vector<int>::iterator innerIter = (*outerIter).begin();

	do {
		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = permutation.begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
			// First iter:
			// outerIter = first vertex permutationVector
			// inner = previousVertex (source -> 0)

			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);

			innerIter = (*outerIter).begin();
			std::advance(innerIter, previousVertex);
			currentPath += *innerIter;

			// End if current path already longer than current best
			if (currentPath > shortestPath) goto ZA_DUZO;

			previousVertex = *permutationIterator;
		}

		outerIter = pointerMat->begin();
		innerIter = (*outerIter).begin();
		std::advance(innerIter, previousVertex);
		currentPath += *innerIter;

	ZA_DUZO:
		if (currentPath < shortestPath) {
			shortestPath = currentPath;
			*orderQueue = permutation;
		}
	} while (std::next_permutation(permutation.begin(), permutation.end()) && (permutation.front() == permutationNumber));

	*pathLength = shortestPath;
}