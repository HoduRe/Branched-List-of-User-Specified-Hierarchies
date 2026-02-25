#include "BLUSH.h"

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


}

void BLUSH::LoadDataTrees() {


}

