#pragma once
#include "CircularMotion/CircularMotion.h"

class BLUSHTree {

public:

	BLUSHTree();
	~BLUSHTree();

public:

	std::string treeName;

};

class BLUSH {

public:

	BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight);
	~BLUSH();

	bool Update();

private:

	void DrawTreeData(int initialX, int initialY);
	void SaveDataTrees();
	void LoadDataTrees();

public:


private:

	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	std::string currentTreeName;

};

