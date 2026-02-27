#pragma once
#include "CircularMotion/CircularMotion.h"
#define SAVE_DATA_FILE "BLUSH_Data.txt"
#define DATA_MENU_MULTIPLIER 0.6f

enum class PENDING_ACTION {

	NONE,
	CREATE,
	DELETE,
	MOVE_TO_ROOT,
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

	BLUSHNode(std::string _name = "NewNode");
	~BLUSHNode();

public:

	const int nodeID;
	std::string nodeName;
	std::vector<BLUSHNode> childNodes;

private:

	static int idGenerator;

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

	BLUSHNode* GetNodeByID(int id);
	BLUSHNode* GetChildNodeByID(BLUSHNode& parentNode, int id);

	void DrawTreeData(std::vector<BLUSHNode>& rootNodes, int initialX, int initialY);
	void DrawTreeChildData(const BLUSHNode& node, int level);
	void DrawTreeDataEditingMenu(std::string& name, std::vector<BLUSHNode>& rootNodes, int sizeX);
	void HandlePendingAction();
	void LoadDataTrees();
	void SaveDataTreeChildNodes(const BLUSHNode& node, pugi::xml_node& xmlNode, int index);
	BLUSHNode LoadDataTreeChildNodes(pugi::xml_node& treeNode);

public:


private:

	char treeNameBuffer[64];
	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	int currentTreeIndex, currentNodeIndex, selectedNode, selectedNodeAux;
	PENDING_ACTION pendingAction;
	NODE_TOGGLE nodeToggle;
	bool includeChildNodes;
	pugi::xml_document fileHandle;

};

