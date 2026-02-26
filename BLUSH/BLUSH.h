#pragma once
#include "CircularMotion/CircularMotion.h"

class BLUSHNode {

public:

	BLUSHNode();
	~BLUSHNode();

	std::string nodeName;
	std::vector<BLUSHNode*> childNodes;
	BLUSHNode* parentNode;

};

class BLUSHTree {

public:

	BLUSHTree();
	~BLUSHTree();

public:

	std::string treeName;
	std::vector<BLUSHNode*> rootNodes;

};

class BLUSH {

public:

	BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight);
	~BLUSH();

	bool Update();

private:

	void DrawTreeData(int initialX, int initialY);
	void SaveDataTrees();
	void SaveDataTreeChildNodes(BLUSHNode* node, pugi::xml_node& xmlNode, int index);
	void LoadDataTrees();

public:


private:

	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	std::string currentTreeName;
	pugi::xml_document fileHandle;

};

