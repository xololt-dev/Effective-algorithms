// Effective-algorithms.h: plik dołączany dla standardowych systemowych plików dołączanych,
// lub pliki dołączane specyficzne dla projektu.

#pragma once

#include "algorithms.hpp"

#include <iostream>

enum class AlgorithmType {
	BruteForce,
	DynamicProgramming,
	TabuSearch,
	SimulatedAnnealing,
	GeneticAlgorithm
};

class Menu {
public:
	void mainMenu();
	
	// void benchmark(Matrix* matrix);

private:
	char option;
	int value;
	std::string fileName = "";
	Matrix matrix;

	BruteForce bf;
	DynamicProgramming dp;
	// TabuSearch ts;
	// SimulatedAnnealing sa;
	// GeneticAlgorithm ga;

	// NeighbourhoodType enumMenu();

	void bfMenu();
	void dpMenu();
	void tsMenu();
	void saMenu();
	void gaMenu();

	void bruteForce(int multithread = 1);
	void dynamicProgramming();

	void displayResults(AlgorithmType a_type);
};