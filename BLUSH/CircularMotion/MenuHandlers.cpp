#include "CircularMotion.h"

#pragma region FileSystem

void FileSystem::OpenFileSystemLoadMenu() { OpenMenu(loaderMenu); }

void FileSystem::DrawFileSystemMenus() {

	menuClosed = menuOpen;

	DrawFileSystemLoadMenu();

	UpdateMenuState();

}

#pragma endregion

#pragma region TagSystem

void TagCluster::OpenGlobalEditorMenu() { OpenMenu(globalTagEditor); }

void TagCluster::OpenLocalEditorMenu(TagData* data, TagData* prior, TagData* next, Texture* dataImage, std::string elementName) {
	
	OpenMenu(localTagEditor);
	SetNewMenuData(data, prior, next, dataImage, elementName);

}

void TagCluster::OpenOptionsMenu() { OpenMenu(optionsMenu); }

void TagCluster::DrawTagMenus() {

	menuClosed = menuOpen;

	if (menuData == nullptr || openedGlobalEditorFromLocal) {
		
		DrawGlobalTagEditor();
	
		if (menuData != nullptr && (!openedGlobalEditorFromLocal || (menuOpen.count(globalTagEditor) > 0 && !menuOpen[globalTagEditor]))) {

			openedGlobalEditorFromLocal = false;
			OpenLocalEditorMenu(menuData, priorMenuData, nextMenuData, menuDataImage, CircularMotion::GetStringBeforeLastOfString(localTagEditor, " - "));
			if (menuData != nullptr) { UpdateTagDataValues(*menuData); }
			// We update local menu data, but all other external data will need user to update by checking DidAnyMenuCloseThisFrame or so

		}

	}

	else {
		
		DrawLocalTagEditor();
		if (menuOpen.count(localTagEditor) > 0 && !menuOpen[localTagEditor]) {
			
			menuData = nullptr;
			priorMenuData = nullptr;
			nextMenuData = nullptr;
			menuDataImage = nullptr;

		}

		if (openedGlobalEditorFromLocal) { ImGui::CloseCurrentPopup(); OpenGlobalEditorMenu(); }

	}

	DrawClusterOptionsMenu();

	UpdateMenuState();
	if (menuClosed.count(optionsMenu) > 0 && menuClosed[optionsMenu]) { valueBuffer = valueDefault; }

}

#pragma endregion



