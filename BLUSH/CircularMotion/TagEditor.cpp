#include "CircularMotion.h"

TagData::TagData() : tags() {}

void TagData::ResetData() { tags.clear(); }

TagCluster::TagClusterData::TagClusterData() : selected(false), name("Unnamed Tag"), alias(), color(0.0f, 1.0f), values() {}

TagCluster::TagClusterData::TagClusterData(std::string _name, cmf4 _color, bool _selected) : selected(_selected), alias(), color(_color), values() { strcpy_s(name, _name.c_str()); }

TagCluster::TagCluster(cmi2* _windowSize) : ImGuiBase(_windowSize), tags(), valueDefault(0), nameBuffer(), colorBuffer(), valueNameBuffer(), valueBuffer(0),
values(), openedGlobalEditorFromLocal(false), localTagEditor("Tag Data Editor"), globalTagEditor("Tag Cluster Editor"), optionsMenu("Tag Options Menu"),
useTagsForNomenclature(0), menuData(nullptr), nameTermination("(vx)"), nameSeparator("."), startingX(1), terminateAllElements(true), useSeparator(true),
includeDefaultScores(true), includeDefaultFinalScores(true), rewriteOldTermination(true), oldTerminationIsNumberOnly(true), systemTermination(" (x)"),
menuDataImage(nullptr), priorMenuData(nullptr), nextMenuData(nullptr), menuDataChanged(0), currentElementName() {}

bool TagCluster::DrawGlobalTagEditor() {

	int deleteTagID = -1, deleteValueID = -1;
	bool addedNewTag = false, uniqueName = true, addedNewValue = false, uniqueValueName = true, changedSelection = false;

	if (StartPopup(globalTagEditor)) {

		ImGui::NewLine(); // NEW TAG EDITOR -----------------------------------------------------------
		ImGui::Text("New tag:"); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.4f);
		if (ImGui::InputText("###NewTagName", (char*)nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) { addedNewTag = true; }
		ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.3f);
		if (ImGui::InputFloat3(MakeImGuiName("##NewTagColor", -1).c_str(), colorBuffer)) {

			for (size_t colorIt = 0; colorIt < 3; colorIt++) { CircularMotion::Clamp(colorBuffer[colorIt], 0.0f, 1.0f); }

		}
		ImGui::SameLine(); ImGuiButtonColored("", ImVec4(colorBuffer[0], colorBuffer[1], colorBuffer[2], 1.0f), -1);
		ImGui::SameLine(); if (ImGui::Button("Add tag")) { addedNewTag = true; }

		for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { if (strcmp(tagIt->second.name, nameBuffer) == 0) { uniqueName = false; break; } }

		if (!uniqueName) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "There's already a tag with that name."); }
		else if (addedNewTag && strcmp(nameBuffer, "") > 0) {

			AddTag(TagClusterData(nameBuffer, cmf4(colorBuffer[0], colorBuffer[1], colorBuffer[2], 1.0f)));
			memset(nameBuffer, 0, sizeof(nameBuffer));
			memset(colorBuffer, 0, sizeof(colorBuffer));

		} // -----------------------------------------------------------

		ImGui::NewLine(); // TAG VALUE EDITOR -----------------------------------------------------------
		ImGui::Text("New value:"); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.5f);
		if (ImGui::InputText("###NewTagValueName", (char*)valueNameBuffer, IM_ARRAYSIZE(valueNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) { addedNewValue = true; }
		ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.2f);
		ImGui::InputInt(MakeImGuiName("##NewTagValueValue", -1).c_str(), &valueBuffer, 0.0f);
		ImGui::SameLine(); if (ImGui::Button("Add value")) { addedNewValue = true; }

		for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

			if (strcmp(valueIt->second.name, valueNameBuffer) == 0) { uniqueValueName = false; break; }

		}

		if (!uniqueValueName) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "There's already a value with that name."); ImGui::SameLine(); }
		else if (addedNewValue && strcmp(valueNameBuffer, "") > 0) {

			AddValue(valueNameBuffer, valueBuffer);
			memset(valueNameBuffer, 0, sizeof(valueNameBuffer));
			valueBuffer = valueDefault;

		}

		ImGui::NewLine(); ImGui::Columns(2, NULL, false);
		for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

			ImGui::SetNextItemWidth(popupSize.a * 0.3f);
			std::string deleteValueName = MakeImGuiName("Delete##Value", valueIt->first);
			ImGui::InputText(MakeImGuiName("##TagValueNameEdit", valueIt->first).c_str(), (char*)valueIt->second.name, IM_ARRAYSIZE(valueIt->second.name));
			ImGui::SetNextItemWidth(popupSize.a * 0.075f); ImGui::SameLine();
			ImGui::InputInt(MakeImGuiName("##TagValueValueEdit", valueIt->first).c_str(), &valueIt->second.defaultValue, 0);
			ImGui::SameLine(); if (ImGui::Button(deleteValueName.c_str())) { ImGui::OpenPopup(deleteValueName.c_str()); }
			if (DrawImGuiDeletePopup(deleteValueName)) { deleteValueID = valueIt->first; }
			ImGui::NextColumn();

		}

		ImGui::Columns(1); // -----------------------------------------------------------

		if (values.size() > 0) { ImGui::NewLine(); }
		ImGui::Separator(); ImGui::Text("Filtering + default tag menu");
		ImGui::Separator(); // TAG EDITOR -----------------------------------------------------------
		if (ImGui::Button("Select all tags")) { for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { tagIt->second.selected = true; } } ImGui::SameLine();
		if (ImGui::Button("Unselect all tags")) { for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { tagIt->second.selected = false; } } ImGui::SameLine();
		if (ImGui::Button("Toggle all tags")) { for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { tagIt->second.selected = !tagIt->second.selected; } } ImGui::SameLine();
		if (ImGui::Button("Reset tags")) { for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { tagIt->second.selected = false; } } ImGui::SameLine();
		if (ImGui::Button("Reset values")) {

			for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

				for (std::map<int, ValueFilterData>::iterator valueIt = tagIt->second.values.begin(); valueIt != tagIt->second.values.end(); valueIt++) {

					valueIt->second.button.SetState(0);
					valueIt->second.comparativeValue = values[valueIt->first].defaultValue;

				}

			}

		} ImGui::SameLine();

		if (ImGui::Button("Reset all filters")) {

			for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

				tagIt->second.selected = false;
				std::map<int, ValueFilterData>& valuesRef = tagIt->second.values;
				for (std::map<int, ValueFilterData>::iterator valueIt = valuesRef.begin(); valueIt != valuesRef.end(); valueIt++) {

					valueIt->second.button.SetState(0);
					valueIt->second.comparativeValue = values[valueIt->first].defaultValue;

				}

			}

		}

		ImGui::SameLine();
		if (openedGlobalEditorFromLocal && ImGui::Button("Return to local editor")) { ImGui::CloseCurrentPopup(); openedGlobalEditorFromLocal = false; }
		ImGui::NewLine();

		if (AreFiltersBeingUsed()) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: filtering options are being used."); }

		for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

			ImGui::NewLine(); ImGui::Spacing(); ImGui::SameLine();
			if (ImGui::Checkbox(MakeImGuiName("##TagSelection", tagIt->first).c_str(), &tagIt->second.selected)) { changedSelection = true; }
			ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.4f);
			ImGui::InputText(MakeImGuiName("##TagName", tagIt->first).c_str(), tagIt->second.name, IM_ARRAYSIZE(tagIt->second.name));
			ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.3f);
			if (ImGui::InputFloat3(MakeImGuiName("##TagColor", tagIt->first).c_str(), &tagIt->second.color.a)) { tagIt->second.color.Clamp(0.0f, 1.0f); }
			ImGui::SameLine(); ImGuiButtonColored("", ImVec4(tagIt->second.color.a, tagIt->second.color.b, tagIt->second.color.c, tagIt->second.color.d), tagIt->first);

			std::string deleteTagName = MakeImGuiName("Delete##Tag", tagIt->first);
			std::map<int, ValueFilterData>& valuesRef = tagIt->second.values;
			if (values.size() > 0) {

				ImGui::SameLine(); if (ImGui::Button(deleteTagName.c_str())) { ImGui::OpenPopup(deleteTagName.c_str()); }
				ImGui::NewLine(); ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
				ImGui::Columns(popupSize.a > 300 ? popupSize.a / 300 : 1, NULL, false);
				for (std::map<int, ValueFilterData>::iterator valueIt = valuesRef.begin(); valueIt != valuesRef.end(); valueIt++) {

					std::string valueNameAux = std::to_string(tagIt->first);
					std::string buttonNameAux = "##TagButton" + valueNameAux + std::to_string(valueIt->first);
					ImGui::Text(GetValueName(valueIt->first).c_str()); ImGui::SameLine();
					ImGui::SetNextItemWidth(popupSize.a > 800 ? popupSize.a * 0.1f / (popupSize.a / 800) : popupSize.a * 0.1f);
					ImGui::InputInt(MakeImGuiName(std::string("##TagValueValue" + valueNameAux), valueIt->first).c_str(), &valueIt->second.value, 0.0f);
					valueIt->second.button.DrawButton(buttonNameAux);
					ImGui::SetNextItemWidth(popupSize.a > 800 ? popupSize.a * 0.1f / (popupSize.a / 800) : popupSize.a * 0.1f);
					ImGui::InputInt(MakeImGuiName(std::string("##TagComparativeValueValue" + valueNameAux), valueIt->first).c_str(), &valueIt->second.comparativeValue, 0.0f);
					ImGui::NextColumn();

				}

				ImGui::Columns(1);

			}

			if (DrawImGuiDeletePopup(deleteTagName)) { deleteTagID = tagIt->first; }

		} // -----------------------------------------------------------

		if (deleteTagID != -1) { tags.erase(tags.find(deleteTagID)); }
		if (deleteValueID != -1) { DeleteValue(deleteValueID); }

		ImGui::EndPopup();

	}

	return changedSelection;

}


void TagCluster::DrawLocalTagEditor() {

	bool includeAllTags = false;

	if (menuData != nullptr && StartPopup(localTagEditor)) {

		ImGui::NewLine(); // TAG EDITOR -----------------------------------------------------------
		if (ImGui::Button("Include all tags")) { includeAllTags = true; } ImGui::SameLine();
		if (ImGui::Button("Remove all tags")) { menuData->tags.clear(); } ImGui::SameLine();
		if (ImGui::Button("Open global editor")) { openedGlobalEditorFromLocal = true; }

		// Whenever we switch elements, it is the user that has to deal with this by updated the three menuDatas and the menuDataImage
		if (priorMenuData != nullptr) {

			ImGui::SameLine();
			if (ImGui::Button("Go to prior element")) { menuDataChanged = -1; }

		}

		if (nextMenuData != nullptr) {

			ImGui::SameLine();
			if (ImGui::Button("Go to next element")) { menuDataChanged = 1; }

		}

		ImGui::SameLine(); ImGui::Text(currentElementName.c_str());

		for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

			bool isSelected = includeAllTags ? true : menuData->tags.count(tagIt->first) ? true : false;
			ImGui::NewLine(); ImGui::Spacing(); ImGui::SameLine();
			if (ImGui::Checkbox(MakeImGuiName("##LocalTagSelection", tagIt->first).c_str(), &isSelected) || includeAllTags) {

				if (isSelected) {

					std::map<int, int > newValues;
					std::map<int, ValueFilterData>& tagValues = tagIt->second.values;
					for (std::map<int, ValueFilterData>::iterator valueIt = tagValues.begin(); valueIt != tagValues.end(); valueIt++) { newValues.insert({ valueIt->first, valueIt->second.value }); }
					menuData->tags.insert(std::pair<int, std::map<int, int>>(tagIt->first, newValues));

				}

				else { menuData->tags.erase(tagIt->first); }

			}

			ImGui::SameLine(); ImGui::Text((GetTagName(tagIt->first) + ":").c_str()); ImGui::SameLine();

			if (menuData->tags.count(tagIt->first) > 0) {

				int valueCounter = 1;
				std::map<int, int>& values = menuData->tags[tagIt->first];
				for (std::map<int, int>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

					if (valueCounter % (popupSize.a > 800 ? int(popupSize.a / 180) : 4) == 0) { ImGui::NewLine(); ImGui::NewLine(); ImGui::Spacing(); ImGui::SameLine(); }
					ImGui::Text(GetValueName(valueIt->first).c_str()); ImGui::SameLine();
					ImGui::SetNextItemWidth(popupSize.a > 800 ? popupSize.a * 0.1f / (popupSize.a / 800) : popupSize.a * 0.1f);
					ImGui::InputInt(MakeImGuiName(std::string("##LocalTagValue" + std::to_string(tagIt->first)), valueIt->first).c_str(), &valueIt->second, 0.0f);
					ImGui::SameLine(); valueCounter++;

				}

			}

			ImGui::NewLine(); ImGui::NewLine(); ImGui::Separator();

		}

		if (menuDataImage != nullptr) {

			float imageRatio = (float)menuDataImage->height / (float)menuDataImage->width;
			ImVec2 imageRealSize = { popupSize.b * 0.8f / imageRatio, popupSize.b * 0.8f };

			ImGui::NewLine();
			ImGui::SetCursorPosX((popupSize.a * 0.5f) - (imageRealSize.x * 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.8f, 0.0f, 0.0f));
			ImGui::ImageButton((void*)menuDataImage->handle, imageRealSize);
			ImGui::PopStyleColor(3);

		}

		ImGui::EndPopup();

	}

}


void TagCluster::DrawClusterOptionsMenu() {

	if (StartPopup(optionsMenu)) {

		bool unusableTermination = false, unusableRewrittenTermination = false;
		ImGui::NewLine(); ImGui::Text("Default value score:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(popupSize.a * 0.1f); ImGui::InputInt("##defaultTagValue", &valueDefault, 0);
		ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine(); ImGui::Text("Nomenclature settings:"); ImGui::NewLine();

		ImGui::TextWrapped("The nomenclature settings comprise the set of variables that will be considered when changing file names, or loading them, using the tag system. Since we want to work with filenames knowing what part is the name, and what part defines tag data, be aware to not include the termination or separator within a file, or change them so they don't coincide.");
		ImGui::NewLine(); ImGui::TextWrapped("WARNING: if you want to rewrite a file name using the following options, know we will not be making checks regarding file-invalid chars, those being >, <, :, '' (double quotes), /, \\, |, ? and *. Similarly, it is advised you don't use this method on nameless files, to prevent giving them a Windows reserved file name (those being CON, PRN, AUX, NUL, COM and LPT, those last two with termination 1-9. If you do, the app will crash, so if you want to take that risk, make sure to save first.).");

		ImGui::NewLine(); ImGui::Text("Separator (?):");
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("An element that will be added at the end of the file's name.\nIt is not mandatory, but it's highle recommended, as it helps in cases like having a file\ncontaining the same alias as a tag, if you want to create tags from file names"); }
		ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.15f);
		ImGui::InputText("##nameSeparator", (char*)nameSeparator, IM_ARRAYSIZE(nameSeparator));
		ImGui::SameLine(); ImGui::Text("Use separator:"); ImGui::SameLine(); ImGui::Checkbox("##useSeparatorCheck", &useSeparator);

		ImGui::NewLine(); ImGui::Text("Termination (?):");
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Added to each set of files that share the same name (any valid expression is accepted,\nwith the condition it contains a single 'x' character, which will be substituted by a number\n(spaces are added automatically))\n\nMake sure the termination and, hypothetically speaking, the rewritten termination you want to erase, aren't the same\nMost of the times, it isn't a problem, but it will provide maximum naming consistency"); }
		ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.15f);
		if (ImGui::InputText("##nameAccumulator", (char*)nameTermination, IM_ARRAYSIZE(nameTermination))) {
			if (std::string(nameTermination).find("x") == std::string::npos) { unusableTermination = true; }
		}
		ImGui::SameLine(); ImGui::Text("Always terminate (?):");
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Meaning even if a filename isn't repeated,\na termination value will be added"); }
		ImGui::SameLine(); ImGui::Checkbox("##terminateCheckbox", &terminateAllElements);
		ImGui::SameLine(); ImGui::Text("Starting value:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(popupSize.a * 0.1f); ImGui::InputInt("##startingTermination", &startingX, 0); ImGui::NewLine();
		if (unusableTermination) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Termination doesn't contain an 'x' value, thus it won't be applied."); }

		ImGui::Text("Rewrite old termination (?):");
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("If the file already has a termination that shouldn't be carried into the final name,\nenabling this option will make the program delete it as specified. SPACES ARE NOT ASSUMED!!"); }
		ImGui::SameLine(); ImGui::Checkbox("##rewriteTerminationCheckbox", &rewriteOldTermination);
		ImGui::SameLine(); ImGui::Text("Is old termination a number?");
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("This option prevents, for example, that renaming causes a file called 'a (b) (1)' that turns to 'a (b)' from becoming 'a' once renaming it again"); }
		ImGui::SameLine(); ImGui::Checkbox("##oldTerminationIsNumberCheckbox", &oldTerminationIsNumberOnly);
		ImGui::SameLine(); ImGui::Text("Original file termination:");
		ImGui::SameLine(); ImGui::SetNextItemWidth(popupSize.a * 0.15f);
		if (ImGui::InputText("##originalTermination", (char*)systemTermination, IM_ARRAYSIZE(systemTermination))) {
			if (std::string(systemTermination).find("x") == std::string::npos) { unusableRewrittenTermination = true; }
		}
		if (unusableRewrittenTermination) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Termination doesn't contain an 'x' value, thus it won't be applied."); }

		ImGui::NewLine();
		std::string nomenclatureButton = useTagsForNomenclature == 0 ? "Use tags" : useTagsForNomenclature == 1 ? "Use total value scores" : "Don't add aliases";
		if (ImGui::Button(nomenclatureButton.c_str())) { useTagsForNomenclature = (useTagsForNomenclature > 1 ? 0 : useTagsForNomenclature + 1); }

		if (useTagsForNomenclature == 1) {

			ImGui::SameLine(); ImGui::Text("Include default scores (?):");
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Whether default scores get included when calculating the final score of the value"); }
			ImGui::SameLine(); ImGui::Checkbox("##includeDefaultScoresCheckbox", &includeDefaultScores);
			ImGui::SameLine(); ImGui::Text("Include default final scores (?):");
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Whether the final score of a value is included in the name if it's equal to the default value"); }
			ImGui::SameLine(); ImGui::Checkbox("##includeDefaultFinalScoresCheckbox", &includeDefaultFinalScores);

		}

		// TODO: add an example of how the nomenclature would look

		if (useTagsForNomenclature == 0) {

			for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

				ImGui::NewLine(); ImGui::Text(("Alias for tag " + GetTagName(tagIt->first) + ":").c_str()); ImGui::SameLine();
				ImGui::SetNextItemWidth(popupSize.a * 0.3f);
				ImGui::InputText(MakeImGuiName("##tagAlias", tagIt->first).c_str(), (char*)tagIt->second.alias, IM_ARRAYSIZE(tagIt->second.alias));
				if (strcmp(nameTermination, tagIt->second.alias) == 0) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Alias coincides with termination, thus termination won't be applied."); }

			}

		}

		else if (useTagsForNomenclature == 1) {

			for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

				ImGui::NewLine(); ImGui::Text(("Alias for value " + GetValueName(valueIt->first) + ":").c_str()); ImGui::SameLine();
				ImGui::SetNextItemWidth(popupSize.a * 0.3f);
				ImGui::InputText(MakeImGuiName("##valueAlias", valueIt->first).c_str(), (char*)values[valueIt->first].alias, IM_ARRAYSIZE(values[valueIt->first].alias));
				if (strcmp(nameTermination, values[valueIt->first].alias) == 0) { ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Alias coincides with termination, thus termination won't be applied."); }

			}

		}

		ImGui::EndPopup();

	}

}


void TagCluster::DrawTagSelectionGridWithoutMenu(int columns) {

	ImGui::NewLine(); ImGui::Text("Tag filtering selection:"); ImGui::Separator(); ImGui::NewLine();

	ImGui::Columns(columns);

	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

		ImGui::Selectable(GetTagName(tagIt->first).c_str(), &tagIt->second.selected, ImGuiSelectableFlags_DontClosePopups);
		ImGui::NextColumn();

	}

	ImGui::Columns(1);

}

void TagCluster::AddTag(TagClusterData newData) {

	AddMissingTagClusterDataValues(newData.values);
	tags.insert({ tags.size() > 0 ? tags.rbegin()->first + 1 : 0, newData });

}


void TagCluster::AddValue(std::string name, int value) {

	int newValueID = values.size() > 0 ? values.rbegin()->first + 1 : 0;
	strcpy_s(values[newValueID].name, name.c_str());
	values[newValueID].defaultValue = value;
	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

		tagIt->second.values[newValueID].value = value;
		tagIt->second.values[newValueID].comparativeValue = value;

	}

}


void TagCluster::DeleteValue(int valueID) {

	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

		if (tagIt->second.values.count(valueID) == 1) { tagIt->second.values.erase(valueID); }

	}

	values.erase(valueID);

}


void TagCluster::AddMissingTagClusterDataValues(std::map<int, ValueFilterData>& dataValues) {

	for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

		if (dataValues.count(valueIt->first) == 0) {

			dataValues[valueIt->first].value = valueIt->second.defaultValue;
			dataValues[valueIt->first].comparativeValue = valueIt->second.defaultValue;

		}

	}

}


void TagCluster::RemoveObsoleteTagClusterDataValues(std::map<int, ValueFilterData>& dataValues) {

	for (std::map<int, ValueFilterData>::iterator valueIt = dataValues.begin(); valueIt != dataValues.end(); ) {

		if (values.count(valueIt->first) == 0) { dataValues.erase(valueIt->first); }
		else { valueIt++; }

	}

}


std::string TagCluster::GetTagName(int tagID) { return tags.count(tagID) == 0 ? "" : tags[tagID].name; }

std::string TagCluster::GetTagAlias(int tagID) { return tags.count(tagID) == 0 ? "" : tags[tagID].alias; }

const TagCluster::TagClusterData* TagCluster::GetTagData(int tagID) { return tags.count(tagID) == 0 ? nullptr : &tags[tagID]; }

int TagCluster::GetTagID(std::string tagName) {

	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { if (tagIt->second.name == tagName) { return tagIt->first; } }
	return -1;

}

TagData TagCluster::GetTagDataFromName(std::string filename) {

	return useTagsForNomenclature == 0 ? GetTagDataTagFromName(filename) : useTagsForNomenclature == 1 ? GetTagDataValuesFromName(filename) : TagData();

}

TagData TagCluster::GetTagDataTagFromName(std::string filename) {

	TagData newData;

	if (tags.size() > 0) {

		int pureNameSize = GetPureName(filename).size();
		std::string tagName = filename.substr(pureNameSize, filename.size() - pureNameSize);

		for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

			if (CircularMotion::GetStringAfterString(tagName, (" " + std::string(tagIt->second.alias))).size() < tagName.size()) {

				newData.tags.insert({ tagIt->first, std::map<int, int>() });

			}

		}

	}

	return newData;

}

TagData TagCluster::GetTagDataValuesFromName(std::string filename) {

	TagData newData;

	if (tags.size() > 0) {

		std::map<int, int> newValues;
		int pureNameSize = GetPureName(filename).size();

		for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

			bool pertinentValueSearch = true;
			std::string valueName = filename.substr(pureNameSize, filename.size() - pureNameSize);

			while (pertinentValueSearch) {

				std::string valueScore = CircularMotion::GetStringAfterString(valueName, (" " + std::string(valueIt->second.alias)));
				if (valueScore.size() < valueName.size() && valueScore.size() > 0) {

					if (CircularMotion::IsNumber(valueScore[0]) || CircularMotion::AreTwoFirstCharsANumber(valueScore, 0)) {

						pertinentValueSearch = false;
						newValues.insert({ valueIt->first, CircularMotion::GetNumberFromMixedString(valueScore, 0) });

					}

					else { valueName = valueScore; } // Value encountered wasn't correct (for example, getting score " PS2" when looking for alias " P")

				}

				else { pertinentValueSearch = false; } // No more values coincide with alias

			}

		}

		std::map<int, TagClusterData>::iterator firstTag = tags.begin();

		for (std::map<int, ValueFilterData>::iterator valueIt = firstTag->second.values.begin(); valueIt != firstTag->second.values.end(); valueIt++) {

			newValues.insert({ valueIt->first, valueIt->second.value });

		}

		newData.tags.insert({ tags.begin()->first, newValues });

	}

	return newData;

}

int TagCluster::GetIfMenuDataChanged(bool cleanData) {

	int aux = menuDataChanged;
	if (cleanData) { menuDataChanged = 0; }
	return aux;

}

void TagCluster::SetNewMenuData(TagData* data, TagData* prior, TagData* next, Texture* dataImage, std::string elementName) {

	menuData = data;
	priorMenuData = prior;
	nextMenuData = next;
	menuDataImage = dataImage;
	currentElementName = elementName;

}

std::string TagCluster::GetValueName(int valueID) { return values.count(valueID) == 0 ? "" : values[valueID].name; }

int TagCluster::GetValueID(std::string valueName) {

	for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

		if (strcmp(valueIt->second.name, valueName.c_str()) == 0) { return valueIt->first; }

	}

	return -1;

}


std::string TagCluster::GetPureName(std::string rawName) {

	std::string pureName = rawName;

	pureName = CircularMotion::GetStringBeforeLastOfString(pureName, "."); // Removes extension
	// TODO: what with extensionless files? Do we compare "." with separator, and if they are the same, count that we have more than 1?

	if (useSeparator) { pureName = CircularMotion::GetStringBeforeLastOfString(pureName, nameSeparator); } // TODO: I don't think we are accounting for separator and termination being equal values
	else {

		bool editedName = false;

		if (useTagsForNomenclature == 0) {

			for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); ) {

				std::string tagAlias = ' ' + GetTagAlias(tagIt->first);
				size_t tagIndex = CircularMotion::GetIndexOfLastStringInsideString(pureName, tagAlias);

				if (tagIndex != std::string::npos) {

					pureName = CircularMotion::GetStringBeforeLastOfString(pureName, tagAlias);
					editedName = true;
					tagIt = tags.end();

				}

				else { tagIt++; }

			}

		}

		else if (useTagsForNomenclature == 1) {

			for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); ) {

				std::string valueAlias = ' ' + std::string(values[valueIt->first].alias);
				size_t valueIndex = CircularMotion::GetIndexOfLastStringInsideString(pureName, valueAlias);

				if (valueIndex != std::string::npos) {

					pureName = CircularMotion::GetStringBeforeLastOfString(pureName, valueAlias);
					editedName = true;
					valueIt = values.end();

				}

				else { valueIt++; }

			}

		}

		if (!editedName) {

			std::string termination = GetTerminationStringFromFileName(pureName, true);
			if (termination.size() > 0) {

				std::string startingTermination = CircularMotion::GetStringBeforeString(nameTermination, "x");
				std::string value = CircularMotion::GetStringAfterLastOfString(termination, startingTermination);
				value = CircularMotion::GetStringBeforeString(termination, CircularMotion::GetStringAfterLastOfString(nameTermination, "x"));
				if (terminateAllElements || std::stoi(value) >= startingX) { pureName = CircularMotion::GetStringBeforeLastOfString(pureName, " " + startingTermination); }

			}

		}

	}

	if (rewriteOldTermination && std::string(systemTermination).find("x") != std::string::npos) {

		bool xValueCorrect = true;
		std::string startingTermination = CircularMotion::GetStringBeforeString(systemTermination, "x");

		if (oldTerminationIsNumberOnly) {

			std::string xTermination = CircularMotion::GetStringAfterLastOfString(pureName, startingTermination);
			xTermination = CircularMotion::GetStringBeforeLastOfString(xTermination, CircularMotion::GetStringAfterLastOfString(systemTermination, "x"));
			xValueCorrect = CircularMotion::IsNumber(xTermination);

		}

		if (xValueCorrect) {

			startingTermination = CircularMotion::GetStringBeforeString(systemTermination, "x");
			pureName = CircularMotion::GetStringBeforeLastOfString(pureName, startingTermination);

		}

	}

	return pureName;

}


std::string TagCluster::GetAliasesString(std::string rawName) {

	std::string aliases = "";

	// TODO: do it when we can ensure aliases don't repeat among themselves. Though we are using aliases to compose already, so what this function for?

	return aliases;

}


std::string TagCluster::ComposeNameWithoutTermination(const std::string& pureName, TagData& data) {

	std::string composedName = pureName;

	if (useSeparator) { composedName += nameSeparator; }

	if (useTagsForNomenclature == 0) {

		for (std::map<int, std::map<int, int>>::iterator tagIt = data.tags.begin(); tagIt != data.tags.end(); tagIt++) { composedName += (" " + GetTagAlias(tagIt->first)); }

	}

	else if (useTagsForNomenclature == 1) {

		std::map<int, int> valueAccumulator;
		for (std::map<int, std::map<int, int>>::iterator tagIt = data.tags.begin(); tagIt != data.tags.end(); tagIt++) {

			for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

				if (includeDefaultScores || tagIt->second[valueIt->first] != valueIt->second.defaultValue) {

					if (valueAccumulator.count(valueIt->first) == 0) { valueAccumulator.insert(std::pair<int, int>(valueIt->first, tagIt->second[valueIt->first])); }
					else { valueAccumulator[valueIt->first] += tagIt->second[valueIt->first]; }

				}

			}

		}

		for (std::map<int, int> ::iterator valueIt = valueAccumulator.begin(); valueIt != valueAccumulator.end(); valueIt++) {

			if (includeDefaultFinalScores || valueIt->second != values[valueIt->first].defaultValue) { composedName += (" " + std::string(values[valueIt->first].alias) + std::to_string(valueIt->second)); }

		}

	}

	return composedName;

}


std::string TagCluster::GetTermination(int fileRepeatedIndex, int totalFilesWithSameName) {

	bool userWantsTermination = totalFilesWithSameName == 1 && !terminateAllElements;
	std::string terminationString = nameTermination;
	std::string finalTermination = " ";
	size_t hasX = terminationString.find("x");

	if (hasX != std::string::npos && !userWantsTermination) {

		for (size_t i = 0; i < terminationString.length(); i++) {

			if (terminationString[i] != 'x') { finalTermination += terminationString[i]; }
			else { finalTermination += std::to_string(startingX + fileRepeatedIndex); }

		}

	}

	else {

		if (userWantsTermination) { return ""; }
		else { return " (" + std::to_string(startingX + fileRepeatedIndex) + ")"; }

	}

	return finalTermination;

}


std::string TagCluster::GetTerminationStringFromFileName(std::string name, bool spaceBeforeName) {

	std::string termination = "";
	std::string preTermination = (spaceBeforeName ? " " : "") + CircularMotion::GetStringBeforeLastOfString(nameTermination, "x");
	std::string postTermination = CircularMotion::GetStringAfterLastOfString(nameTermination, "x");
	size_t startingIndex = CircularMotion::GetIndexOfLastStringInsideString(name, preTermination) + preTermination.length(); // TODO: if we have -x-, technically this is fucked, and we have to check that this find last is happening before the x, even though we don't have a way to find the x, technically (I guess knowing the termination is always at the end means we can check that there are more elements after it)
	size_t endingIndex = CircularMotion::GetIndexOfLastStringInsideString(name, postTermination);

	if (startingIndex != std::string::npos && endingIndex != std::string::npos) {

		std::string xString = name.substr(startingIndex, endingIndex - startingIndex);
		if (CircularMotion::IsNumber(xString)) { termination = preTermination + xString + postTermination; }

	}

	return termination;

}

void TagCluster::ResetClusterData() {

	valueBuffer = valueDefault;
	memset(nameBuffer, 0, sizeof(nameBuffer));
	memset(colorBuffer, 0, sizeof(colorBuffer));
	memset(valueNameBuffer, 0, sizeof(valueNameBuffer));
	openedGlobalEditorFromLocal = false;
	tags.clear();
	values.clear();
	menuData = nullptr;
	priorMenuData = nullptr;
	nextMenuData = nullptr;
	menuDataImage = nullptr;

}


void TagCluster::UpdateTagDataValues(TagData& data) {

	for (std::map<int, std::map<int, int>>::iterator tagIt = data.tags.begin(); tagIt != data.tags.end(); ) {

		if (tags.count(tagIt->first) == 0) { data.tags.erase(tagIt->first); }

		else {

			std::map<int, int> dataValues = data.tags[tagIt->first];
			for (std::map<int, int>::iterator valueIt = dataValues.begin(); valueIt != dataValues.end(); valueIt++) {

				if (values.count(valueIt->first) == 0) { data.tags[tagIt->first].erase(valueIt->first); }

			}

			for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

				data.tags[tagIt->first].insert({ valueIt->first, tags[tagIt->first].values[valueIt->first].value });

			}

			tagIt++;

		}

	}

}


bool TagCluster::AreTagFiltersBeingUsed() {

	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) { if (tagIt->second.selected) { return true; } }
	return false;

}


bool TagCluster::AreValueFiltersBeingUsed() {

	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

		std::map<int, ValueFilterData>& valuesRef = tagIt->second.values;
		for (std::map<int, ValueFilterData>::iterator valueIt = valuesRef.begin(); valueIt != valuesRef.end(); valueIt++) {

			if (!valueIt->second.button.IsNeutralState()) { return true; }

		}

	}

	return false;

}


bool TagCluster::AreFiltersBeingUsed() { return (AreTagFiltersBeingUsed() || AreValueFiltersBeingUsed()); }


bool TagCluster::ShouldDrawData(TagData* data) {

	bool availableTags = false;

	for (std::map<int, std::map<int, int>>::iterator tagIt = data->tags.begin(); tagIt != data->tags.end(); tagIt++) {

		if (!availableTags && tags[tagIt->first].selected) { availableTags = true; }
		if (availableTags) {

			std::map<int, int>& valuesRef = tagIt->second;
			for (std::map<int, int>::iterator valueIt = valuesRef.begin(); valueIt != valuesRef.end(); valueIt++) {

				ValueFilterData& filterRef = tags[tagIt->first].values[valueIt->first];
				if (!filterRef.button.CompareValues(valueIt->second, filterRef.comparativeValue)) { return false; }

			}

		}

	}

	return availableTags;

}


void TagCluster::SaveDataXML(pugi::xml_node& firstNode, bool resetClusterData) {

	pugi::xml_node clusterNode = firstNode.append_child("TagClusterData");
	clusterNode.append_attribute("defaultValue") = valueDefault;
	clusterNode.append_attribute("useTagsForNomenclature") = useTagsForNomenclature;
	clusterNode.append_attribute("nameTermination") = nameTermination;
	clusterNode.append_attribute("nameSeparator") = nameSeparator;
	clusterNode.append_attribute("systemTermination") = systemTermination;
	clusterNode.append_attribute("startingX") = startingX;
	clusterNode.append_attribute("terminateAllElements") = terminateAllElements;
	clusterNode.append_attribute("useSeparator") = useSeparator;
	clusterNode.append_attribute("includeDefaultScores") = includeDefaultScores;
	clusterNode.append_attribute("includeDefaultFinalScores") = includeDefaultFinalScores;
	clusterNode.append_attribute("rewriteTermination") = rewriteOldTermination;
	clusterNode.append_attribute("oldTerminationIsNumberOnly") = oldTerminationIsNumberOnly;

	pugi::xml_node tagNode = clusterNode.append_child("TagClusterList");
	for (std::map<int, TagClusterData>::iterator tagIt = tags.begin(); tagIt != tags.end(); tagIt++) {

		pugi::xml_node tagDataNode = tagNode.append_child("Tag");
		tagDataNode.append_attribute("name") = GetTagName(tagIt->first).c_str();
		tagDataNode.append_attribute("colorR") = tagIt->second.color.a;
		tagDataNode.append_attribute("colorG") = tagIt->second.color.b;
		tagDataNode.append_attribute("colorB") = tagIt->second.color.c;
		tagDataNode.append_attribute("colorA") = tagIt->second.color.d;
		tagDataNode.append_attribute("alias") = tagIt->second.alias;
		tagDataNode.append_attribute("selected") = tagIt->second.selected;

		pugi::xml_node tagValueNode = tagDataNode.append_child("TagClusterValueList");
		for (std::map<int, ValueFilterData>::iterator tagValueIt = tagIt->second.values.begin(); tagValueIt != tagIt->second.values.end(); tagValueIt++) {

			pugi::xml_node tagValueDataNode = tagValueNode.append_child("Value");
			tagValueDataNode.append_attribute("name") = GetValueName(tagValueIt->first).c_str();
			tagValueDataNode.append_attribute("value") = tagValueIt->second.value;
			tagValueDataNode.append_attribute("comparativeValue") = tagValueIt->second.comparativeValue;
			tagValueDataNode.append_attribute("buttonState") = tagValueIt->second.button.GetState();

		}

	}

	pugi::xml_node valueNode = clusterNode.append_child("ValueClusterList");
	for (std::map<int, ValueClusterData>::iterator valueIt = values.begin(); valueIt != values.end(); valueIt++) {

		pugi::xml_node valueData = valueNode.append_child("Value");
		valueData.append_attribute("name") = GetValueName(valueIt->first).c_str();
		valueData.append_attribute("alias") = values[valueIt->first].alias;
		valueData.append_attribute("default") = values[valueIt->first].defaultValue;

	}

	if (resetClusterData) { ResetClusterData(); }

}


void TagCluster::LoadDataXML(pugi::xml_node& node) {

	pugi::xml_node clusterNode = node.child("TagClusterData");
	valueDefault = clusterNode.attribute("defaultValue").as_int();
	useTagsForNomenclature = clusterNode.attribute("useTagsForNomenclature").as_bool();
	strcpy_s(nameTermination, clusterNode.attribute("nameTermination").as_string());
	strcpy_s(nameSeparator, clusterNode.attribute("nameSeparator").as_string());
	strcpy_s(systemTermination, clusterNode.attribute("systemTermination").as_string());
	startingX = clusterNode.attribute("startingX").as_int();
	terminateAllElements = clusterNode.attribute("terminateAllElements").as_bool();
	useSeparator = clusterNode.attribute("useSeparator").as_bool();
	includeDefaultScores = clusterNode.attribute("includeDefaultScores").as_bool();
	includeDefaultFinalScores = clusterNode.attribute("includeDefaultFinalScores").as_bool();
	rewriteOldTermination = clusterNode.attribute("rewriteTermination").as_bool();
	oldTerminationIsNumberOnly = clusterNode.attribute("oldTerminationIsNumberOnly").as_bool();

	for (pugi::xml_node valueNode = clusterNode.child("ValueClusterList").first_child(); valueNode; valueNode = valueNode.next_sibling()) {

		int newID = values.size();
		strcpy_s(values[newID].name, valueNode.attribute("name").as_string());
		strcpy_s(values[newID].alias, valueNode.attribute("alias").as_string());
		values[newID].defaultValue = valueNode.attribute("default").as_int();

	}

	for (pugi::xml_node tagNode = clusterNode.child("TagClusterList").first_child(); tagNode; tagNode = tagNode.next_sibling()) {

		TagClusterData newTag(tagNode.attribute("name").as_string(), cmf4(tagNode.attribute("colorR").as_float(), tagNode.attribute("colorG").as_float(), tagNode.attribute("colorB").as_float(), tagNode.attribute("colorA").as_float()), tagNode.attribute("selected").as_bool());
		strcpy_s(newTag.alias, tagNode.attribute("alias").as_string());
		for (pugi::xml_node tagValueNode = tagNode.child("TagClusterValueList").first_child(); tagValueNode; tagValueNode = tagValueNode.next_sibling()) {

			std::string tagName = tagValueNode.attribute("name").as_string();
			newTag.values[GetValueID(tagName)].value = tagValueNode.attribute("value").as_int();
			newTag.values[GetValueID(tagName)].comparativeValue = tagValueNode.attribute("comparativeValue").as_int();
			newTag.values[GetValueID(tagName)].button.SetState(tagValueNode.attribute("buttonState").as_int());

		}

		AddTag(newTag);

	}

}


void TagCluster::SaveDataXML(pugi::xml_node& uniquelyNamedParentNode, TagData& data, bool resetTagData) {

	for (std::map<int, std::map<int, int>>::iterator dataIt = data.tags.begin(); dataIt != data.tags.end(); dataIt++) {

		UpdateTagDataValues(data);
		pugi::xml_node tagNode = uniquelyNamedParentNode.append_child("Tag");
		tagNode.append_attribute("name") = GetTagName(dataIt->first).c_str();

		pugi::xml_node valueParentNode = tagNode.append_child("Values");
		for (std::map<int, int>::iterator valueIt = dataIt->second.begin(); valueIt != dataIt->second.end(); valueIt++) {

			pugi::xml_node valueNode = valueParentNode.append_child("Value");
			valueNode.append_attribute("name") = GetValueName(valueIt->first).c_str();
			valueNode.append_attribute("value") = valueIt->second;

		}

	}

	if (resetTagData) { data.ResetData(); }

}


void TagCluster::LoadDataXML(pugi::xml_node& uniquelyNamedParentNode, TagData& data) {

	for (pugi::xml_node tagNode = uniquelyNamedParentNode.first_child(); tagNode; tagNode = tagNode.next_sibling()) {

		int tagID = GetTagID(tagNode.attribute("name").as_string());
		// TODO: pretty sure this is where we ideally offer reassigning options
		if (tagID == -1) {}
		else {

			for (pugi::xml_node valueNode = tagNode.first_child().first_child(); valueNode; valueNode = valueNode.next_sibling()) {

				int valueID = GetValueID(valueNode.attribute("name").as_string());
				if (valueID == -1) {}// TODO: same problem. We should offer reassigning options here
				else {

					int value = valueNode.attribute("value").as_int();
					data.tags.insert(std::pair<int, std::map<int, int>>(tagID, std::map<int, int>()));
					data.tags[tagID].insert(std::pair<int, int>(valueID, value));

				}

			}

		}

	}

	UpdateTagDataValues(data);

}


