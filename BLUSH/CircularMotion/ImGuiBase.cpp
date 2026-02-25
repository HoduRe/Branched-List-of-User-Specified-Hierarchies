#include "CircularMotion.h"

std::vector<std::string> ImGuiBase::errors;
cmi2* ImGuiBase::windowSize = nullptr;
const ImGuiInputTextFlags ImGuiBase::inputTextFlags(ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

ImGuiBase::ImGuiBase(cmi2* _windowSize) : popupSize(-1.0f, -1.0f), menuOpen(), menuClosed() { windowSize = _windowSize; }

bool ImGuiBase::DidMenuCloseThisFrame(std::string menuName) { return (menuClosed.count(menuName) > 0 && menuClosed[menuName]); }

bool ImGuiBase::DidAnyMenuCloseThisFrame() {

	for (std::map<const std::string, bool>::iterator menuIt = menuClosed.begin(); menuIt != menuClosed.end(); menuIt++) { if (menuIt->second) { return true; } }
	return false;

}

void ImGuiBase::AddErrorMessage(std::string message) { errors.push_back(message); }

void ImGuiBase::DrawErrorMessage() {

	if (errors.size() > 0) {

		ImGui::OpenPopup("ImGui Error menu");

		cmf2 finalSize = cmf2(float(windowSize->a) * 0.8f, float(windowSize->b) * 0.8f);
		ImGui::SetNextWindowPos(ImVec2(float(windowSize->a) * 0.5f - (finalSize.a * 0.5f), float(windowSize->b) * 0.5f - (finalSize.b * 0.5f)));
		ImGui::SetNextWindowSize(ImVec2(finalSize.a, finalSize.b));
		if (ImGui::BeginPopup("ImGui Error menu", (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar))) {

			ImGui::NewLine(); ImGui::Text("List of errors:");
			ImGui::Separator(); ImGui::NewLine();
			if (ImGui::Button("Clear all errors")) { errors.clear(); } ImGui::NewLine();

			for (size_t i = 0; i < errors.size(); ) {

				if (ImGui::Button("Delete")) { errors.erase(errors.begin() + i); }
				else { ImGui::SameLine(); ImGui::Text(errors[i].c_str()); i++; }

			}

			ImGui::EndPopup();

		}

		ImGui::CloseCurrentPopup();

	}

}


void ImGuiBase::OpenMenu(std::string menuName) { ImGui::OpenPopup(menuName.c_str()); menuOpen[menuName] = true; }


bool ImGuiBase::StartPopup(const std::string& popupName) {

	popupSize = cmf2(float(windowSize->a) * 0.9f, float(windowSize->b) * 0.9f);
	ImGui::SetNextWindowPos(ImVec2(float(windowSize->a) * 0.5f - (popupSize.a * 0.5f), float(windowSize->b) * 0.5f - (popupSize.b * 0.5f)));
	ImGui::SetNextWindowSize(ImVec2(popupSize.a, popupSize.b));
	return ImGui::BeginPopupModal(popupName.c_str(), &menuOpen[popupName], (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar));

}


void ImGuiBase::UpdateMenuState() {

	for (std::map<const std::string, bool>::iterator menuIt = menuOpen.begin(); menuIt != menuOpen.end();) {

		if (menuClosed.count(menuIt->first) > 0 && menuClosed[menuIt->first] && menuIt->second == false) { menuClosed[menuIt->first] = true; }
		else { menuClosed[menuIt->first] = false; }

		if (menuIt->second == false) { menuOpen.erase(menuIt++); }
		else { menuIt++; }

	}

}


std::string ImGuiBase::MakeImGuiName(std::string elementName, int tagID) { return (elementName + "##" + std::to_string(tagID)); }


bool ImGuiBase::DrawImGuiDeletePopup(std::string popupName) {

	bool ret = false;
	ImGuiWindowFlags fixedFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;

	if (ImGui::BeginPopupModal(popupName.c_str(), NULL, fixedFlags)) {

		ImGui::Text("Are you sure you want to delete this element?");

		if (ImGui::Button("YES")) {

			ImGui::CloseCurrentPopup();
			ret = true;

		}

		ImGui::SameLine();
		if (ImGui::Button("NO")) { ImGui::CloseCurrentPopup(); }

		ImGui::EndPopup();

	}

	return ret;

}


bool ImGuiBase::ImGuiButtonColored(std::string buttonName, ImVec4 color, int buttonID) {

	bool clicked = false;
	ImGui::PushStyleColor(ImGuiCol_Button, color);

	ImVec4 hoverColor = color;
	bool darkText = CircularMotion::IsColorLighter(color.x, color.y, color.z, 0.5f);
	float hoverOffset = CircularMotion::IsColorLighter(color.x, color.y, color.z, 0.25f) ? -0.2f : 0.3f;
	hoverColor.x = hoverColor.x + hoverOffset < 0.0f ? 0.0f : hoverColor.x + hoverOffset > 1.0f ? 1.0f : hoverColor.x + hoverOffset;
	hoverColor.y = hoverColor.y + hoverOffset < 0.0f ? 0.0f : hoverColor.y + hoverOffset > 1.0f ? 1.0f : hoverColor.y + hoverOffset;
	hoverColor.z = hoverColor.z + hoverOffset < 0.0f ? 0.0f : hoverColor.z + hoverOffset > 1.0f ? 1.0f : hoverColor.z + hoverOffset;

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
	ImGui::PushStyleColor(ImGuiCol_Text, darkText ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	clicked = ImGui::Button(MakeImGuiName(buttonName, buttonID).c_str());
	ImGui::PopStyleColor(3);

	return clicked;

}


void ImGuiBase::ImGuiTextWrappedColored(std::string textName, ImVec4 color) {

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::TextWrapped(textName.c_str());
	ImGui::PopStyleColor(1);

}


