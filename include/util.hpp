#pragma once

#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <random>
#include <iostream>
#include <tuple>
#include <queue>

enum class NeighbourhoodType {
	INVERSE,
	SWAP,
	INSERT,
	INSERT_SUB
};

struct QueueData {
	std::vector<short> pathOrder;
	int pathLength = 0;
	int anchorOne = 0;
	int anchorTwo = 0;
};

class Matrix {
public:
	int size = 0;
	// wype³niane wierszami
	std::vector<std::vector<int>> mat;

	bool loadFromFile(std::string fileName);
	void generate(int size);
	void display();

private:
	bool loadFromTXT(std::string fileName);
	bool loadFromATSP(std::string fileName);
};

struct Cache {
	std::vector<short> path;
	int pathLength = 0;
	Cache() : 
		path({ 0 }), pathLength(0) 
	{ }

	Cache(std::vector<short> pathIn, int pathLengthIn) : 
		path(pathIn), pathLength(pathLengthIn) 
	{ }
};

struct PathData {
	std::vector<short> pathOrder;
	int pathLength = 0;

	PathData() {}

	~PathData() {}

	PathData(PathData& other) {		//copy constructor
		pathOrder = other.pathOrder;
		pathLength = other.pathLength;
	}

	PathData& operator=(PathData other) {
		pathOrder = other.pathOrder;
		pathLength = other.pathLength;

		return *this;
	}

	PathData(PathData&& qd) noexcept :
		pathOrder(std::move(qd.pathOrder)),					// explicit move of a member of class type
		pathLength(std::exchange(qd.pathLength, 0))	// explicit move of a member of non-class type
	{}
};

struct EdgeTable {
	std::vector<std::vector<short>> singleEdge,
									doubleEdge;

	bool isInSingle(int valueOne, int valueTwo) {
		if (singleEdge[valueOne].empty())
			return false;

		// Not found
		if (std::find(singleEdge[valueOne].begin(),
			singleEdge[valueOne].end(),
			valueTwo) == singleEdge[valueOne].end())
			return false;

		return true;
	};

	void display() {
		for (int i = 0; i < singleEdge.size(); i++) {
			if (!singleEdge[i].empty() || !doubleEdge[i].empty()) {
				std::cout << i + 1 << ": ";
				for (short s : singleEdge[i])
					std::cout << s << " ";
				for (short s : doubleEdge[i])
					std::cout << s << "+ ";
				std::cout << "\n";
			}
		}
		std::cout << "\n";
	}

	EdgeTable() {}

	EdgeTable(int matrixSize) :
		singleEdge(std::vector<std::vector<short>>(matrixSize)),
		doubleEdge(std::vector<std::vector<short>>(matrixSize)) {
		for (std::vector<short>& v : singleEdge) {
			v.reserve(4);
		}
		for (std::vector<short>& v : doubleEdge) {
			v.reserve(4);
		}
	}

	~EdgeTable() {}
};

void clear();