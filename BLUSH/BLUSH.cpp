#include "BLUSH.h"

BLUSHNode::BLUSHNode() : nodeName("NewNode"), childNodes(), parentNode(nullptr) {}

BLUSHNode::BLUSHNode(std::string _name, BLUSHNode* _parent) : nodeName(_name), childNodes(), parentNode(_parent) {}

BLUSHNode::~BLUSHNode() { parentNode = nullptr; }

BLUSHTree::BLUSHTree() : treeName("NewTree"), rootNodes() {}

BLUSHTree::BLUSHTree(std::string _name) : treeName(_name), rootNodes() {}

BLUSHTree::~BLUSHTree() {}

BLUSH::BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight) : windowRef(_window), screenWidth(_screenWidth), screenHeight(_screenHeight),
currentTreeIndex(0), trees(), fileHandle() {

	LoadDataTrees();

}

BLUSH::~BLUSH() {

	trees.clear();

}

bool BLUSH::Update() {

	bool ret = true;
	static ImVec2 winSize = ImVec2(screenWidth * 0.3f, screenHeight * 0.8f);
	static ImVec2 winPos = ImVec2(screenWidth * 0.05f, screenHeight * 0.1f);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree List", NULL, flags);

	if (ImGui::Button("Create New Tree")) {

		trees.push_back(BLUSHTree());

	}

	int upTree = -1, downTree = -1;

	for (size_t i = 0; i < trees.size(); i++) {

		ImGui::NewLine();
		if (ImGui::Button(ImGuiBase::MakeImGuiName("^", i).c_str())) { upTree = i; } ImGui::SameLine();
		if (ImGui::Button(ImGuiBase::MakeImGuiName("v", i).c_str())) { downTree = i; } ImGui::SameLine();
		if (ImGui::Selectable(ImGuiBase::MakeImGuiName(trees[i].treeName, i).c_str(), currentTreeIndex == i)) { currentTreeIndex = i; }

		if (i == currentTreeIndex) {

			DrawTreeData(winPos.x + winSize.x, winPos.y);

		}

	}

	ImGui::End();

	return ret;

}


void BLUSH::DrawTreeData(int initialX, int initialY) {

	static ImVec2 winSize = ImVec2(screenWidth * 0.6f, screenHeight * 0.8f);
	static ImVec2 winPos = ImVec2(initialX, initialY);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree Data", NULL, flags);



	ImGui::End();

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


void BLUSH::SaveDataTreeChildNodes(BLUSHNode* node, pugi::xml_node& xmlNode, int index) {

	pugi::xml_node childNode = xmlNode.append_child(("Node" + std::to_string(index)).c_str());
	childNode.append_attribute("nodeValue") = node->nodeName.c_str();
	index++;

	for (size_t k = 0; k < node->childNodes.size(); k++) { SaveDataTreeChildNodes(node->childNodes[k], childNode, index); }

}


void BLUSH::LoadDataTrees() {

	pugi::xml_parse_result treeList = fileHandle.load_file(SAVE_DATA_FILE);
	pugi::xml_node baseNode;

	if (treeList != NULL) {

		for (baseNode = fileHandle.first_child().first_child(); baseNode; baseNode = baseNode.next_sibling()) {

			BLUSHTree newTree(baseNode.attribute("name").as_string());

			for (pugi::xml_node treeNode = baseNode.first_child(); treeNode; treeNode = treeNode.next_sibling()) {

				BLUSHNode newChildNode = LoadDataTreeChildNodes(nullptr, treeNode);
				newTree.rootNodes.push_back(&newChildNode);

			}

			trees.push_back(newTree);

		}

	}

}

BLUSHNode BLUSH::LoadDataTreeChildNodes(BLUSHNode* parentNode, pugi::xml_node& treeNode) {

	BLUSHNode newNode(treeNode.attribute("nodeValue").as_string(), parentNode);

	for (pugi::xml_node childNode = treeNode.first_child(); childNode; childNode = childNode.next_sibling()) {

		BLUSHNode newChildNode = LoadDataTreeChildNodes(&newNode, childNode);
		newNode.childNodes.push_back(&newChildNode);

	}

	return newNode;

}

