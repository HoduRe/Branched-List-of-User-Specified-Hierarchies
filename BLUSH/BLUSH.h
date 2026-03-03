#pragma once
#include "CircularMotion/CircularMotion.h"
#define SAVE_DATA_FILE "BLUSH_Data.txt"
#define DATA_MENU_MULTIPLIER 0.6f
#define NODE_NAME_BUFFER_SIZE 256

enum class PENDING_ACTION {

	NONE,
	MOVE,
	MOVE_UP,
	MOVE_DOWN

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

	int nodeID;
	std::string nodeName;
	char nameBuffer[NODE_NAME_BUFFER_SIZE];
	std::vector<BLUSHNode> childNodes; // In this particular day, I refuse to work with lists, even if it's the right decision :3

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

	BLUSHTree* GetTree(int index);
	std::pair<BLUSHNode*, BLUSHNode*> GetNodeAndParentByID(int id);
	std::pair<BLUSHNode*, BLUSHNode*> GetChildNodeAndParentByID(BLUSHNode& parentNode, int id);
	bool ContainsNode(int id);
	bool ContainsNewNodeChild(BLUSHNode& parentNode, int id);
	void ReorderNode(int id);
	void ReorderChildNode(std::vector<BLUSHNode>& parentNode, int id);
	void DeleteNodeByID(int id, bool deleteChilds);
	int DeleteChildNodeByID(BLUSHNode& parentNode, int id, bool deleteChilds);

	void HandleTreeNodes(int upId, int downId);
	void DrawTreeData(std::vector<BLUSHNode>& rootNodes, int initialX, int initialY);
	void DrawTreeChildData(BLUSHNode& node);
	void DrawTreeDataEditingMenu(std::string& name, std::vector<BLUSHNode>& rootNodes, int sizeX);
	void HandlePendingAction();
	void MoveNode(int movedId, int newParentId);
	void LoadDataTrees();
	void SaveDataTreeChildNodes(const BLUSHNode& node, pugi::xml_node& xmlNode, int index);
	BLUSHNode LoadDataTreeChildNodes(pugi::xml_node& treeNode);

public:


private:

	char treeNameBuffer[64];
	SDL_Window* windowRef;
	int screenWidth, screenHeight;
	std::vector<BLUSHTree> trees;
	int currentTreeIndex, selectedNode, selectedNodeAux, newNodeIndex, moveNodeIndex;
	PENDING_ACTION pendingAction;
	NODE_TOGGLE nodeToggle;
	bool includeChildNodes;
	pugi::xml_document fileHandle;

};

