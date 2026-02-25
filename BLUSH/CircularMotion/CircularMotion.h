#ifndef __CircularMotion__
#define __CircularMotion__

#include "CircularMotionStructs.h"

namespace CircularMotion {

	// Generic functions ----------------------------------------------------
	void Clamp(float& value, float floor, float ceiling);
	float Lerp(float min, float max, float alpha);

	cmf2 PositionOfTextureAtCenterOfPoint(cmf2 point, float width, float height, bool positiveYAxis);

	bool IsColorLighter(float r, float g, float b, float averageThreshold);

	bool IsNumber(char c);
	bool IsNumber(std::string baseString);
	bool AreTwoFirstCharsANumber(std::string baseString, size_t startingIndex);
	bool IsLetter(char c);
	int GetNumberFromMixedString(std::string baseString, size_t firstDigitIndex);
	size_t GetIndexOfLastStringInsideString(std::string baseString, std::string substring);
	int GetIntFromString(std::string baseString, int initialIndex, int finalIndex);
	std::string GetStringBeforeString(std::string baseString, std::string thresholdString);
	std::string GetStringBeforeLastOfString(std::string baseString, std::string thresholdString);
	std::string GetStringAfterString(std::string baseString, std::string thresholdString);
	std::string GetStringAfterLastOfString(std::string baseString, std::string thresholdString);
	std::string GetStringWithoutString(std::string baseString, std::string excludingString);
	std::string GetStringLowerCase(const std::string& baseString);
	std::string GetStringUpperCase(const std::string& baseString);
	bool IsStringOrderEarlierWindowsLike(std::string string1, std::string string2);
	bool IsImageFormat(std::string filename);

	// Collision functions ----------------------------------------------------

	bool IsPointInsideQuad(cmf2 point, cmf4 quadXYWH);

}

class ImGuiBase {

public:

	ImGuiBase(cmi2* _windowSize);

	bool DidMenuCloseThisFrame(std::string menuName);
	bool DidAnyMenuCloseThisFrame();

	static void AddErrorMessage(std::string message);
	static void DrawErrorMessage(); // GUIDELINES: just call this once every frame. This one doesn't need an OpenPopup

	static std::string MakeImGuiName(std::string elementName, int tagID);
	static bool DrawImGuiDeletePopup(std::string popupName);
	static bool ImGuiButtonColored(std::string buttonName, ImVec4 color, int buttonID);
	static void ImGuiTextWrappedColored(std::string textName, ImVec4 color);

protected:

	void OpenMenu(std::string menuName);
	bool StartPopup(const std::string& popupName);
	void UpdateMenuState();

protected:

	cmf2 popupSize;
	static cmi2* windowSize;
	std::map<const std::string, bool> menuOpen;
	std::map<const std::string, bool> menuClosed; // Data is NOT reliable during the menu drawing function
	const static ImGuiInputTextFlags inputTextFlags;

private:

	static std::vector<std::string> errors;

};

#pragma region FileSystemFunctionality

class FileSystem : public ImGuiBase {

public:

	FileSystem(cmi2* _windowSize);

	void OpenFileSystemLoadMenu();
	void DrawFileSystemMenus();

	bool DrawTabWithoutMenuToGetValidPath(std::string inputName, std::string buttonName, bool needsFile, bool needsDirectory, char* directoryBuffer, int arraySize);

	std::string GetFilenameFromPath(std::string globalFilePath);
	std::string GetDirectoryFromPath(std::string globalFilePath, bool includeBackslashes = true);
	bool RenameFile(std::string globalFilePath, std::string newName);
	bool CreateDirectory(std::string globalDirectoryPath); // TODO: if windows.h is included it has a header definition that replaces this function with something else all together
	std::set<std::string> GetFiles(std::string globalDirectory, bool includeChildFolders);

	bool PathExists(std::string globalPath);
	bool IsDirectory(std::string globalPath);
	bool IsFile(std::string globalPath);

	Texture LoadTexture(std::string filepath);
	TextureAdv LoadTextureAdv(std::string filepath);
	void UnloadTexture(GLuint handle);

private:

	void DrawFileSystemLoadMenu();
	bool IsExistingPathAFile(std::string globalPath);
	void CreateTexture(TextureAdv& newTexture, GLenum _type, void* pixels);

public:

	std::set<std::string> files;
	const std::string loaderMenu;

private:

	char directory[512];
	bool childFolders;

};

#pragma endregion

#pragma region TagFunctionality

struct TagData { // TagData is the structure for elements to hold a subset of tags

	TagData();
	void ResetData();

	std::map<int, std::map<int, int>> tags;

};

class TagCluster : public ImGuiBase { // TagCluster is the functionality to manage a set of tags

private:

	struct ValueClusterData {

		int defaultValue;
		char name[128];
		char alias[16];

	};

	struct ValueFilterData {

		int value;
		int comparativeValue;
		ComparativeButton button;

	};

	struct TagClusterData {

		TagClusterData();
		TagClusterData(std::string _name, cmf4 _color, bool _selected = false);

		bool selected;
		char name[128];
		char alias[16];
		cmf4 color;
		std::map<int, ValueFilterData> values;

	};

public:

	TagCluster(cmi2* _windowSize);
	void OpenGlobalEditorMenu();
	void OpenLocalEditorMenu(TagData* data, TagData* prior, TagData* next, Texture* dataImage, std::string elementName);
	void OpenOptionsMenu();
	void DrawTagMenus();
	void DrawTagSelectionGridWithoutMenu(int columns);

	std::string GetTagName(int tagID);
	std::string GetTagAlias(int tagID);
	const TagClusterData* GetTagData(int tagID);
	int GetTagID(std::string tagName);
	//	int GetTagID(Quaternion color);
	//	ImVec4 GetTagColor(int tagInt);
	TagData GetTagDataFromName(std::string filename);
	int GetIfMenuDataChanged(bool cleanData = true);
	void SetNewMenuData(TagData* data, TagData* prior, TagData* next, Texture* dataImage, std::string elementName);

	std::string GetValueName(int valueID);
	int GetValueID(std::string valueName);
	std::string GetPureName(std::string rawName);
	std::string GetAliasesString(std::string rawName);
	std::string ComposeNameWithoutTermination(const std::string& pureName, TagData& data);
	std::string GetTermination(int fileRepeatedIndex, int totalFilesWithSameName);
	std::string GetTerminationStringFromFileName(std::string name, bool spaceBeforeName);

	void ResetClusterData();
	void UpdateTagDataValues(TagData& data);
	bool AreTagFiltersBeingUsed();
	bool AreValueFiltersBeingUsed();
	bool AreFiltersBeingUsed();
	bool ShouldDrawData(TagData* data);
	void SaveDataXML(pugi::xml_node& firstNode, bool resetClusterData);
	void LoadDataXML(pugi::xml_node& node); // GUIDELINES: always load cluster data before any tag data
	void SaveDataXML(pugi::xml_node& uniquelyNamedParentNode, TagData& data, bool resetTagData);
	void LoadDataXML(pugi::xml_node& uniquelyNamedParentNode, TagData& data);

private:

	bool DrawGlobalTagEditor();
	void DrawLocalTagEditor();
	void DrawClusterOptionsMenu();

	void AddTag(TagClusterData newData);
	void AddValue(std::string name, int value);
	void DeleteValue(int valueID);
	TagData GetTagDataTagFromName(std::string filename);
	TagData GetTagDataValuesFromName(std::string filename);
	void AddMissingTagClusterDataValues(std::map<int, ValueFilterData>& dataValues);
	void RemoveObsoleteTagClusterDataValues(std::map<int, ValueFilterData>& dataValues);

public:

	const std::string globalTagEditor, optionsMenu, localTagEditor;

private:

	float colorBuffer[3];
	bool openedGlobalEditorFromLocal;
	char nameBuffer[128], valueNameBuffer[128];
	int valueBuffer, valueDefault;
	std::map<int, TagClusterData> tags;
	std::map<int, ValueClusterData> values;

	// --------- Nomenclature variables -----------------------

	char nameTermination[16], nameSeparator[16], systemTermination[16];
	int useTagsForNomenclature, startingX, menuDataChanged;
	bool terminateAllElements, useSeparator, includeDefaultScores, includeDefaultFinalScores, rewriteOldTermination, oldTerminationIsNumberOnly;

	// --------------------------------------------------------

	std::string currentElementName;
	TagData* menuData, *priorMenuData, *nextMenuData;
	Texture* menuDataImage;

};

#pragma endregion

#endif