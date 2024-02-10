#pragma once

#include "util.hpp"

class Algorithm {
public:
	virtual void run() = 0;

	void setMatrix(Matrix* a_matrix)
		{ matrix = a_matrix; };
	void displayResults();

protected:
	Matrix* matrix;
	int pathLength;
	std::vector<short> vertexOrder;
	std::chrono::duration<double> runningTime;
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

	int dynamicHelperFunction(int maskCode, int currentVertex, std::vector<short>* vertexOrder, Matrix* matrix);
};

class AdvancedAlgorithm : public Algorithm {
protected:
	std::chrono::duration<double> maxExecutionTime = std::chrono::seconds(1);
	
	// Random
	std::random_device rd;
	std::mt19937 gen;

	std::vector<short> inverse(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> swap(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> insert(std::vector<short>* currentOrder, int firstPosition = 0, int secondPosition = 0);
	std::vector<short> insertSub(std::vector<short>* currentOrder);
};

class TabuSearch : public AdvancedAlgorithm {
public:

private:
	std::vector<short> tabu;
};

class SimulatedAnnealing : public AdvancedAlgorithm {
public:
	void setStopCriterium(int value);
	void setCoolingConstant(float value);
	void setNeighbourhoodType(NeighbourhoodType type);

private:
	double coolingConstant = 0.99f;
	NeighbourhoodType currentNeighbourhoodType = NeighbourhoodType::INVERSE;
};

class GeneticAlgorithm : public AdvancedAlgorithm {

};