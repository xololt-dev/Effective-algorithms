// Effective-algorithms.h: plik dołączany dla standardowych systemowych plików dołączanych,
// lub pliki dołączane specyficzne dla projektu.

#pragma once

#include "algorithms.hpp"

#include <iostream>

class Menu {
public:
	void mainMenu();

private:
	char option = 'a';
	int value = 0;
	std::string fileName = "";

	Matrix matrix;

	BruteForce bf;
	DynamicProgramming dp;
	TabuSearch ts;
	SimulatedAnnealing sa;
	GeneticAlgorithm ga;

	void bfMenu();
	void dpMenu();
	void tsMenu();
	void saMenu();
	void saSettingsMenu();
	void gaMenu();
	void gaSettingsMenu();
	void neighbourMenu();

	void menuSetup();
	void setMatrixAll();
	void setStopCriterium(int a_value);
};