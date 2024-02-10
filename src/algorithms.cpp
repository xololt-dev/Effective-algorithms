#include "algorithms.hpp"

#include <thread>
#include <bitset>
#include <numeric>

void Algorithm::displayResults() {
	std::cout << "Dlugosc sciezki: " << pathLength << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : this->vertexOrder) std::cout << a << " ";
	std::cout << "0\nCzas trwania algorytmu: " << runningTime.count() << "s\n";
}

void BruteForce::run() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	if (multithread) {
		std::vector<std::vector<short>> orders; // results dump
		std::vector<int> pathLengths; // results dump
		int matrixSize = matrix->size;

		std::vector<short> permutationVector(matrixSize - 1);
		std::iota(permutationVector.begin(), permutationVector.end(), 1);
		// permutationVector.reserve(matrixSize);
		// for (int i = 1; i < matrixSize; i++) permutationVector.push_back(i);
		int previousVertex = 1;

		// mo¿na stworzyæ wartoœci na ka¿dy w¹tek bez problemu 

		std::vector<std::thread*> vectorOfThreadsInFlight;
		vectorOfThreadsInFlight.reserve(matrixSize - 1);
		orders.resize(matrixSize - 1);
		pathLengths.resize(matrixSize - 1, INT_MAX);
		std::vector<std::vector<short>>::iterator ordersIterator = orders.begin();
		std::vector<int>::iterator pathLengthsIterator = pathLengths.begin();

		// tworzenie w¹tków dla zestawów permutacji
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

		// sprawdzenie czy wszystkie w¹tki siê wykona³y
		bool notFinished = 0;
		do {
			notFinished = 0;
			for (auto& a : pathLengths) if (a == INT_MAX) {
				notFinished = 1;
				break;
			}
		} while (notFinished);

		// ustalenie najkrótszej drogi
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
	// póki s¹ permutacje, analizujemy (do while, aby pierwsza permutacja by³a niezmieniona)
	do {
		int previousVertex = 0;
		int currentPath = 0;
		int currentVertexNumber = 0;

		for (permutationIterator = permutationVector.begin(); currentVertexNumber < matrixSize - 1; permutationIterator++, currentVertexNumber++) {
			// np pierwsza iteracja(od zrodla)
			// outerIter = pierwszy wierzcholek permutationVector
			// inner = previousVertex (czyli zrodlo, czyli 0)

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
			/* np pierwsza iteracja(od zrodla)
			* outerIter = pierwszy wierzcholek permutationVector
			* inner = previousVertex (czyli zrodlo, czyli 0)
			*/

			outerIter = pointerMat->begin();
			std::advance(outerIter, *permutationIterator);

			innerIter = (*outerIter).begin();
			std::advance(innerIter, previousVertex);
			currentPath += *innerIter;

			// jezeli liczona sciezka jest dluzsza niz obecnie najkrotsza, wczesniej konczymy
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

	// zamiast return
	*pathLength = shortestPath;
}

void DynamicProgramming::run() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	const int matrixSize = matrix->size;
	int result = INT_MAX, tempResult = 0;
	// rezerwacja wymaganej iloœci pamiêci
	std::unordered_map<int, Cache> insideTempMap;
	cachedPathsNew.resize(matrixSize - 1, insideTempMap);
	for (auto& a : cachedPathsNew)
		a.reserve(pow(2, matrixSize - 2));
	std::vector<short> tempOrder;

	// odwiedz kazdy wierzcholek oprócz startu (0)
	for (int i = 1; i < matrixSize; i++) {
		tempResult = dynamicHelperFunction((1 << matrixSize) - 1 - (int)pow(2, i), i, &tempOrder);

		// jeœli rezultat znaleziony przez helper function jest mniejszy od obecnego najmniejszego, ustaw tempResult jako obecne rozwi¹zanie
		if (tempResult + matrix->mat[i][0] < result) {
			vertexOrder = tempOrder;
			result = tempResult + matrix->mat[i][0];
		}
	}

	// kolejnoœæ otrzymywana z helper function jest w odwrotnej kolejnoœci
	std::reverse(vertexOrder.begin(), vertexOrder.end());
	vertexOrder.pop_back();
	pathLength = result;
	vertexOrder = vertexOrder;

	runningTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now);

	cachedPathsNew.clear();
}

int DynamicProgramming::dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder) {
	// sprawdzenie maski
	// jesli bit 0 i bit previousVertex jest zaznaczony, to odwiedzilismy wszystko inne
	if (maskCode == 1) {
		vertexOrder->push_back(0);
		vertexOrder->push_back(currentVertex);
		return matrix->mat[0][currentVertex];
	}

	// sprawdzenie cache
	// jesli bylo poprzednio obliczone, wez gotowy wynik
	std::unordered_map<int, Cache>::iterator cacheHit = cachedPathsNew[currentVertex - 1].find(maskCode);
	if (cacheHit != cachedPathsNew[currentVertex - 1].end()) {
		*vertexOrder = cacheHit->second.path;
		return cacheHit->second.pathLength;
	}

	// obliczanie "reczne"
	// jesli brak cache, przejdz do mniejszych problemów
	int result = INT_MAX, tempResult;
	const int matrixSize = matrix->size;
	std::vector<std::vector<int>>& toMatrix = (matrix->mat);
	std::vector<short> resultOrder, tempOrder;

	for (int i = 0; i < matrixSize; i++) {
		// nie odnosimy siê do tego samego vertexu lub
		// nie odnosimy siê do startu 
		// plus maskCode wykrywa "petle"
		if (i != currentVertex && i != 0 && (maskCode & (1 << i))) {

			tempResult = dynamicHelperFunction(maskCode & (~(1 << i)), i, &tempOrder);
			if (tempResult + toMatrix[i][currentVertex] < result) {
				result = tempResult + toMatrix[i][currentVertex];
				resultOrder = tempOrder;
				resultOrder.push_back(currentVertex);
			}
		}
	}
	// jak nie ma mniejszego problemu, daj wynik

	*vertexOrder = resultOrder;
	// sprawdzenie dodatkowe czy zapisaæ do cache, 
	// jeœli za d³ugi ci¹g (nie do wykorzystania), nie wpisujemy do Cache
	std::bitset<32> countBit(maskCode);
	if (countBit.count() > matrixSize - 2) return result;

	cachedPathsNew[currentVertex - 1].insert({ maskCode, Cache(resultOrder, result) });

	return result;
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

void TabuSearch::run() {
	// greedy path generator
	std::tuple<std::vector<short>, int> t = generateInitialSolution();
	int secondVertex = std::get<0>(t)[0];

	tabu.clear();

	std::cout << "\nDlugosc sciezki zachlannej: " << std::get<1>(t) << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : std::get<0>(t)) std::cout << a << " ";
	std::cout << "0\n";

	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

	std::vector<short> currentSolutionOrder = std::get<0>(t), randomCandidateOrder, bestSolutionOrder = std::get<0>(t);
	int currentSolutionLength = std::get<1>(t), bestSolutionLength = std::get<1>(t);
	int tabuResize = 0, notImprovedStreak = 0, maxAllowedNonImprovement = getPathDelta() * matrix->size;

	QueueData qData;
	for (int i = matrix->size - 2; i > 0; i--)
		tabuResize += i;
	tabu.resize(tabuResize, 0);

	std::cout << "\nTabu size: " << tabuResize << "\n";
	// std::cout << "Neighbourhood: " << currentNeighbourhoodType << "\n";
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
			t = generateNewSolution(secondVertex);
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

QueueData TabuSearch::generateBestMove(std::vector<short>* currentOrder, int bestLength, std::vector<std::vector<int>>* matrix) {
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

QueueData TabuSearch::getNewOrder(std::vector<short>* currentOrder, int anchorOne, int anchorTwo, std::vector<std::vector<int>>* matrix) {
	QueueData tempData;
	short previousVertex = 0;

	switch (currentNeighbourhoodType)
	{
	case NeighbourhoodType::INVERSE:
		tempData.pathOrder = inverse(currentOrder, anchorOne, anchorTwo);
		break;
	case NeighbourhoodType::SWAP:
		tempData.pathOrder = swap(currentOrder, anchorOne, anchorTwo);
		break;
	case NeighbourhoodType::INSERT:
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

void TabuSearch::updateTabu(int anchorOne, int anchorTwo, int matrixSize) {
	std::vector<short>::iterator iter = tabu.begin();

	while (iter != tabu.end()) {
		if (*iter > 0)
			(*iter)--;

		iter++;
	}

	int placeInTabu = (matrixSize - anchorOne + 1) * (anchorOne - 1) + (anchorTwo - anchorOne - 1);
	tabu[placeInTabu] = 2 * sqrt(matrixSize);
}

void SimulatedAnnealing::run() {
	// greedy path generator
	std::tuple<std::vector<short>, int> t = generateInitialSolution();
	int secondVertex = std::get<0>(t)[0];

	std::cout << "\nDlugosc sciezki zachlannej: " << std::get<1>(t) << "\n";
	std::cout << "Kolejnosc wierzcholkow:\n0 ";
	for (auto a : std::get<0>(t)) std::cout << a << " ";
	std::cout << "0\n";

	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

	/*
	* pathDelta:
	* - low value
	*	- not much difference path to path
	*	- suggestion: set high discovery, low-ish local search
	* - high value
	*	- big difference in total path lengths
	*	- suggestion: set low discovery, high local search
	* tempEra:
	* - low value
	*	- checks neighbourhoods for a short time
	*	- higher discoverity
	* - high value
	*	- checks neighbourhoods for a long time
	*	- higher local search
	* maxAllowedNonImprovement:
	* - low value
	*	- re-generates current solution after short time
	*	- higher discoverity
	* - high value
	*	- re-generates current solution after long time
	*	- higher local search
	* currentTemp:
	* - also "temperature zero" at the beginning
	* - low value
	*	- rarely takes worse solution
	*	- higher local search
	* - high value
	*	- frequently takes worse solution
	*	- higher discoverity
	*/
	int pathDelta = getPathDelta();
	int tempEra = eraLength, maxAllowedNonImprovement = maxNonImproved, notImprovedStreak = 0;
	double currentTemp = startingTemp;

	if (currentTemp == 0.0f)
		currentTemp = std::get<1>(t) / log(1 + coolingConstant) / (double)pathDelta;
	if (eraLength <= 0)
		tempEra = std::min(pathDelta, matrix->size - 2);
	if (maxNonImproved <= 0)
		maxAllowedNonImprovement = 2000000;

	std::cout << "\nT_0: " << currentTemp << "\n";
	std::cout << "Cooling coefficient: " << coolingConstant << "\n";
	std::cout << "Path delta: " << pathDelta << "\n";
	std::cout << "Era length: " << tempEra << "\n";
	std::cout << "Max non improvement streak allowed: " << maxAllowedNonImprovement << "\n";

	std::vector<short> currentSolutionOrder = std::get<0>(t), randomCandidateOrder, bestSolutionOrder = std::get<0>(t);
	int currentSolutionLength = std::get<1>(t), bestSolutionLength = std::get<1>(t);

	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start) < maxExecutionTime) {
		// eraLength = max time per set temperature
		int eraLength = 0;

		while (eraLength < tempEra) {
			randomCandidateOrder = generateRandomCandidate(&currentSolutionOrder);

			int candidatePath = calculateCandidate(&randomCandidateOrder);

			if (changeSolutions(candidatePath, currentSolutionLength, currentTemp)) {
				if (candidatePath < bestSolutionLength) {
					bestSolutionLength = candidatePath;
					bestSolutionOrder = randomCandidateOrder;
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

				currentSolutionOrder = randomCandidateOrder;
				currentSolutionLength = candidatePath;
			}
			else notImprovedStreak++;

			if (notImprovedStreak > maxAllowedNonImprovement) {
				t = generateNewSolution(secondVertex);
				currentSolutionOrder = std::get<0>(t);
				currentSolutionLength = std::get<1>(t);
				secondVertex = std::get<0>(t)[0];
				notImprovedStreak = 0;
				break;
			}

			eraLength++;
		}

		// decrease temp
		currentTemp *= coolingConstant;
	}

	pathLength = bestSolutionLength;
	vertexOrder = bestSolutionOrder;
}

std::vector<short> SimulatedAnnealing::generateRandomCandidate(std::vector<short>* currentOrder) {
	std::vector<short> returnVector;

	switch (currentNeighbourhoodType)
	{
	case NeighbourhoodType::INVERSE:
		returnVector = inverse(currentOrder);
		break;
	case NeighbourhoodType::SWAP:
		returnVector = swap(currentOrder);
		break;
	case NeighbourhoodType::INSERT:
		returnVector = insert(currentOrder);
		break;
	case NeighbourhoodType::INSERT_SUB:
		returnVector = insertSub(currentOrder);
		break;
	default:
		break;
	}

	return returnVector;
}

bool SimulatedAnnealing::changeSolutions(int candidatePath, int currentPath, double currentTemp) {
	if (candidatePath <= currentPath)
		return true;

	double p = std::exp(-((double)(candidatePath - currentPath) / currentTemp));
	std::uniform_real_distribution<> distribution(0.0, 1.0);
	double r = distribution(gen);

	if (r <= p)
		return true;

	return false;
}

int SimulatedAnnealing::calculateCandidate(std::vector<short>* candidateOrder) {
	int pathLength = 0, previousVector = 0;
	std::vector<std::vector<int>>* matrixStart = &(matrix->mat);
	std::vector<short>::iterator ending = candidateOrder->end(), beginning = candidateOrder->begin();

	for (auto iter = beginning; iter != ending; iter++) {
		pathLength += (*matrixStart)[*iter][previousVector];
		previousVector = *iter;
	}

	pathLength += (*matrixStart)[0][previousVector];

	return pathLength;
}

void GeneticAlgorithm::run() {
	if (currentCrossoverType)
		geneticEAX();
	else
		geneticOX();
}

void GeneticAlgorithm::geneticOX() {
	int iterations = 0;
	// Priority queue func
	struct {
		bool operator()(const PathData x, const PathData y) const
		{
			return x.pathLength < y.pathLength;
		};
	} compareS;

	pathLength = INT_MAX;

	// Starting population generation
	std::vector<PathData> population = generateStartingPopulation();
	std::sort(population.begin(), population.end(), compareS);

	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	std::vector<double> lowerBounds;
	lowerBounds.reserve(population.size());
	std::vector<std::tuple<int, int>> parents;
	parents.reserve(population.size());
	std::vector<PathData> childrenData;
	childrenData.reserve(population.size());

	while (std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - start) < maxExecutionTime) {

		// Update best solution if applicable
		if (population[0].pathLength < pathLength) {
			pathLength = population[0].pathLength;
			vertexOrder = population[0].pathOrder;
			runningTime = std::chrono::duration_cast<std::chrono::microseconds>
				(std::chrono::steady_clock::now() - start);

			for (short& s : population[0].pathOrder) {
				std::cout << s << " ";
			}
			std::cout << "\n";
		}

		// Get lower bounds
		lowerBounds = getVertexLowerBounds(population.size());

		// Generate pairs
		parents = generateParents(&lowerBounds, population.size());

		// Genetic operations
		std::uniform_real_distribution<> distribution(0.0, 1.0);
		for (std::tuple<int, int>& t : parents) {
			if (distribution(gen) > crossoverConstant)
				continue;

			// Crossover
			PathData child = generateChildOX(&population[std::get<0>(t)].pathOrder,
				&population[std::get<1>(t)].pathOrder);

			child.pathLength = calculateCandidate(&child.pathOrder);
			// Mutate
			if (distribution(gen) <= mutationConstant) {
				if (child.pathLength < pathLength)
					childrenData.push_back(PathData(child));

				std::tuple<int, int> iT = generateRandomTwoPositions(0, matrix->size - 2);
				child = getNewOrder(&child.pathOrder, std::get<0>(iT),
					std::get<1>(iT), &(matrix->mat));
			}

			childrenData.emplace_back(std::move(child));
		}

		// Combine to get new generation
		population = getNewRandomGeneration(&population, &childrenData);

		std::sort(population.begin(), population.end(), compareS);

		iterations++;
		lowerBounds.clear();
		parents.clear();
		childrenData.clear();
	}

	std::cout << iterations << "\n";
}

void GeneticAlgorithm::geneticEAX() {
	int iterations = 0;
	// Priority queue func
	struct {
		bool operator()(const PathData x, const PathData y) const
		{
			return x.pathLength < y.pathLength;
		};
	} compareS;

	pathLength = INT_MAX;

	// Starting population generation
	std::vector<PathData> population = generateStartingPopulation();

	std::sort(population.begin(), population.end(), compareS);

	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	std::vector<double> lowerBounds;
	lowerBounds.reserve(population.size());
	std::vector<std::tuple<int, int>> parents;
	parents.reserve(population.size());
	std::vector<PathData> childrenData;
	childrenData.reserve(population.size());

	while (std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - start) < maxExecutionTime) {

		// Update best solution if applicable
		if (population[0].pathLength < pathLength) {
			pathLength = population[0].pathLength;
			vertexOrder = population[0].pathOrder;
			runningTime = std::chrono::duration_cast<std::chrono::microseconds>
				(std::chrono::steady_clock::now() - start);

			for (short& s : population[0].pathOrder) {
				std::cout << s << " ";
			}
			std::cout << "\n";
		}

		// Get lower bounds
		lowerBounds = getVertexLowerBounds(population.size());

		// Generate pairs
		parents = generateParents(&lowerBounds, population.size());

		// Generate children

		// Genetic operations
		std::uniform_real_distribution<> distribution(0.0, 1.0);
		total = 0,
			repeat = 0;
		for (std::tuple<int, int>& t : parents) {
			if (distribution(gen) > crossoverConstant)
				continue;
			total++;
			// Crossover
			PathData child = generateChildEAX(&population[std::get<0>(t)].pathOrder,
				&population[std::get<1>(t)].pathOrder);

			// Mutate
			if (distribution(gen) <= mutationConstant) {
				std::tuple<int, int> iT = generateRandomTwoPositions(0, matrix->size - 2);
				child = getNewOrder(&child.pathOrder, std::get<0>(iT),
					std::get<1>(iT), &(matrix->mat));
			}
			else child.pathLength = calculateCandidate(&child.pathOrder);

			childrenData.emplace_back(std::move(child));
		}

		// Combine to get new generation
		population = getNewRandomGeneration(&population, &childrenData);

		std::sort(population.begin(), population.end(), compareS);
		iterations++;
		lowerBounds.clear();
		parents.clear();
		childrenData.clear();
	}
	std::cout << iterations << "\n";
}

std::vector<PathData> GeneticAlgorithm::generateStartingPopulation() {
	std::vector<PathData> returnVec;
	returnVec.reserve(startingPopulationSize);
	std::vector<short> leftVertices(matrix->size - 1);
	std::iota(leftVertices.begin(), leftVertices.end(), 1);

	if (startingPopulationRandom == true) {
		generateRandomStartingPopulation(&leftVertices, &returnVec);
	}
	else {
		generateGreedyStartingPopulation(&leftVertices, &returnVec);

		if (startingPopulationSize >= matrix->size) {
			std::vector<short> leftVertices(matrix->size - 1);
			std::iota(leftVertices.begin(), leftVertices.end(), 1);

			generateRandomStartingPopulation(&leftVertices, &returnVec);
		}
	}

	return returnVec;
}

void GeneticAlgorithm::generateRandomStartingPopulation(std::vector<short>* leftVertices, std::vector<PathData>* returnVec) {
	while (returnVec->size() < startingPopulationSize) {
		PathData tempData;
		std::shuffle(leftVertices->begin(), leftVertices->end(), gen);

		short previousVertex = 0;
		tempData.pathLength = 0;
		tempData.pathOrder = *leftVertices;

		for (int k = 0; k < tempData.pathOrder.size(); k++) {
			tempData.pathLength += matrix->mat[tempData.pathOrder[k]][previousVertex];
			previousVertex = tempData.pathOrder[k];
		}

		tempData.pathLength += matrix->mat[0][previousVertex];

		returnVec->emplace_back(std::move(tempData));
	}
}

void GeneticAlgorithm::generateGreedyStartingPopulation(std::vector<short>* leftVertices, std::vector<PathData>* returnVec) {
	int cap = 0;
	PathData tempData;

	std::tuple<std::vector<short>, int> t = generateInitialSolution();
	short firstOne = std::get<0>(t)[0];
	leftVertices->erase(
		std::find(leftVertices->begin(), leftVertices->end(), firstOne)
	);

	tempData.pathOrder = std::get<0>(t);
	tempData.pathLength = std::get<1>(t);
	returnVec->emplace_back(std::move(tempData));

	if (startingPopulationSize < matrix->size)
		cap = startingPopulationSize;
	else cap = matrix->size - 1;

	for (int i = 1; i < cap; i++) {
		PathData tempDataTwo;

		std::shuffle(leftVertices->begin(), leftVertices->end(), gen);

		t = generateNewSolutionV((*leftVertices)[0]);

		firstOne = std::get<0>(t)[0];
		leftVertices->erase(
			std::find(leftVertices->begin(), leftVertices->end(), firstOne)
		);

		tempDataTwo.pathOrder = std::get<0>(t);
		tempDataTwo.pathLength = std::get<1>(t);
		returnVec->emplace_back(std::move(tempDataTwo));
	}
}

std::vector<double> GeneticAlgorithm::getVertexLowerBounds(int vectorSize) {
	std::vector<double> vertexLowerBound(vectorSize, 0.0);

	for (int i = 1; i < vertexLowerBound.size(); i++) {
		double current = (1.0 - (double)i / (double)vectorSize);
		vertexLowerBound[i] = current + vertexLowerBound[i - 1];
	}

	double sum = vertexLowerBound[vertexLowerBound.size() - 1];

	for (int i = 1; i < vertexLowerBound.size(); i++) {
		vertexLowerBound[i] /= sum;
	}

	return vertexLowerBound;
}

std::vector<std::tuple<int, int>> GeneticAlgorithm::generateParents(std::vector<double>* boundsVector, int vectorSize) {
	std::vector<std::tuple<int, int>> pairsVector;
	pairsVector.reserve(boundsVector->size());
	// Maybe saved "wrong" generations to a vector for later usage?
	std::vector<int> generatedRandoms;
	generatedRandoms.reserve(4);

	// Generate number(s) [0, 1]
	std::uniform_real_distribution<> distribution(0.0, 1.0);
	int childrenGenerated = 0;

	while (childrenGenerated < vectorSize) {
		double generated;
		int firstCandidate = -1;
		int secondCandidate = 0;
		std::vector<double>::iterator boundIter = boundsVector->begin();

		// Check if we generated something we can take
		if (!generatedRandoms.empty()) {
			firstCandidate = generatedRandoms[0];

			generatedRandoms.erase(
				std::find(generatedRandoms.begin(), generatedRandoms.end(), firstCandidate)
			);
		}
		else {
			generated = distribution(gen);

			while (*boundIter < generated
				&& boundIter != boundsVector->end()) {
				firstCandidate++;
				boundIter++;
			}
			// firstCandidate--;
		}

		// Check if we generated something we can take
		if (!generatedRandoms.empty()) {
			for (int& vertex : generatedRandoms) {
				if (firstCandidate != vertex) {
					secondCandidate = vertex;
					generatedRandoms.erase(
						std::find(generatedRandoms.begin(), generatedRandoms.end(), secondCandidate)
					);
					break;
				}
			}
		}

		// If we did, save and end this iteration early
		if (secondCandidate != firstCandidate)
			pairsVector.push_back(std::tuple<int, int>(firstCandidate, secondCandidate));

		// Otherwise, go standard
		else {
			while (true) {
				secondCandidate = -1;
				generated = distribution(gen);
				boundIter = boundsVector->begin();

				while (*boundIter < generated
					&& boundIter != boundsVector->end()) {
					secondCandidate++;
					boundIter++;
				}
				// secondCandidate--;

				// Found candidate, save and exit
				if (secondCandidate != firstCandidate) {
					pairsVector.push_back(std::tuple<int, int>(firstCandidate, secondCandidate));
					break;
				}
				// Otherwise save for later
				else generatedRandoms.push_back(secondCandidate);
			}
		}

		childrenGenerated++;
	}

	return pairsVector;
}

PathData GeneticAlgorithm::generateChildOX(std::vector<short>* firstParent, std::vector<short>* secondParent) {
	PathData generatedChild;

	std::tuple<int, int> t = generateRandomTwoPositions(0, matrix->size - 2);

	std::vector<short>::iterator pathIterF =
		firstParent->begin();
	std::vector<short>::iterator pathIterB =
		firstParent->begin();

	std::advance(pathIterF, std::get<0>(t));
	std::advance(pathIterB, std::get<1>(t));

	// Copy fragment
	std::vector<short> segmentFromParent(std::get<1>(t) - std::get<0>(t) + 1);
	segmentFromParent.reserve(firstParent->size());
	std::copy(pathIterF, pathIterB + 1, segmentFromParent.begin());

	// From last place till end
	pathIterB = secondParent->begin();
	std::advance(pathIterB, std::get<1>(t) + 1);

	int buffer = 0;
	while (pathIterB != secondParent->end()) {
		if (std::find(segmentFromParent.begin(), segmentFromParent.end(), *pathIterB)
			== segmentFromParent.end()) {
			segmentFromParent.push_back(*pathIterB);
		}
		else buffer++;

		pathIterB++;
	}

	// From begin to first place copied
	pathIterB = secondParent->begin();
	std::advance(pathIterB, std::get<0>(t) + buffer);
	pathIterF = secondParent->begin();
	std::vector<short> childPath;
	childPath.reserve(firstParent->size());

	while (pathIterF != pathIterB) {
		if (std::find(segmentFromParent.begin(), segmentFromParent.end(), *pathIterF)
			== segmentFromParent.end()) {
			childPath.push_back(*pathIterF);
		}
		else pathIterB++;

		pathIterF++;
	}

	// Connect two vectors
	childPath.insert(childPath.end(), segmentFromParent.begin(), segmentFromParent.end());

	generatedChild.pathOrder = std::move(childPath);

	return generatedChild;
}

PathData GeneticAlgorithm::generateChildEAX(std::vector<short>* firstParent, std::vector<short>* secondParent) {
	PathData generatedChild;

	if (*firstParent == *secondParent) {
		repeat++;
		std::uniform_real_distribution<> distribution(0.0, 1.0);
		// Mutate
		if (distribution(gen) <= mutationConstant * (repeat + 1) / total) {
			std::tuple<int, int> iT = generateRandomTwoPositions(0, matrix->size - 2);
			generatedChild = getNewOrder(firstParent, std::get<0>(iT),
				std::get<1>(iT), &(matrix->mat));
		}
		else generatedChild.pathOrder = std::vector<short>(firstParent->begin(), firstParent->end());

		return generatedChild;
	}

	int matrixSize = matrix->size;
	generatedChild.pathOrder.reserve(matrixSize - 1);
	// Edge tables
	EdgeTable edgeTable(matrixSize - 1);

	// Fill edge table
	updateTable(&edgeTable, firstParent, secondParent);

	EdgeTable referenceCopy = edgeTable;

	// Vertices left
	std::vector<short> verticesLeft(matrixSize - 1);
	std::iota(verticesLeft.begin(), verticesLeft.end(), 1);
	std::shuffle(verticesLeft.begin(), verticesLeft.end(), gen);

	// Current vertex
	std::uniform_int_distribution<> distribution(0, 1);
	short current = 0;
	if (distribution(gen))
		current = (*firstParent)[0];
	else current = (*secondParent)[0];

	while (!verticesLeft.empty()) {
		// Add vertex
		generatedChild.pathOrder.push_back(current);

		// Delete current from verticesLeft
		auto iterLeft = std::find(verticesLeft.begin(), verticesLeft.end(), current);

		if (iterLeft != verticesLeft.end())
			verticesLeft.erase(iterLeft);

		// Find occurences
		std::vector<short> toDelete(referenceCopy.singleEdge[current - 1].begin(),
			referenceCopy.singleEdge[current - 1].end());

		toDelete.resize(referenceCopy.singleEdge[current - 1].size() +
			referenceCopy.doubleEdge[current - 1].size());

		std::copy_backward(referenceCopy.doubleEdge[current - 1].begin(),
			referenceCopy.doubleEdge[current - 1].end(), toDelete.end());

		// Delete occurences
		deleteOccurences(&edgeTable, current, &toDelete);

		// If not found next == -1
		short next = getNext(&edgeTable, current, generatedChild.pathOrder[0]);

		if (next == -1) {
			if (!verticesLeft.empty())
				current = verticesLeft[0];
			else break;
		}
		else current = next;
	}
	// std::cout << "\n";
	return generatedChild;
}

void GeneticAlgorithm::updateTable(EdgeTable* edgeTable, std::vector<short>* firstParent, std::vector<short>* secondParent) {
	int matrixSize = matrix->size;
	if (matrixSize < 250) {
		for (int i = 1; i <= matrixSize - 1; i++) {
			updateTableFind(edgeTable, firstParent, i);
			updateTableFind(edgeTable, secondParent, i);
		}
	}
	else updateTableBig(edgeTable, firstParent, secondParent);
}

void GeneticAlgorithm::updateTableFind(EdgeTable* edgeTable, std::vector<short>* parent, int vertexToFind) {
	std::vector<short>::iterator iter =
		std::find(parent->begin(), parent->end(), vertexToFind);

	// Check if double connection
		// Needs to be changed to checking singleEdge vectors, since "same connection" can be in different places of vectors
		// Maybe only look forward and for end() look at front()

	short firstValue = vertexToFind - 1, // -1 to correct for vector offset
		secondValue = (*iter == parent->back() ? parent->front() - 1 : (*(iter + 1) - 1));

	bool first = edgeTable->isInSingle(firstValue, secondValue + 1);
	bool second = edgeTable->isInSingle(secondValue, firstValue + 1);

	// Exists in single
	if (first || second) {
		// Delete from single...
		edgeTable->singleEdge[firstValue].erase(
			std::find(edgeTable->singleEdge[firstValue].begin(),
				edgeTable->singleEdge[firstValue].end(),
				secondValue + 1)
		);
		edgeTable->singleEdge[secondValue].erase(
			std::find(edgeTable->singleEdge[secondValue].begin(),
				edgeTable->singleEdge[secondValue].end(),
				firstValue + 1)
		);
		// ... move to double
		edgeTable->doubleEdge[firstValue].push_back(secondValue + 1);
		edgeTable->doubleEdge[secondValue].push_back(firstValue + 1);
	}
	// Doesn't exist in single, add to single
	else {
		edgeTable->singleEdge[firstValue].push_back(secondValue + 1);
		edgeTable->singleEdge[secondValue].push_back(firstValue + 1);
	}
}

// Better for bigger matrices
void GeneticAlgorithm::updateTableBig(EdgeTable* edgeTable, std::vector<short>* firstParent, std::vector<short>* secondParent) {
	// Iterate through firstParent vertices
	for (auto iter = firstParent->begin(); iter != firstParent->end(); iter++) {
		short secondValue = (*iter == firstParent->back() ? firstParent->front() : *(iter + 1));

		edgeTable->singleEdge[*iter - 1].push_back(secondValue);
		edgeTable->singleEdge[secondValue - 1].push_back(*iter);
	}

	// Iterate through secondParent vertices
	for (auto iter = secondParent->begin(); iter != secondParent->end(); iter++) {
		short secondValue = (*iter == secondParent->back() ? secondParent->front() : *(iter + 1));

		bool first = edgeTable->isInSingle(*iter - 1, secondValue);
		bool second = edgeTable->isInSingle(secondValue - 1, *iter);

		// Exists in single
		if (first || second) {
			// Delete from single...
			edgeTable->singleEdge[*iter - 1].erase(
				std::find(edgeTable->singleEdge[*iter - 1].begin(),
					edgeTable->singleEdge[*iter - 1].end(),
					secondValue)
			);
			edgeTable->singleEdge[secondValue - 1].erase(
				std::find(edgeTable->singleEdge[secondValue - 1].begin(),
					edgeTable->singleEdge[secondValue - 1].end(),
					*iter)
			);
			// ... move to double
			edgeTable->doubleEdge[*iter - 1].push_back(secondValue);
			edgeTable->doubleEdge[secondValue - 1].push_back(*iter);
		}
		// Doesn't exist in single, add to single
		else {
			edgeTable->singleEdge[*iter - 1].push_back(secondValue);
			edgeTable->singleEdge[secondValue - 1].push_back(*iter);
		}
	}
}

void GeneticAlgorithm::deleteOccurences(EdgeTable* edgeTable, short currentVertex, std::vector<short>* vertexToDelete) {
	std::vector<short>::iterator iterOccur;

	for (short& s : *vertexToDelete) {
		iterOccur = std::find(edgeTable->singleEdge[s - 1].begin(),
			edgeTable->singleEdge[s - 1].end(), currentVertex);

		// If found
		if (iterOccur != edgeTable->singleEdge[s - 1].end())
			edgeTable->singleEdge[s - 1].erase(iterOccur);

		iterOccur = std::find(edgeTable->doubleEdge[s - 1].begin(),
			edgeTable->doubleEdge[s - 1].end(), currentVertex);

		// If found
		if (iterOccur != edgeTable->doubleEdge[s - 1].end())
			edgeTable->doubleEdge[s - 1].erase(iterOccur);
	}
}

short GeneticAlgorithm::getNext(EdgeTable* edgeTable, short current, short currentFallback) {
	short next = 0;
	int tableSize = INT_MAX;
	std::vector<short>::iterator temp = edgeTable->doubleEdge[current - 1].begin();

	// Double edges exist
	if (!edgeTable->doubleEdge[current - 1].empty()) {
		while (temp != edgeTable->doubleEdge[current - 1].end()) {
			// If table size is lower than previous, set as next
			if (edgeTable->singleEdge[*temp - 1].size() +
				edgeTable->doubleEdge[*temp - 1].size() < INT_MAX) {
				tableSize = edgeTable->singleEdge[*temp - 1].size() +
					edgeTable->doubleEdge[*temp - 1].size();

				next = *temp;
			}
			temp++;
		}

		// Delete occurences from local
		edgeTable->doubleEdge[current - 1].erase(
			std::find(edgeTable->doubleEdge[current - 1].begin(),
				edgeTable->doubleEdge[current - 1].end(), next)
		);

		return next;
	}

	// Single edges exist
	if (!edgeTable->singleEdge[current - 1].empty()) {
		temp = edgeTable->singleEdge[current - 1].begin();

		while (temp != edgeTable->singleEdge[current - 1].end()) {
			// If table size is lower than previous, set as next
			if (edgeTable->singleEdge[*temp - 1].size() +
				edgeTable->doubleEdge[*temp - 1].size() < INT_MAX) {
				tableSize = edgeTable->singleEdge[*temp - 1].size() +
					edgeTable->doubleEdge[*temp - 1].size();

				next = *temp;
			}
			temp++;
		}

		// Delete occurences from local
		edgeTable->singleEdge[current - 1].erase(
			std::find(edgeTable->singleEdge[current - 1].begin(),
				edgeTable->singleEdge[current - 1].end(), next)
		);

		return next;
	}

	// Check other side
	if (currentFallback)
		return getNext(edgeTable, currentFallback, 0);

	return -1;
}

std::vector<PathData> GeneticAlgorithm::getNewRandomGeneration(std::vector<PathData>* parents, std::vector<PathData>* children) {
	if (children->empty())
		return *parents;

	int pSize = parents->size();
	std::vector<PathData> newGeneration;//(parents->size() / 2);
	newGeneration.reserve(pSize);

	auto compare = [](PathData x, PathData y)
		{ return x.pathLength < y.pathLength; };
	// Only use when you wanna be elite about children
	std::partial_sort(children->begin(), children->begin() + 1, children->end(), compare);

	std::vector<int> possibleIndexCombined(pSize + children->size() - 2);

	// Doesn't include position of elite child
	std::iota(possibleIndexCombined.begin(), possibleIndexCombined.begin() + pSize - 1, 1);
	std::iota(possibleIndexCombined.begin() + pSize - 1, possibleIndexCombined.end(), pSize + 1);

	std::shuffle(possibleIndexCombined.begin(), possibleIndexCombined.end(), gen);

	newGeneration.emplace_back(std::move((*parents)[0]));
	newGeneration.emplace_back(std::move((*children)[0]));

	for (int i = 0; i < startingPopulationSize - 2; i++) {
		if (possibleIndexCombined[i] < pSize)
			newGeneration.emplace_back(std::move((*parents)[possibleIndexCombined[i]]));
		else
			newGeneration.emplace_back(std::move((*children)[possibleIndexCombined[i] - pSize]));
	}

	return newGeneration;
}

std::tuple<std::vector<short>, int> GeneticAlgorithm::generateNewSolutionV(const short startVertex) {
	// Greedy method
	if (!startVertex)
		return generateInitialSolution();

	int matrixSize = matrix->size;
	std::vector<short> localPossibleVertices(matrixSize - 1), returnVector;
	int returnLength = INT_MAX;
	returnVector.reserve(matrixSize);

	// usun z mozliwych do wybrania wierzcholkow
	std::iota(localPossibleVertices.begin(), localPossibleVertices.end(), 1);
	localPossibleVertices.erase(
		std::find(localPossibleVertices.begin(), localPossibleVertices.end(), startVertex)
	);
	returnVector.push_back(startVertex);
	returnLength = matrix->mat[startVertex][0];

	// zmien "obecny" wierzcholek
	int currentVertex = 0;
	while (localPossibleVertices.size()) {
		int value = INT_MAX, lowestIndex = 1;
		// znajdz najkrotsza mozliwa sciezke
		for (int i = 1; i < matrixSize; i++) {
			if (matrix->mat[i][currentVertex] < value
				&& (std::find(localPossibleVertices.begin(), localPossibleVertices.end(), i) != std::end(localPossibleVertices))) {
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
		localPossibleVertices.erase(
			std::find(localPossibleVertices.begin(), localPossibleVertices.end(), lowestIndex)
		);
	}

	returnLength += matrix->mat[0][currentVertex];

	return std::make_tuple(returnVector, returnLength);
}

PathData GeneticAlgorithm::getNewOrder(std::vector<short>* currentOrder, int anchorOne, int anchorTwo, std::vector<std::vector<int>>* matrix) {
	PathData tempData;
	short previousVertex = 0;

	switch (currentNeighbourhoodType)
	{
	case NeighbourhoodType::INVERSE:
		tempData.pathOrder = inverse(currentOrder, anchorOne, anchorTwo);
		break;
	case NeighbourhoodType::SWAP:
		tempData.pathOrder = swap(currentOrder, anchorOne, anchorTwo);
		break;
	case NeighbourhoodType::INSERT:
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

	return tempData;
}

int GeneticAlgorithm::calculateCandidate(std::vector<short>* candidateOrder) {
	int pathLength = 0,
		previousVector = 0;
	std::vector<std::vector<int>>* matrixStart = &(matrix->mat);
	std::vector<short>::iterator ending = candidateOrder->end(),
		beginning = candidateOrder->begin();

	for (auto& iter = beginning; iter != ending; iter++) {
		pathLength += (*matrixStart)[*iter][previousVector];
		previousVector = *iter;
	}

	pathLength += (*matrixStart)[0][previousVector];

	return pathLength;
}