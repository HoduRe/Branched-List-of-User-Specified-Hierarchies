#include "BLUSH.h"

int BLUSHNode::idGenerator = 0;

BLUSHNode::BLUSHNode(std::string _name) : nodeName(_name), childNodes(), nodeID(idGenerator++) { strncpy_s(nameBuffer, _name.c_str(), _name.length()); }

BLUSHNode::~BLUSHNode() {}

BLUSHTree::BLUSHTree() : treeName("NewTree"), rootNodes() {}

BLUSHTree::BLUSHTree(std::string _name) : treeName(_name), rootNodes() {}

BLUSHTree::~BLUSHTree() {}

BLUSH::BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight) : windowRef(_window), screenWidth(_screenWidth), screenHeight(_screenHeight),
currentTreeIndex(0), trees(), fileHandle(), treeNameBuffer(""), pendingAction(PENDING_ACTION::NONE), includeChildNodes(true), selectedNode(-1),
selectedNodeAux(-1), nodeToggle(NODE_TOGGLE::SET_OPEN), newNodeIndex(-1), moveNodeIndex(-1) {

	LoadDataTrees();

}

BLUSH::~BLUSH() {

	for (size_t i = 0; i < trees.size(); i++) { trees[i].rootNodes.clear(); }
	trees.clear();

}

bool BLUSH::Update() {

	bool ret = true;
	static ImVec2 winSize = ImVec2(screenWidth * 0.3f, screenHeight * 0.8f);
	static ImVec2 winPos = ImVec2(screenWidth * 0.05f, screenHeight * 0.1f);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree List", NULL, flags);

	if (ImGui::Button("Create New Tree")) {

		int newIndex = trees.size();
		trees.push_back(BLUSHTree());
		currentTreeIndex = newIndex;
		strcpy_s(treeNameBuffer, sizeof(treeNameBuffer), trees[newIndex].treeName.c_str());

	}

	int upTree = -1, downTree = -1;

	for (size_t i = 0; i < trees.size(); i++) {

		ImGui::NewLine();
		if (ImGui::Button(ImGuiBase::MakeImGuiName("^", i).c_str())) { upTree = i; } ImGui::SameLine();
		if (ImGui::Button(ImGuiBase::MakeImGuiName("v", i).c_str())) { downTree = i; } ImGui::SameLine();

		if (ImGui::Selectable(ImGuiBase::MakeImGuiName(trees[i].treeName, i).c_str(), currentTreeIndex == i)) {

			currentTreeIndex = i;
			strcpy_s(treeNameBuffer, sizeof(treeNameBuffer), trees[i].treeName.c_str());

		}

		if (i == currentTreeIndex) {

			DrawTreeDataEditingMenu(trees[i].treeName, trees[i].rootNodes, winPos.x + winSize.x);
			DrawTreeData(trees[i].rootNodes, winPos.x + winSize.x, winPos.y);

		}

	}

	ImGui::End();

	return ret;

}


BLUSHTree* BLUSH::GetTree(int index) { return currentTreeIndex < trees.size() ? &trees[index] : nullptr; }


std::pair<BLUSHNode*, BLUSHNode*> BLUSH::GetNodeAndParentByID(int id) {

	std::pair<BLUSHNode*, BLUSHNode*> nodeAndParent(nullptr, nullptr);

	if (!(currentTreeIndex < trees.size())) { return nodeAndParent; }

	for (size_t i = 0; i < trees[currentTreeIndex].rootNodes.size(); i++) {

		nodeAndParent = GetChildNodeAndParentByID(trees[currentTreeIndex].rootNodes[i], id);
		if (nodeAndParent.first != nullptr) { return nodeAndParent; }

	}

	return nodeAndParent;

}


std::pair<BLUSHNode*, BLUSHNode*> BLUSH::GetChildNodeAndParentByID(BLUSHNode& parentNode, int id) {

	std::pair<BLUSHNode*, BLUSHNode*> nodeAndParent(nullptr, nullptr);

	if (parentNode.nodeID == id) {

		nodeAndParent.first = &parentNode;
		return nodeAndParent;

	}

	for (size_t i = 0; i < parentNode.childNodes.size(); i++) {

		nodeAndParent = GetChildNodeAndParentByID(parentNode.childNodes[i], id);

		if (nodeAndParent.first != nullptr) {

			if (nodeAndParent.second == nullptr) { nodeAndParent.second = &parentNode; }
			return nodeAndParent;

		}

	}

	return nodeAndParent;

}


bool BLUSH::ContainsNode(int id) {

	if (id == -1) { return false; }

	for (size_t i = 0; i < trees[currentTreeIndex].rootNodes.size(); i++) {

		bool contains = ContainsNewNodeChild(trees[currentTreeIndex].rootNodes[i], id);
		if (contains) { return true; }

	}

	return false;

}


bool BLUSH::ContainsNewNodeChild(BLUSHNode& parentNode, int id) {

	if (parentNode.nodeID == id) { return true; }

	for (size_t i = 0; i < parentNode.childNodes.size(); i++) {

		bool contains = ContainsNewNodeChild(parentNode.childNodes[i], id);
		if (contains) { return true; }

	}

	return false;

}


void BLUSH::ReorderNode(int id) {

	if (id == -1) { return; }
	ReorderChildNode(trees[currentTreeIndex].rootNodes, id);

}


void BLUSH::ReorderChildNode(std::vector<BLUSHNode>& parentNode, int id) {

	for (size_t i = 0; i < parentNode.size(); i++) {

		BLUSHNode& node = parentNode[i];

		if (node.nodeID == id) {

			if (pendingAction == PENDING_ACTION::MOVE_UP) {

				if (i != 0) {

					parentNode.emplace(parentNode.begin() + i - 1, node);
					parentNode.erase(parentNode.begin() + i + 1);

				}

				pendingAction = PENDING_ACTION::NONE;

			}

			if (pendingAction == PENDING_ACTION::MOVE_DOWN) {

				if (i != parentNode.size() - 1) {

					parentNode.emplace(parentNode.begin() + i, parentNode[i + 1]);
					parentNode.erase(parentNode.begin() + i + 2);

				}

				pendingAction = PENDING_ACTION::NONE;

			}

			return;

		}

		else { ReorderChildNode(node.childNodes, id); }

	}

}


void BLUSH::DeleteNodeByID(int id, bool deleteChilds) {

	if (!(currentTreeIndex < trees.size())) { return; }

	for (size_t i = 0; i < trees[currentTreeIndex].rootNodes.size(); i++) {

		int toDelete = DeleteChildNodeByID(trees[currentTreeIndex].rootNodes[i], id, deleteChilds);

		if (toDelete == 1) {

			if (!deleteChilds) {

				std::vector<BLUSHNode>::iterator nodeIt = trees[currentTreeIndex].rootNodes.begin() + i; // Can't use this for erase because iterator will be broken due to vector repositioning its memory
				trees[currentTreeIndex].rootNodes.insert(trees[currentTreeIndex].rootNodes.end(), nodeIt->childNodes.begin(), nodeIt->childNodes.end());

			}

			trees[currentTreeIndex].rootNodes.erase(trees[currentTreeIndex].rootNodes.begin() + i);
			return;

		}

	}

}


int BLUSH::DeleteChildNodeByID(BLUSHNode& parentNode, int id, bool deleteChilds) {

	if (parentNode.nodeID == id) { return 1; }

	for (size_t i = 0; i < parentNode.childNodes.size(); i++) {

		int toDelete = DeleteChildNodeByID(parentNode.childNodes[i], id, deleteChilds);
		if (toDelete > 0) {

			if (toDelete == 1) {

				if (!deleteChilds) {

					std::vector<BLUSHNode>::iterator nodeIt = parentNode.childNodes.begin() + i; // Can't use this for erase because iterator will be broken due to vector repositioning its memory
					parentNode.childNodes.insert(parentNode.childNodes.end(), nodeIt->childNodes.begin(), nodeIt->childNodes.end());

				}

				parentNode.childNodes.erase(parentNode.childNodes.begin() + i);
				toDelete = 2;

			}

			return toDelete;

		}

	}

	return 0;

}


void BLUSH::DrawTreeData(std::vector<BLUSHNode>& rootNodes, int initialX, int initialY) {

	static ImVec2 winPos = ImVec2(initialX, initialY);
	static ImVec2 winSize = ImVec2(screenWidth * DATA_MENU_MULTIPLIER, screenHeight * 0.8f);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree Data", NULL, flags);

	for (size_t i = 0; i < rootNodes.size(); i++) { DrawTreeChildData(rootNodes[i]); }

	if (nodeToggle == NODE_TOGGLE::SET_OPEN) { nodeToggle = NODE_TOGGLE::CLOSE; }
	if (nodeToggle == NODE_TOGGLE::SET_CLOSE) { nodeToggle = NODE_TOGGLE::OPEN; }

	ImGui::End();
	HandlePendingAction();

}


void BLUSH::DrawTreeChildData(BLUSHNode& node) {

	bool wasClicked = false;
	std::string nameBuffer = ImGuiBase::MakeImGuiName(node.nodeName, node.nodeID);
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::Button(ImGuiBase::MakeImGuiName("^", node.nodeID).c_str())) { moveNodeIndex = node.nodeID; pendingAction = PENDING_ACTION::MOVE_UP; } ImGui::SameLine();
	if (ImGui::Button(ImGuiBase::MakeImGuiName("v", node.nodeID).c_str())) { moveNodeIndex = node.nodeID; pendingAction = PENDING_ACTION::MOVE_DOWN; }ImGui::SameLine();

	if (pendingAction == PENDING_ACTION::MOVE) {

		if (ImGui::Button(ImGuiBase::MakeImGuiName("Select", node.nodeID).c_str())) { selectedNodeAux = node.nodeID; }
		ImGui::SameLine();

	}

	if (nodeToggle == NODE_TOGGLE::SET_OPEN) { ImGui::SetNextItemOpen(true); }
	if (nodeToggle == NODE_TOGGLE::SET_CLOSE) { ImGui::SetNextItemOpen(false); } // This ain't going to close ALL nodes, only root, but ImGui can't close a node without drawing it, which instantly opens the parent
	if (newNodeIndex != -1 && ContainsNode(newNodeIndex)) { ImGui::SetNextItemOpen(true); }

	if (node.childNodes.size() == 0) { nodeFlags |= ImGuiTreeNodeFlags_Leaf; }
	bool open = ImGui::TreeNodeEx(nameBuffer.c_str(), nodeFlags);

	if (ImGui::IsItemClicked()) { selectedNode = node.nodeID; wasClicked = true; }

	if (selectedNode == node.nodeID) {

		ImGui::SameLine();
		ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_AutoSelectAll;
		ImGui::SetNextItemWidth(screenWidth * DATA_MENU_MULTIPLIER * 0.5f);
		ImGui::InputText(ImGuiBase::MakeImGuiName("##nodeNameEdit", node.nodeID).c_str(), node.nameBuffer, sizeof(node.nameBuffer), textFlags);
		if (newNodeIndex == node.nodeID) { ImGui::SetKeyboardFocusHere(); newNodeIndex = -1; }
		if (wasClicked) { ImGui::SetKeyboardFocusHere(); }

	}

	if (open) {

		for (size_t i = 0; i < node.childNodes.size(); i++) { DrawTreeChildData(node.childNodes[i]); }
		ImGui::TreePop();

	}

	node.nodeName = node.nameBuffer;

}


void BLUSH::DrawTreeDataEditingMenu(std::string& name, std::vector<BLUSHNode>& rootNodes, int sizeX) {

	std::pair<BLUSHNode*, BLUSHNode*> nodeAndParentRef(nullptr, nullptr);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(ImVec2(sizeX, screenHeight * 0.015f));
	ImGui::SetNextWindowSize(ImVec2(screenWidth * DATA_MENU_MULTIPLIER, screenHeight * 0.085f));
	ImGui::Begin("##DataEditor", NULL, flags);

	ImGui::SetNextItemWidth(screenWidth * DATA_MENU_MULTIPLIER * 0.5f);
	ImGui::InputText("##treeNameEditor", treeNameBuffer, sizeof(treeNameBuffer));
	name = treeNameBuffer; ImGui::SameLine();

	if (ImGui::Button("New Root Node")) {

		BLUSHNode newNode;
		selectedNode = newNode.nodeID;
		newNodeIndex = selectedNode;
		rootNodes.push_back(newNode);

	}

	ImGui::SameLine();

	if (ImGui::Button("New Child Node") && selectedNode != -1) {

		nodeAndParentRef = GetNodeAndParentByID(selectedNode);

		if (nodeAndParentRef.first != nullptr) {

			BLUSHNode newNode;
			selectedNode = newNode.nodeID;
			newNodeIndex = selectedNode;
			nodeAndParentRef.first->childNodes.push_back(newNode);

		}

	} ImGui::Separator();

	if (ImGui::Button(nodeToggle == NODE_TOGGLE::OPEN ? "Open All Nodes" : "Close All Nodes")) {

		nodeToggle = nodeToggle == NODE_TOGGLE::OPEN ? NODE_TOGGLE::SET_OPEN : NODE_TOGGLE::SET_CLOSE;

	} ImGui::SameLine();

	ImGui::Checkbox("Include Child Nodes", &includeChildNodes); ImGui::SameLine();

	if (ImGui::Button("Delete Node") && selectedNode != -1) {

		DeleteNodeByID(selectedNode, includeChildNodes);
		selectedNode = -1;

	} ImGui::SameLine();

	if (ImGui::Button("Move Node to Root") && selectedNode != -1) {

		MoveNode(selectedNode, -1);
		selectedNode = -1;

	} ImGui::SameLine();

	if (ImGui::Button("Move Node")) { pendingAction = PENDING_ACTION::MOVE; }

	ImGui::End();

}


void BLUSH::HandlePendingAction() {

	bool reset = false;

	switch (pendingAction) {

	case PENDING_ACTION::MOVE:

		if (selectedNode != -1 && selectedNodeAux != -1) {

			MoveNode(selectedNode, selectedNodeAux);
			selectedNode = -1;
			selectedNodeAux = -1;
			pendingAction = PENDING_ACTION::NONE;

		} break;

	case PENDING_ACTION::MOVE_UP:

		if (moveNodeIndex != -1) {

			ReorderNode(moveNodeIndex);
			moveNodeIndex = -1;
			pendingAction = PENDING_ACTION::NONE;

		} break;

	case PENDING_ACTION::MOVE_DOWN:

		if (moveNodeIndex != -1) {

			ReorderNode(moveNodeIndex);
			moveNodeIndex = -1;
			pendingAction = PENDING_ACTION::NONE;

		} break;

	}

}


void BLUSH::MoveNode(int movedId, int newParentId) {

	if (newParentId == movedId) { return; }

	BLUSHTree* treeRef = nullptr;
	std::pair<BLUSHNode*, BLUSHNode*> nodeAndParentRef = GetNodeAndParentByID(movedId);
	std::pair<BLUSHNode*, BLUSHNode*> newParentRef(nullptr, nullptr);
	int originalParentRefId = -1;

	if (newParentId != -1) { newParentRef = GetNodeAndParentByID(newParentId); }
	if (nodeAndParentRef.second != nullptr) { originalParentRefId = nodeAndParentRef.second->nodeID; }

	if (nodeAndParentRef.first != nullptr) {

		std::vector<BLUSHNode> childNodesRef;

		if (!includeChildNodes) {

			childNodesRef = nodeAndParentRef.first->childNodes;
			nodeAndParentRef.first->childNodes.clear();

		}

		BLUSHNode nodecopy = *nodeAndParentRef.first;
		DeleteNodeByID(nodeAndParentRef.first->nodeID, true);

		if (!includeChildNodes) {

			if (originalParentRefId != -1) {

				std::pair<BLUSHNode*, BLUSHNode*> originalParentRef = GetNodeAndParentByID(originalParentRefId); // We get it after vector relocated memory
				if (originalParentRef.first != nullptr) {

					originalParentRef.first->childNodes.insert(originalParentRef.first->childNodes.end(), childNodesRef.begin(), childNodesRef.end());

				}

			}

			else {

				treeRef = GetTree(currentTreeIndex);
				if (treeRef != nullptr) { treeRef->rootNodes.insert(treeRef->rootNodes.end(), childNodesRef.begin(), childNodesRef.end()); }

			}

		}

		if (newParentRef.first != nullptr) { newParentRef.first->childNodes.push_back(nodecopy); }
		else {

			treeRef = GetTree(currentTreeIndex);
			if (treeRef != nullptr) { treeRef->rootNodes.push_back(nodecopy); }

		}

	}

}


void BLUSH::SaveDataTrees() {

	if (trees.size() <= 0) { return; }

	fileHandle.reset();
	if (fileHandle.first_child() == nullptr) { fileHandle.append_child("BLUSH_Trees"); }
	pugi::xml_node fileNode = fileHandle.first_child();

	for (size_t i = 0; i < trees.size(); i++) {

		BLUSHTree& treeAux = trees[i];
		int nodeIndex = 0;
		pugi::xml_node baseNode = fileNode.append_child(("Tree" + std::to_string(i)).c_str());
		baseNode.append_attribute("name") = treeAux.treeName.c_str();

		for (size_t j = 0; j < treeAux.rootNodes.size(); j++) { SaveDataTreeChildNodes(treeAux.rootNodes[j], baseNode, nodeIndex); }

	}

	std::string localDirectory = SAVE_DATA_FILE;
	fileHandle.save_file((localDirectory).c_str());

}


void BLUSH::SaveDataTreeChildNodes(const BLUSHNode& node, pugi::xml_node& xmlNode, int index) {

	pugi::xml_node childNode = xmlNode.append_child(("Node" + std::to_string(index)).c_str());
	childNode.append_attribute("nodeValue") = node.nameBuffer;
	index++;

	for (size_t k = 0; k < node.childNodes.size(); k++) { SaveDataTreeChildNodes(node.childNodes[k], childNode, index); }

}


void BLUSH::LoadDataTrees() {

	pugi::xml_parse_result treeList = fileHandle.load_file(SAVE_DATA_FILE);
	pugi::xml_node baseNode;

	if (treeList != NULL) {

		for (baseNode = fileHandle.first_child().first_child(); baseNode; baseNode = baseNode.next_sibling()) {

			BLUSHTree newTree(baseNode.attribute("name").as_string());

			for (pugi::xml_node treeNode = baseNode.first_child(); treeNode; treeNode = treeNode.next_sibling()) {

				newTree.rootNodes.push_back(LoadDataTreeChildNodes(treeNode));

			}

			trees.push_back(newTree);

		}

	}

	for (size_t i = 0; i < trees.size(); i++) { // To prevent tree name from resetting on frame 1

		if (i == currentTreeIndex) { strcpy_s(treeNameBuffer, sizeof(treeNameBuffer), trees[i].treeName.c_str()); }

	}

}

BLUSHNode BLUSH::LoadDataTreeChildNodes(pugi::xml_node& treeNode) {

	BLUSHNode newNode(treeNode.attribute("nodeValue").as_string());

	for (pugi::xml_node childNode = treeNode.first_child(); childNode; childNode = childNode.next_sibling()) {

		BLUSHNode newChildNode = LoadDataTreeChildNodes(childNode);
		newNode.childNodes.push_back(newChildNode);

	}

	return newNode;

}

