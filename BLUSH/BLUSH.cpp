#include "BLUSH.h"

int BLUSHNode::idGenerator = 0;

BLUSHNode::BLUSHNode(std::string _name) : nodeName(_name), childNodes(), nodeID(idGenerator++) {}

BLUSHNode::~BLUSHNode() {}

BLUSHTree::BLUSHTree() : treeName("NewTree"), rootNodes() {}

BLUSHTree::BLUSHTree(std::string _name) : treeName(_name), rootNodes() {}

BLUSHTree::~BLUSHTree() {}

BLUSH::BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight) : windowRef(_window), screenWidth(_screenWidth), screenHeight(_screenHeight),
currentTreeIndex(0), trees(), fileHandle(), treeNameBuffer(""), pendingAction(PENDING_ACTION::NONE), includeChildNodes(false), selectedNode(-1),
selectedNodeAux(-1), nodeToggle(NODE_TOGGLE::SET_OPEN) {

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


BLUSHNode* BLUSH::GetNodeByID(int id) {

	if (!currentTreeIndex < trees.size()) { return nullptr; }

	for (size_t i = 0; i < trees[currentTreeIndex].rootNodes.size(); i++) {

		BLUSHNode* nodeAux = GetChildNodeByID(trees[currentTreeIndex].rootNodes[i], id);
		if (nodeAux != nullptr) { return nodeAux; }

	}

	return nullptr;

}


BLUSHNode* BLUSH::GetChildNodeByID(BLUSHNode& parentNode, int id) {

	if (parentNode.nodeID == id) { return &parentNode; }

	for (size_t i = 0; i < parentNode.childNodes.size(); i++) {

		BLUSHNode* nodeAux = GetChildNodeByID(parentNode.childNodes[i], id);
		if (nodeAux != nullptr) { return nodeAux; }

	}

	return nullptr;

}


void BLUSH::DrawTreeData(std::vector<BLUSHNode>& rootNodes, int initialX, int initialY) {

	static ImVec2 winPos = ImVec2(initialX, initialY);
	static ImVec2 winSize = ImVec2(screenWidth * DATA_MENU_MULTIPLIER, screenHeight * 0.8f);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree Data", NULL, flags);

	for (size_t i = 0; i < rootNodes.size(); i++) { DrawTreeChildData(rootNodes[i], 0); }

	if (nodeToggle == NODE_TOGGLE::SET_OPEN) { nodeToggle = NODE_TOGGLE::CLOSE; }
	if (nodeToggle == NODE_TOGGLE::SET_CLOSE) { nodeToggle = NODE_TOGGLE::OPEN; }

	ImGui::End();
	HandlePendingAction();

}


void BLUSH::DrawTreeChildData(const BLUSHNode& node, int level) {

	if (pendingAction > PENDING_ACTION::NONE) {

		if (ImGui::Button("Select")) { selectedNode = node.nodeID; }
		ImGui::SameLine();

	}

	if (nodeToggle == NODE_TOGGLE::SET_OPEN) { ImGui::SetNextItemOpen(true); }
	if (nodeToggle == NODE_TOGGLE::SET_CLOSE) { ImGui::SetNextItemOpen(false); }

	if (ImGui::TreeNode(ImGuiBase::MakeImGuiName(node.nodeName, node.nodeID).c_str())) {

		for (size_t i = 0; i < node.childNodes.size(); i++) { DrawTreeChildData(node.childNodes[i], level + 1); }
		ImGui::TreePop();

	}

}


void BLUSH::DrawTreeDataEditingMenu(std::string& name, std::vector<BLUSHNode>& rootNodes, int sizeX) {

	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(ImVec2(sizeX, screenHeight * 0.015f));
	ImGui::SetNextWindowSize(ImVec2(screenWidth * DATA_MENU_MULTIPLIER, screenHeight * 0.085f));
	ImGui::Begin("##DataEditor", NULL, flags);

	ImGui::SetNextItemWidth(screenWidth * DATA_MENU_MULTIPLIER * 0.5f);
	ImGui::InputText("##treeNameEditor", treeNameBuffer, sizeof(treeNameBuffer));
	name = treeNameBuffer; ImGui::SameLine();

	if (ImGui::Button("New Root Node")) { rootNodes.push_back(BLUSHNode()); } ImGui::SameLine();
	if (ImGui::Button("New Child Node")) { pendingAction = PENDING_ACTION::CREATE; } ImGui::Separator();

	if (ImGui::Button(nodeToggle == NODE_TOGGLE::OPEN ? "Open All Nodes" : "Close All Nodes")) {

		nodeToggle = nodeToggle == NODE_TOGGLE::OPEN ? NODE_TOGGLE::SET_OPEN : NODE_TOGGLE::SET_CLOSE;

	} ImGui::SameLine();

	ImGui::Checkbox("Include Child Nodes", &includeChildNodes); ImGui::SameLine();
	if (ImGui::Button("Delete Node")) { pendingAction = PENDING_ACTION::DELETE; } ImGui::SameLine();
	if (ImGui::Button("Move Node to Root")) { pendingAction = PENDING_ACTION::MOVE_TO_ROOT; } ImGui::SameLine();
	if (ImGui::Button("Move Node")) { pendingAction = PENDING_ACTION::MOVE; }
	ImGui::End();

}


void BLUSH::HandlePendingAction() {

	if (selectedNode != -1) {

		switch (pendingAction) {

		case PENDING_ACTION::CREATE:

			break;

		case PENDING_ACTION::DELETE:

			break;

		case PENDING_ACTION::MOVE_TO_ROOT:

			break;

		case PENDING_ACTION::MOVE:

			break;

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
	childNode.append_attribute("nodeValue") = node.nodeName.c_str();
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

