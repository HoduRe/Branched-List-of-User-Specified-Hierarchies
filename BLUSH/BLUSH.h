#pragma once
#include "CircularMotion/CircularMotion.h"
#define SAVE_DATA_FILE "BLUSH_Data.txt"

class BLUSHNode {

public:

	BLUSHNode();
	BLUSHNode(std::string _name, BLUSHNode* _parent);
	~BLUSHNode();

	std::string nodeName;
	std::vector<BLUSHNode*> childNodes;
	BLUSHNode* parentNode;

};

class BLUSHTree {

public:

	BLUSHTree();
	BLUSHTree(std::string _name);
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
	void SaveDataTrees();

private:

	void DrawTreeData(int initialX, int initialY);
	void LoadDataTrees();
	void SaveDataTreeChildNodes(BLUSHNode* node, pugi::xml_node& xmlNode, int index);
	BLUSHNode LoadDataTreeChildNodes(BLUSHNode* parentNode, pugi::xml_node& treeNode);

public:


private:

	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	int currentTreeIndex;
	pugi::xml_document fileHandle;

};

