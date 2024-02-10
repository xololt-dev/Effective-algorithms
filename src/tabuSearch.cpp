#include "util.hpp"
/*
void Menu::tabuSearch(Matrix* matrix) {
	// greedy path generator
	std::tuple<std::vector<short>, int> t = generateInitialSolution(matrix);
	int secondVertex = std::get<0>(t)[0];

	tabu.clear();

	std::cout << "\nDlugosc sciezki zachlannej: " << std::get<1>(t) << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : std::get<0>(t)) std::cout << a << " ";
	std::cout << "0\n";

	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

	std::vector<short> currentSolutionOrder = std::get<0>(t), randomCandidateOrder, bestSolutionOrder = std::get<0>(t);
	int currentSolutionLength = std::get<1>(t), bestSolutionLength = std::get<1>(t);
	int tabuResize = 0, notImprovedStreak = 0, maxAllowedNonImprovement = getPathDelta(matrix) * matrix->size;

	QueueData qData;
	for (int i = matrix->size - 2; i > 0; i--)
		tabuResize += i;
	tabu.resize(tabuResize, 0);

	std::cout << "\nTabu size: " << tabuResize << "\n";
	std::cout << "Neighbourhood: " << currentNeighbourhoodType << "\n";
	std::cout << "Max non improvement streak allowed: " << maxAllowedNonImprovement << "\n";

	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start) < maxExecutionTime) {
		// Generate candidate list
		// Choose best candidate
		qData = generateBestMove(&currentSolutionOrder, bestSolutionLength, &(matrix->mat));

		currentSolutionOrder = qData.pathOrder;
		currentSolutionLength = qData.pathLength;
		if (qData.pathLength < bestSolutionLength) {
			bestSolutionOrder = qData.pathOrder;
			bestSolutionLength = qData.pathLength;
			notImprovedStreak = 0;

			runningTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
#ifdef _DEBUG 
			std::cout << "\nDlugosc najlepszej sciezki: " << bestSolutionLength << "\n";
			std::cout << "Kolejnosc wierzcholkow:\n0 ";
			for (auto a : bestSolutionOrder) std::cout << a << " ";
			std::cout << "0\n";
#endif
		}
		else notImprovedStreak++;

		if (notImprovedStreak > maxAllowedNonImprovement) {
			t = generateNewSolution(matrix, secondVertex);
			currentSolutionOrder = std::get<0>(t);
			currentSolutionLength = std::get<1>(t);
			secondVertex = std::get<0>(t)[0];
			notImprovedStreak = 0;
			improvConstant *= 0.99f;
		}

		updateTabu(qData.anchorOne, qData.anchorTwo, matrix->mat[0].size());
	}

	pathLength = bestSolutionLength;
	vertexOrder = bestSolutionOrder;
}

void Menu::tabuSearchBenchmark(Matrix* matrix, int iteration) {
	// greedy path generator
	std::tuple<std::vector<short>, int> t = generateInitialSolution(matrix);
	int secondVertex = std::get<0>(t)[0];

	tabu.clear();
	runningTime = std::chrono::seconds(0);

	std::cout << "\nDlugosc sciezki zachlannej: " << std::get<1>(t) << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : std::get<0>(t)) std::cout << a << " ";
	std::cout << "0\n";

	std::fstream file;
	file.open(benchmarkFile, std::fstream::app | std::fstream::out);
	if (file.good()) {
		file << currentNeighbourhoodType << ";" << iteration << ";" << std::chrono::duration_cast<std::chrono::microseconds>(runningTime).count() << ";" << std::get<1>(t) << ";0-";
		for (auto a : std::get<0>(t)) file << a << "-";
		file << "0\n";

		std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

		std::vector<short> currentSolutionOrder = std::get<0>(t), randomCandidateOrder, bestSolutionOrder = std::get<0>(t);
		int currentSolutionLength = std::get<1>(t), bestSolutionLength = std::get<1>(t);
		int tabuResize = 0, notImprovedStreak = 0, maxAllowedNonImprovement = getPathDelta(matrix) * matrix->size;

		QueueData qData;
		for (int i = matrix->size - 2; i > 0; i--)
			tabuResize += i;
		tabu.resize(tabuResize, 0);

		while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start) < maxExecutionTime) {
			// Generate candidate list
			// Choose best candidate
			qData = generateBestMove(&currentSolutionOrder, bestSolutionLength, &(matrix->mat));

			currentSolutionOrder = qData.pathOrder;
			currentSolutionLength = qData.pathLength;
			if (qData.pathLength < bestSolutionLength) {
				bestSolutionOrder = qData.pathOrder;
				bestSolutionLength = qData.pathLength;
				notImprovedStreak = 0;

				runningTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);

				file << currentNeighbourhoodType << ";" << iteration << ";" << std::chrono::duration_cast<std::chrono::microseconds>(runningTime).count() << ";" << bestSolutionLength << ";0-";
				for (auto a : bestSolutionOrder) file << a << "-";
				file << "0\n";
#ifdef _DEBUG 
				std::cout << "\nDlugosc najlepszej sciezki: " << bestSolutionLength << "\n";
				std::cout << "Kolejnosc wierzcholkow:\n0 ";
				for (auto a : bestSolutionOrder) std::cout << a << " ";
				std::cout << "0\n";
#endif
			}
			else notImprovedStreak++;

			if (notImprovedStreak > maxAllowedNonImprovement) {
				t = generateNewSolution(matrix, secondVertex);
				currentSolutionOrder = std::get<0>(t);
				currentSolutionLength = std::get<1>(t);
				secondVertex = std::get<0>(t)[0];
				notImprovedStreak = 0;
			}

			updateTabu(qData.anchorOne, qData.anchorTwo, matrix->mat[0].size());
		}

		pathLength = bestSolutionLength;
		vertexOrder = bestSolutionOrder;

		file.close();
	}
}

QueueData Menu::generateBestMove(std::vector<short>* currentOrder, int bestLength, std::vector<std::vector<int>>* matrix) {
	// Priority queue	
	auto compare = [](QueueData x, QueueData y)
		{return x.pathLength > y.pathLength; };

	std::priority_queue<
		QueueData,
		std::vector<QueueData>,
		decltype(compare)> queue(compare);

	int currentOrderSize = currentOrder->size();

	QueueData tempData;
	// Pack every neighbour possibility into a queue

	for (int i = 0; i < currentOrderSize; i++) {
		for (int j = i + 1; j < currentOrderSize; j++) {
			tempData = getNewOrder(currentOrder, i, j, matrix);
			queue.push(tempData);
		}
	}

	// Check best candidate till found the desired one
		// Is it in tabu?
		// If yes check aspiration and decide if better
		// Not in tabu -> grab it
		// Decide whenever to keep looking

	while (queue.size()) {
		tempData = queue.top();

		if (tempData.anchorOne > tempData.anchorTwo) {
			int temp = tempData.anchorOne;
			tempData.anchorOne = tempData.anchorTwo;
			tempData.anchorTwo = temp;
		}
		// 	(size - x)(x - 1) + (y - x - 1)
		int placeInTabu = ((int)(matrix->size()) - tempData.anchorOne + 1) * (tempData.anchorOne - 1) + (tempData.anchorTwo - tempData.anchorOne - 1);

		if (!tabu[placeInTabu])
			return tempData;

		// check aspir
		// if best solution so far
		if (tempData.pathLength * improvConstant < float(bestLength))
			return tempData;

		queue.pop();
	}

	return tempData;
}

QueueData Menu::getNewOrder(std::vector<short>* currentOrder, int anchorOne, int anchorTwo, std::vector<std::vector<int>>* matrix) {
	QueueData tempData;
	short previousVertex = 0;

	switch (currentNeighbourhoodType)
	{
	case INVERSE:
		tempData.pathOrder = inverse(currentOrder, anchorOne, anchorTwo);
		break;
	case SWAP:
		tempData.pathOrder = swap(currentOrder, anchorOne, anchorTwo);
		break;
	case INSERT:
		tempData.pathOrder = insert(currentOrder, anchorOne, anchorTwo);
		break;
	default:
		break;
	}

	tempData.pathLength = 0;

	for (int k = 0; k < tempData.pathOrder.size(); k++) {
		tempData.pathLength += (*matrix)[tempData.pathOrder[k]][previousVertex];
		previousVertex = tempData.pathOrder[k];
	}

	tempData.pathLength += (*matrix)[0][previousVertex];
	tempData.anchorOne = tempData.pathOrder[anchorOne];
	tempData.anchorTwo = tempData.pathOrder[anchorTwo];

	return tempData;
}

void Menu::updateTabu(int anchorOne, int anchorTwo, int matrixSize) {
	std::vector<short>::iterator iter = tabu.begin();

	while (iter != tabu.end()) {
		if (*iter > 0)
			(*iter)--;

		iter++;
	}

	int placeInTabu = (matrixSize - anchorOne + 1) * (anchorOne - 1) + (anchorTwo - anchorOne - 1);
	tabu[placeInTabu] = 2 * sqrt(matrixSize);
}

*/