#include "menu.hpp"

#include <conio.h>

void Menu::menuSetup() {
	ts.initRandom();
	sa.initRandom();
	ga.initRandom();

	setMatrixAll();
}

void Menu::mainMenu() {
	menuSetup();

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
			if (!matrix.loadFromFile(fileName))
				fileName = "";
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
			bfMenu();
			clear();
			break;

		case '5':
			clear();
			dpMenu();
			clear();
			break;

		case '6':
			clear();
			tsMenu();
			clear();
			break;

		case '7':
			clear();
			saMenu();
			clear();
			break;

		case '8':
			clear();
			gaMenu();
			clear();
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
			if (!matrix.loadFromFile(fileName))
				fileName = "";
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
			if (!matrix.loadFromFile(fileName))
				fileName = "";
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
			dp.displayResults();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::tsMenu() {
	do {
		std::cout << "==== TABU SEARCH ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Zmien dlugosc wykonywania: " << ts.getStopCriterium() <<"\n";
		std::cout << "5.Zmien typ sasiedztwa: ";
		switch (ts.getNeighbourhoodType())
		{
		case NeighbourhoodType::INVERSE:
			std::cout << "INVERSE\n";
			break;
		case NeighbourhoodType::SWAP:
			std::cout << "SWAP\n";
			break;
		case NeighbourhoodType::INSERT:
			std::cout << "INSERT\n";
			break;
		default:
			std::cout << "NONE\n";
			break;
		}
		std::cout << "6.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru: ";
			std::cin >> fileName;
			if (!matrix.loadFromFile(fileName))
				fileName = "";
			clear();
			break;

		case '2':
			std::cout << "Podaj ilosc wierzcholkow: ";
			std::cin >> value;
			matrix.generate(value);
			clear();
			break;

		case '3':
			clear();
			ts.displayResults();
			break;

		case '4':
			std::cout << "Podaj dlugosc wykonania (s): ";
			std::cin >> value;
			setStopCriterium(value);
			clear();
			break;

		case '5':
			clear();
			neighbourMenu();
			break;

		case '6':
			clear();
			ts.run();
			ts.displayResults();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::neighbourMenu() {
	char option;
	do {
		std::cout << "\n==== SASIEDZTWO ===\n";
		std::cout << "1.Odwrotna kolejnosc\n";
		std::cout << "2.Zamiana miejsc\n";
		std::cout << "3.Wstaw w miejsce\n";
		std::cout << "4.Wstaw podsciezke - tylko wyzarzanie\n";
		std::cout << "0.Powrot\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			ts.setNeighbourhoodType(NeighbourhoodType::INVERSE);
			sa.setNeighbourhoodType(NeighbourhoodType::INVERSE);
			ga.setNeighbourhoodType(NeighbourhoodType::INVERSE);
			return;

		case '2':
			ts.setNeighbourhoodType(NeighbourhoodType::SWAP);
			sa.setNeighbourhoodType(NeighbourhoodType::SWAP);
			ga.setNeighbourhoodType(NeighbourhoodType::SWAP);
			return;

		case '3':
			ts.setNeighbourhoodType(NeighbourhoodType::INSERT);
			sa.setNeighbourhoodType(NeighbourhoodType::INSERT);
			ga.setNeighbourhoodType(NeighbourhoodType::INSERT);
			return;

		case '4':
			ts.setNeighbourhoodType(NeighbourhoodType::INSERT_SUB);
			sa.setNeighbourhoodType(NeighbourhoodType::INSERT_SUB);
			ga.setNeighbourhoodType(NeighbourhoodType::INSERT_SUB);
			return;
		}
	} while (option != '0');
}

void Menu::saMenu() {
	do {
		std::cout << "==== SIMULATED ANNEALING ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Ustawienia\n";
		std::cout << "5.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			if (!matrix.loadFromFile(fileName))
				fileName = "";
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
			sa.displayResults();
			break;

		case '4':
			clear();
			saSettingsMenu();
			clear();
			break;

		case '5':
			clear();
			sa.run();
			sa.displayResults();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::saSettingsMenu() {
	do {
		std::cout << "==== SIMULATED ANNEALING SETTINGS ====\n";
		std::cout << "1.Zmien dlugosc wykonywania (s): " << sa.getStopCriterium() << "\n";
		std::cout << "2.Zmien typ sasiedztwa: ";
		switch (sa.getNeighbourhoodType())
		{
		case NeighbourhoodType::INVERSE:
			std::cout << "INVERSE\n";
			break;
		case NeighbourhoodType::SWAP:
			std::cout << "SWAP\n";
			break;
		case NeighbourhoodType::INSERT:
			std::cout << "INSERT\n";
			break;
		default:
			std::cout << "NONE\n";
			break;
		};
		std::cout << "3.Zmien wspolczynnik schladzania (default 0.99): " << sa.getCoolingConstant() << "\n";
		std::cout << "4.Zmien temperature poczatkowa (auto = 0.0): " << sa.getStartingTemp() << "\n";
		std::cout << "5.Zmien dlugosc ery (auto = 0): " << sa.getEraLength() << "\n";
		std::cout << "6.Zmien czas iteracji do critical event (auto = 0): " << sa.getMaxNonImproved() << "\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		double v;
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << "Podaj dlugosc wykonania (s): ";
			std::cin >> value;
			setStopCriterium(value);
			clear();
			break;

		case '2':
			neighbourMenu();
			clear();
			break;

		case '3':
			std::cout << "Podaj wspolczynnik schladzania: ";
			std::cin >> v;
			sa.setCoolingConstant(v);
			clear();
			break;

		case '4':
			std::cout << "Podaj temperature poczatkowa: ";
			std::cin >> v;
			sa.setStartingTemp(v);
			clear();
			break;

		case '5':
			std::cout << "Podaj dlugosc ery: ";
			std::cin >> value;
			sa.setEraLength(value);
			clear();
			break;

		case '6':
			std::cout << "Podaj ilosc iteracji bez poprawy: ";
			std::cin >> value;
			sa.setMaxNonImproved(value);
			clear();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::gaMenu() {
	do {
		std::cout << "==== GENETIC ALGORITHM ====\n";
		std::cout << "1.Wczytaj z pliku - " << fileName << "\n";
		std::cout << "2.Generuj przyklad\n";
		std::cout << "3.Wyswietl wynik\n";
		std::cout << "4.Ustawienia\n";
		std::cout << "5.Uruchom algorytm\n";
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche();
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << " Podaj nazwe zbioru:";
			std::cin >> fileName;
			if (!matrix.loadFromFile(fileName))
				fileName = "";
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
			ga.displayResults();
			break;

		case '4':
			clear();
			gaSettingsMenu();
			clear();
			break;

		case '5':
			clear();
			ga.run();
			ga.displayResults();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::gaSettingsMenu() {
	do {
		std::cout << "==== GENETIC ALGORITHM SETTINGS ====\n";
		std::cout << "1.Zmien dlugosc wykonywania: " << ga.getStopCriterium() << "\n";
		std::cout << "2.Zmien typ sasiedztwa: ";
		switch (ga.getNeighbourhoodType())
		{
		case NeighbourhoodType::INVERSE:
			std::cout << "INVERSE\n";
			break;
		case NeighbourhoodType::SWAP:
			std::cout << "SWAP\n";
			break;
		case NeighbourhoodType::INSERT:
			std::cout << "INSERT\n";
			break;
		default:
			std::cout << "NONE\n";
			break;
		};
		std::cout << "3.Zmien rozmiar populacji startowej (default = 10): " << ga.getStartingPopulationSize() << "\n";
		std::cout << "4.Zmien losowosc generacji startowej (auto = 0): " << ga.getStartingPopulationRandom() << "\n";
		std::cout << "5.Zmien wspolczynnik mutacji (default = 0.01): " << ga.getMutationConstant() << "\n";
		std::cout << "6.Zmien wspolczynnik krzyzowania (default = 0.8): " << ga.getCrossoverConstant() << "\n";
		std::cout << "7.Zmien typ krzyzowania: " << (ga.getCrossoverType() ? "EAX\n" : "OX\n");
		std::cout << "0.Wyjdz\n";
		std::cout << "Podaj opcje:";
		option = _getche(); 
		double v;
		std::cout << std::endl;

		switch (option) {
		case '1':
			std::cout << "Podaj dlugosc wykonania (s): ";
			std::cin >> value;
			setStopCriterium(value);
			clear();
			break;

		case '2':
			neighbourMenu();
			clear();
			break;

		case '3':
			std::cout << "Podaj rozmiar populacji startowej: ";
			std::cin >> value;
			ga.setStartingPopulationSize(value);
			clear();
			break;

		case '4':
			ga.setStartingPopulationRandom();
			clear();
			break;

		case '5':
			std::cout << "Podaj wspolczynnik mutacji: ";
			std::cin >> v;
			ga.setMutationConstant(v);
			clear();
			break;

		case '6':
			std::cout << "Podaj wspolczynnik krzyzowania: ";	
			std::cin >> v;
			ga.setCrossoverConstant(v);
			clear();
			break;

		case '7':
			std::cout << "Podaj typ krzyzowania 0X - 0, EAX - 1: ";
			std::cin >> value;
			ga.setCrossoverType(value);
			clear();
			break;

		default:
			break;
		}
	} while (option != '0');

	option = '9';
}

void Menu::setMatrixAll() {
	bf.setMatrix(&matrix);
	dp.setMatrix(&matrix);
	ts.setMatrix(&matrix);
	sa.setMatrix(&matrix);
	ga.setMatrix(&matrix);
}

void Menu::setStopCriterium(int a_value) {
	ts.setStopCriterium(a_value);
	sa.setStopCriterium(a_value);
	ga.setStopCriterium(a_value);
}
