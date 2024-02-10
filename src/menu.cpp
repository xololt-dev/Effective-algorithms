#include "menu.hpp"

#include <conio.h>

void Menu::mainMenu() {
	do {
		std::cout << "==== MENU GLOWNE ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wyniki\n";
		std::cout << "4.Brute Force\n";
		std::cout << "5.Dynamic Programming\n";
		std::cout << "6.Tabu Search\n";
		std::cout << "7.Simulated Annealing\n";
		std::cout << "8.Genetic Algorithm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow:";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			matrix.display();
			break;

		case '4':
			clear();
			bf.setMatrix(&matrix);
			bfMenu();
			break;

		case '5':
			clear();
			dp.setMatrix(&matrix);
			dpMenu();
			break;

		default:
			break;
		}
	} while (option != '0');
}

void Menu::bfMenu() {
	do {
		std::cout << "==== BRUTE FORCE ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Uruchom algorytm (ST)\n";
		std::cout << "5.Uruchom algorytm (MT)\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow:";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			bf.displayResults();
			break;

		case '4':
			clear();
			bf.setMultithread(false);
			bf.run();
			bf.displayResults();
			break;

		case '5':
			clear();
			bf.setMultithread(true);
			bf.run();
			bf.displayResults();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::dpMenu() {
	do {
		std::cout << "==== DYNAMIC PROGRAMMING ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow:";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			dp.displayResults();
			break;

		case '4':
			clear();
			dp.run();
			break;

		default:
			break;
		}
	} while (option != '0');
}

void Menu::tsMenu() {
	do {
		std::cout << "==== TABU SEARCH ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			matrix.loadFromFile(fileName);
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow:";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			dp.displayResults();
			break;

		case '4':
			clear();
			dp.run();
			break;

		default:
			break;
		}
	} while (option != '0');
}

void Menu::displayResults(AlgorithmType a_type) {
	switch (a_type)	{
	case AlgorithmType::BruteForce:
		bf.displayResults();
		break;
	case AlgorithmType::DynamicProgramming:
		dp.displayResults();
		break;
	case AlgorithmType::TabuSearch:
		// ts.displayResults();
		break;
	case AlgorithmType::SimulatedAnnealing:
		// sa.displayResults();
		break;
	case AlgorithmType::GeneticAlgorithm:
		// ga.displayResults();
		break;
	default:
		break;
	}
}