#include "BLUSH.h"

BLUSHNode::BLUSHNode() {}

BLUSHNode::~BLUSHNode() { parentNode = nullptr; }

BLUSHTree::BLUSHTree() {}

BLUSHTree::~BLUSHTree() {}

BLUSH::BLUSH(SDL_Window* _window, int _screenWidth, int _screenHeight) : windowRef(_window), screenWidth(_screenWidth), screenHeight(_screenHeight) {

	LoadDataTrees();

}

BLUSH::~BLUSH() {

	trees.clear();

}

bool BLUSH::Update() {

	bool ret = true;
	static ImVec2 winSize = ImVec2(screenWidth * 0.2f, screenHeight * 0.8f);
	static ImVec2 winPos = ImVec2(screenWidth * 0.03125f, screenHeight * 0.055f);
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(winPos);
	ImGui::SetNextWindowSize(winSize);
	ImGui::Begin("Tree List", NULL, flags);

	for (size_t i = 0; i < trees.size(); i++) {

		if (trees[i].treeName == currentTreeName) {

			DrawTreeData(winPos.x + winSize.x, winPos.y);

			i = trees.size();

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

	if (trees.size() <= 0) { return; } // TODO: give feedback for this?

	fileHandle.reset();
	if (fileHandle.first_child() == nullptr) { fileHandle.append_child("BLUSH_Trees"); }
	pugi::xml_node fileNode = fileHandle.first_child();

	for (size_t i = 0; i < trees.size(); i++) {

		BLUSHTree& treeAux = trees[i];
		int nodeIndex = 0;
		pugi::xml_node baseNode = fileNode.append_child(("Tree" + std::to_string(i)).c_str());
		fileNode.append_attribute("name") = treeAux.treeName.c_str();

		for (size_t j = 0; j < treeAux.rootNodes.size(); j++) { SaveDataTreeChildNodes(treeAux.rootNodes[j], baseNode, nodeIndex); }

	}

	////

	/*fileNode.append_attribute("columns") = defaults.columnsPerPage;
	fileNode.append_attribute("rows") = defaults.rowsPerPage;
	fileNode.append_attribute("freeComposition") = defaults.freeCompositionDefault;
	fileNode.append_attribute("includeRepeatedFiles") = defaults.includeRepeatedFiles;
	fileNode.append_attribute("defaultRarity") = defaults.defaultRarity.c_str();
	rarityNode = fileHandle.append_child("AlbumRarityData");

	for (std::map<std::string, GLuint>::iterator it = rarityData.begin(); it != rarityData.end(); it++) {

		pugi::xml_node rarityChildNode = rarityNode.append_child("Rarity");
		rarityChildNode.append_attribute("name") = it->first.c_str();
		rarityChildNode.append_attribute("borderName") = GetBorderFileNameFromTextureHandle(it->second).c_str();

	}

	for (std::vector<Page>::iterator pageIt = albumPages.begin(); pageIt != albumPages.end(); pageIt++) {

		pageNode = fileNode.append_child("page");

		pageNode.append_attribute("columns") = pageIt->GetColumns();
		pageNode.append_attribute("rows") = pageIt->GetRows();
		pageNode.append_attribute("freeComposition") = pageIt->freeComposition;

		pageIt->SaveCardData(pageNode);

	}

	if (newAlbum) { availableAlbums.push_back(_albumName); }

	std::string localDirectory = GetAppPath() + SAVE_DATA_LOCAL_DIRECTORY + _albumName + ".txt";
	albumFileHandle.save_file((localDirectory).c_str());*/

}


void BLUSH::SaveDataTreeChildNodes(BLUSHNode* node, pugi::xml_node& xmlNode, int index) {

	pugi::xml_node childNode = xmlNode.append_child(("Node" + std::to_string(index)).c_str());
	childNode.append_attribute("nodeValue") = node->nodeName.c_str();
	index++;

	for (size_t k = 0; k < node->childNodes.size(); k++) { SaveDataTreeChildNodes(node->childNodes[k], childNode, index); }

}


void BLUSH::LoadDataTrees() {


}

