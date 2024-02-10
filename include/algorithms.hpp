#pragma once

#include "util.hpp"

class Algorithm {
public:
	virtual void run() = 0;

	void setMatrix(Matrix* a_matrix)
		{ matrix = a_matrix; };
	void displayResults();

protected:
	Matrix* matrix = nullptr;
	int pathLength = 0;
	std::vector<short> vertexOrder;
	std::chrono::duration<double> runningTime = std::chrono::seconds(0);
};

class BruteForce : public Algorithm {
public:
	virtual void run();
	void setMultithread(bool a_multithread) 
		{ multithread = a_multithread; }

protected:
	bool multithread = false;

	int bruteHelperFunction(std::vector<short>* orderQueue);
	static void bruteHelperMultithread(std::vector<short>* orderQueue, int* pathLength, std::vector<short> permutation, int permutationNumber, Matrix* matrix);
};

class DynamicProgramming : public Algorithm {
public:
	virtual void run();
protected:
	std::vector<std::unordered_map<int, Cache>> cachedPathsNew;

	int dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder);
};

class AdvancedAlgorithm : public Algorithm {
public:
	int getStopCriterium() 
		{ return maxExecutionTime.count(); };
	NeighbourhoodType getNeighbourhoodType()
		{ return currentNeighbourhoodType; };

	void setStopCriterium(int a_value)
		{ maxExecutionTime = std::chrono::seconds(a_value); };
	void setNeighbourhoodType(NeighbourhoodType a_type)
		{ currentNeighbourhoodType = a_type; };
	void initRandom()
		{ gen.seed(rd()); };

protected:
	NeighbourhoodType currentNeighbourhoodType = NeighbourhoodType::INSERT;
	std::chrono::duration<double> maxExecutionTime = std::chrono::seconds(1);
	
	int getPathDelta();

	// Random
	std::random_device rd;
	std::mt19937 gen;

	std::tuple<int, int> generateRandomTwoPositions(int lowerBound, int higherBound, bool correctOrder = 1);
	std::tuple<std::vector<short>, int> generateInitialSolution();
	std::tuple<std::vector<short>, int> generateNewSolution(int notAllowedSecondary);

	std::vector<short> inverse(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> swap(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> insert(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> insertSub(std::vector<short>* currentOrder);
};

class TabuSearch : public AdvancedAlgorithm {
public:
	virtual void run();
private:
	double improvConstant = 1.1f;
	std::vector<short> tabu;

	QueueData generateBestMove(std::vector<short>* currentOrder, int bestLength, std::vector<std::vector<int>>* matrix);
	QueueData getNewOrder(std::vector<short>* currentOrder, int anchorOne, int anchorTwo, std::vector<std::vector<int>>* matrix);
	void updateTabu(int anchorOne, int anchorTwo, int matrixSize);
};

class SimulatedAnnealing : public AdvancedAlgorithm {
public:
	virtual void run();

	int getEraLength()
		{ return eraLength; };
	int getMaxNonImproved()
		{ return maxNonImproved; };
	double getCoolingConstant()
		{ return coolingConstant; };
	double getStartingTemp()
		{ return startingTemp; };

	void setEraLength(int a_value)
		{ eraLength = a_value; };
	void setMaxNonImproved(int a_value)
		{ maxNonImproved = a_value; };
	void setCoolingConstant(double a_value)
		{ coolingConstant = a_value; };
	void setStartingTemp(double a_value)
		{ startingTemp = a_value; };

private:
	int eraLength = 0,
		maxNonImproved = 0;
	double	coolingConstant = 0.99f,
			startingTemp = 0.0f;
	
	std::vector<short> generateRandomCandidate(std::vector<short>* currentOrder);
	bool changeSolutions(int candidatePath, int currentPath, double currentTemp);
	int calculateCandidate(std::vector<short>* candidateOrder);
};

class GeneticAlgorithm : public AdvancedAlgorithm {
public:
	virtual void run();

	int getStartingPopulationSize()
		{ return startingPopulationSize; };
	bool getStartingPopulationRandom()
		{ return startingPopulationRandom; };
	double getMutationConstant()
		{ return mutationConstant; };
	double getCrossoverConstant()
		{ return crossoverConstant; };
	int getCrossoverType()
		{ return currentCrossoverType; };

	void setStartingPopulationSize(int a_value) {
		if (a_value >= 3)
			startingPopulationSize = a_value;
		else
			startingPopulationSize = 3;
	};
	void setStartingPopulationRandom()
		{ startingPopulationRandom = !startingPopulationRandom; };
	void setMutationConstant(double a_value)
		{ mutationConstant = a_value; };
	void setCrossoverConstant(double a_value) 
		{ crossoverConstant = a_value; };
	void setCrossoverType(int a_type) 
		{ a_type ? currentCrossoverType = 1 : currentCrossoverType = 0; };

private:
	int startingPopulationSize = 10; // min 3
	bool startingPopulationRandom = false; // true = random, false = greedy path
	double mutationConstant = 0.01f;
	double crossoverConstant = 0.8f;
	int currentCrossoverType = 0; // 0 OX, 1 EAX

	int repeat = 0;
	int total = 0;

	void geneticOX();
	void geneticEAX();

	std::vector<PathData> generateStartingPopulation();
	void generateRandomStartingPopulation(std::vector<short>* leftVertices, std::vector<PathData>* returnVec);
	void generateGreedyStartingPopulation(std::vector<short>* leftVertices, std::vector<PathData>* returnVec);
	std::vector<double> getVertexLowerBounds(int vectorSize);
	std::vector<std::tuple<int, int>> generateParents(std::vector<double>* boundsVector, int vectorSize);
	// OX
	PathData generateChildOX(std::vector<short>* firstParent, std::vector<short>* secondParent);
	// EAX
	PathData generateChildEAX(std::vector<short>* firstParent, std::vector<short>* secondParent);

	void updateTable( EdgeTable* edgeTable, std::vector<short>* firstParent, std::vector<short>* secondParent);
	void updateTableFind(EdgeTable* edgeTable, std::vector<short>* parent, int vertexToFind);
	void updateTableBig(EdgeTable* edgeTable, std::vector<short>* firstParent, std::vector<short>* secondParent);
	void deleteOccurences(EdgeTable* edgeTable, short currentVertex, std::vector<short>* vertexToDelete);
	short getNext(EdgeTable* edgeTable, short current, short currentFallback);

	std::vector<PathData> getNewRandomGeneration(std::vector<PathData>* parents, std::vector<PathData>* children);

	PathData getNewOrder(std::vector<short>* currentOrder, int anchorOne, int anchorTwo, std::vector<std::vector<int>>* matrix);
	int calculateCandidate(std::vector<short>* candidateOrder);
	std::tuple<std::vector<short>, int> generateNewSolutionV(short startVertex);
};