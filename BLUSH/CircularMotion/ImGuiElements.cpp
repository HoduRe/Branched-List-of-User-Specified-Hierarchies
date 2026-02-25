#include "CircularMotion.h"

bool ComparativeButton::DrawButton(std::string& nameTermination) {

	bool ret = false;
	std::string buttonName = "o";

	switch (buttonState) {

	case 1: buttonName = "<"; break;
	case 2: buttonName = "<="; break;
	case 3: buttonName = "=="; break;
	case 4: buttonName = "!="; break;
	case 5: buttonName = ">"; break;
	case 6: buttonName = ">="; break;
	default: break;

	}

	ImGui::SameLine();
	if (ImGui::Button((buttonName + "##" + nameTermination).c_str())) {

		buttonState++;
		if (buttonState > 6) { buttonState = 0; }
		ret = true;

	}

	ImGui::SameLine();
	return ret;

}

bool ComparativeButton::IsNeutralState() { return buttonState == 0; }

bool ComparativeButton::IsLesserThan() { return buttonState == 1; }

bool ComparativeButton::IsLesserEqualThan() { return buttonState == 2; }

bool ComparativeButton::IsEqualTo() { return buttonState == 3; }

bool ComparativeButton::IsNotEqualTo() { return buttonState == 4; }

bool ComparativeButton::IsBiggerThan() { return buttonState == 5; }

bool ComparativeButton::IsBiggerEqualThan() { return buttonState == 6; }

int ComparativeButton::GetState() { return buttonState; }

void ComparativeButton::SetState(int _newState) { buttonState = (_newState < 0 || _newState > 6) ? buttonState : _newState; }

bool ComparativeButton::CompareValues(int value1, int value2) {

	if (IsNeutralState()) { return true; }
	else if (IsLesserThan()) { return value1 < value2; }
	else if (IsLesserEqualThan()) { return value1 <= value2; }
	else if (IsEqualTo()) { return value1 == value2; }
	else if (IsNotEqualTo()) { return value1 != value2; }
	else if (IsBiggerThan()) { return value1 > value2; }
	else if (IsBiggerEqualThan()) { return value1 >= value2; }

	return false;

}
