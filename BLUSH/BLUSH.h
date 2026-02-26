#pragma once
#include "CircularMotion/CircularMotion.h"
#define SAVE_DATA_FILE "BLUSH_Data.txt"
#define DATA_MENU_MULTIPLIER 0.6f

enum class PENDING_ACTION {

	NONE,
	CREATE,
	DELETE,
	MOVE

};

enum class NODE_TOGGLE {

	NONE,
	OPEN,
	CLOSE,
	SET_OPEN,
	SET_CLOSE

};

class BLUSHNode {

public:

	BLUSHNode();
	BLUSHNode(std::string _name);
	~BLUSHNode();

public:

	std::string nodeName;
	std::vector<BLUSHNode> childNodes;

};

class BLUSHTree {

public:

	BLUSHTree();
	BLUSHTree(std::string _name);
	~BLUSHTree();

public:

	std::string treeName;
	std::vector<BLUSHNode> rootNodes;

};

class BLUSH {

public:

	BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight);
	~BLUSH();

	bool Update();
	void SaveDataTrees();

private:

	void DrawTreeData(std::vector<BLUSHNode>& rootNodes, int initialX, int initialY);
	void DrawTreeDataEditingMenu(std::string& name, std::vector<BLUSHNode>& rootNodes, int sizeX);
	void LoadDataTrees();
	void SaveDataTreeChildNodes(const BLUSHNode& node, pugi::xml_node& xmlNode, int index);
	BLUSHNode LoadDataTreeChildNodes(pugi::xml_node& treeNode);

public:


private:

	char treeNameBuffer[64];
	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	int currentTreeIndex, currentNodeIndex;
	PENDING_ACTION pendingAction;
	NODE_TOGGLE nodeToggle;
	bool includeChildNodes;
	pugi::xml_document fileHandle;

};

