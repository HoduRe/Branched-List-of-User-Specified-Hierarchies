#include "CircularMotion.h"
#include <filesystem>

FileSystem::FileSystem(cmi2* _screenSize) : ImGuiBase(_screenSize), directory(), childFolders(true), loaderMenu("File Manager") {}

void FileSystem::DrawFileSystemLoadMenu() {

	bool showFiles = true;
	bool newDirectory = false;

	if (StartPopup(loaderMenu)) {

		ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::NewLine();
		ImGui::Text("Load files under a directory:"); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
		ImGui::SetNextItemWidth(popupSize.a * 0.6f);
		if (ImGui::InputText("###LoadingDirectory", directory, IM_ARRAYSIZE(directory), inputTextFlags)) { newDirectory = true; }

		ImGui::NewLine(); ImGui::Checkbox("Check child folders", &childFolders);

		ImGui::NewLine(); if (ImGui::Button("Load files")) { newDirectory = true; } ImGui::SameLine();
		if (ImGui::Button("Show files")) { ImGui::OpenPopup("File List"); } ImGui::SameLine();
		if (ImGui::Button("Unload all files")) { files.clear(); }

		if (newDirectory) {

			std::set<std::string> newFiles = GetFiles(directory, childFolders);
			files.insert(newFiles.begin(), newFiles.end());
			memset(directory, 0, sizeof(directory));

		}

		ImGui::SetNextWindowPos(ImVec2(float(windowSize->a) * 0.5f - (popupSize.a * 0.4f), float(windowSize->b) * 0.5f - (popupSize.b * 0.4f)));
		ImGui::SetNextWindowSize(ImVec2(popupSize.a * 0.8f, popupSize.b * 0.8f));

		if (ImGui::BeginPopupModal("File List", &showFiles, popupFlags)) {

			for (std::set<std::string>::iterator fileIt = files.begin(); fileIt != files.end(); fileIt++) { ImGui::NewLine(); ImGui::Text(fileIt->c_str()); }
			ImGui::EndPopup();

		}

		ImGui::EndPopup();

	}

}


bool FileSystem::DrawTabWithoutMenuToGetValidPath(std::string inputName, std::string buttonName, bool needsFile, bool needsDirectory, char* directoryBuffer, int arraySize) {

	bool returnValidPath = false;

	bool addedPath = false;
	if (ImGui::InputText(inputName.c_str(), directoryBuffer, arraySize, inputTextFlags)) { addedPath = true; } ImGui::SameLine();
	if (ImGui::Button(buttonName.c_str())) { addedPath = true; }

	if (!((needsFile && IsFile(directoryBuffer)) || (needsDirectory && IsDirectory(directoryBuffer)))) {

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Current path isn't valid.");
		addedPath = false;

	}

	if (addedPath) { returnValidPath = true; }

	return returnValidPath;

}



std::string FileSystem::GetFilenameFromPath(std::string globalFilePath) {

	return PathExists(globalFilePath.c_str()) ? CircularMotion::GetStringAfterLastOfString(globalFilePath, "\\") : "";

}


std::string FileSystem::GetDirectoryFromPath(std::string globalFilePath, bool includeBackslashes) {

	return PathExists(globalFilePath.c_str()) ? globalFilePath.substr(0, CircularMotion::GetIndexOfLastStringInsideString(globalFilePath, "\\") - (includeBackslashes ? 0 : 1)) : "";

}


std::set<std::string> FileSystem::GetFiles(std::string globalDirectory, bool includeChildFolders) {

	std::set<std::string> fileVec;
	std::string fileIt;

	if (!PathExists(globalDirectory) || IsFile(globalDirectory)) { return fileVec; }

	if (includeChildFolders) {

		for (std::filesystem::recursive_directory_iterator next(globalDirectory.c_str()), end; next != end; ++next) {

			fileIt = next->path().string();
			if (IsExistingPathAFile(fileIt)) { fileVec.insert(fileIt); }

		}

	}

	else {

		for (std::filesystem::directory_iterator next(globalDirectory.c_str()), end; next != end; ++next) {

			fileIt = next->path().string();
			if (IsExistingPathAFile(fileIt)) { fileVec.insert(fileIt); }

		}

	}

	return fileVec;

}


bool FileSystem::PathExists(std::string globalPath) { return std::filesystem::exists(globalPath.c_str()); }


bool FileSystem::IsDirectory(std::string globalPath) { return (PathExists(globalPath.c_str()) && std::filesystem::is_directory(globalPath)); }


bool FileSystem::IsFile(std::string globalPath) { return (PathExists(globalPath.c_str()) && !std::filesystem::is_directory(globalPath)); }


void FileSystem::CreateTexture(TextureAdv& newTexture, GLenum _type, void* pixels) {

	glGenTextures(1, &newTexture.handle);
	glBindTexture(GL_TEXTURE_2D, newTexture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, newTexture.internalFormat, newTexture.width, newTexture.height, 0, newTexture.format, _type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

}


Texture FileSystem::LoadTexture(std::string filepath) {

	TextureAdv newTexture = LoadTextureAdv(filepath);
	return Texture(newTexture.width, newTexture.height, newTexture.handle);

}


TextureAdv FileSystem::LoadTextureAdv(std::string filepath) {

	SDL_Surface* newImage = nullptr;
	newImage = IMG_Load(filepath.c_str());

	if (newImage == nullptr) { return TextureAdv(); }

	// Convert any image to the OpenGL formats below, and never worry again (except if the surface uses palettes; then we would need SDL_ConvertSurface)
	newImage = SDL_ConvertSurfaceFormat(newImage, SDL_PIXELFORMAT_RGBA8888, 0);
	TextureAdv newTexture(newImage->w, newImage->h, GL_RGBA, GL_RGBA8, filepath);
	CreateTexture(newTexture, GL_UNSIGNED_INT_8_8_8_8, newImage->pixels);
	SDL_FreeSurface(newImage);

	return newTexture;

}


void FileSystem::UnloadTexture(GLuint handle) { glDeleteTextures(1, &handle); }


bool FileSystem::IsExistingPathAFile(std::string globalPath) { return !std::filesystem::is_directory(globalPath); }


bool FileSystem::RenameFile(std::string globalFilePath, std::string newName) { // TODO: should probably have a with \\ function, and one without, because I WILL use this thinking it concantenates the directory separator, and it doesn't

	if (PathExists(globalFilePath)) {

		std::string finalName = GetDirectoryFromPath(globalFilePath) + newName;

		if (globalFilePath.compare(finalName) == 0) { return false; }
		if (PathExists(finalName)) {
			
			AddErrorMessage("Prevented overwriting " + finalName + " when renaming " + globalFilePath);
			return false;
		
		}

		std::filesystem::rename(globalFilePath.c_str(), finalName.c_str());
		return true;

	}

	AddErrorMessage("Attempted to rename non-existing file: " + globalFilePath);
	return false;

}


bool FileSystem::CreateDirectory(std::string globalDirectoryPath) { return std::filesystem::create_directory(globalDirectoryPath.c_str()); }


