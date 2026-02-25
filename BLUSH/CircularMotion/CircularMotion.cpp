#include "CircularMotion.h"
#include <cstring>

#pragma region GenericFunctions

void CircularMotion::Clamp(float& value, float floor, float ceiling) {

	if (value > ceiling) { value = ceiling; }
	else if (value < floor) { value = floor; }

}

float CircularMotion::Lerp(float min, float max, float alpha) { return min + ((max - min) * alpha); }

cmf2 CircularMotion::PositionOfTextureAtCenterOfPoint(cmf2 point, float width, float height, bool positiveYAxis) {

	return cmf2(point.a - (width * 0.5f), positiveYAxis ? point.b + (height * 0.5f) : point.b - (height * 0.5f));

}

bool CircularMotion::IsColorLighter(float r, float g, float b, float averageThreshold) { return (r + g + (b * 0.8f)) * 0.33333f > averageThreshold; }


bool CircularMotion::IsNumber(char c) { return (c >= 48 && c <= 57); }

bool CircularMotion::IsNumber(std::string baseString) {

	bool allAreNumber = true;
	for (size_t i = 0; i < baseString.length(); i++) {

		if (!IsNumber(baseString[i]) && !(baseString.length() > 1 && baseString[0] == '-')) { allAreNumber = false; }

	}

	return allAreNumber;

}

// For when you actually want to know if the start of a "-" is a negative number
bool CircularMotion::AreTwoFirstCharsANumber(std::string baseString, size_t startingIndex) {

	if (baseString.size() - 1 <= startingIndex) { return false; }
	return ((baseString[startingIndex] == '-' || IsNumber(baseString[startingIndex])) && IsNumber(baseString[startingIndex + 1]));

}

bool CircularMotion::IsLetter(char c) { return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)); }


int CircularMotion::GetNumberFromMixedString(std::string baseString, size_t firstDigitIndex) {

	bool isNumber = true;
	std::string number = "";
	size_t firstDigitAux = firstDigitIndex;
	while (firstDigitIndex < baseString.length() && isNumber) {

		isNumber = false;

		if (IsNumber(baseString[firstDigitIndex]) || (firstDigitAux == firstDigitIndex && AreTwoFirstCharsANumber(baseString, firstDigitAux))) {

			number += baseString[firstDigitIndex];
			isNumber = true;

		}

		firstDigitIndex++;

	}

	if (number.size() == 0) { return -1; }

	return std::stoi(number);

}


size_t CircularMotion::GetIndexOfLastStringInsideString(std::string baseString, std::string substring) { // Turns out find_last_of doesn't return an index to the last substring of a string. It returns the last index to any char of the substring within the string

	if (substring.length() > 0) {

		for (int i = int(baseString.length()) - 1; i >= 0; i--) {

			if (baseString[i] == substring[0] && int(baseString.length()) - i >= int(substring.length())) {

				bool sameString = true;

				for (int j = 0; j < substring.length(); j++) { if (!(substring[j] == baseString[i + j])) { sameString = false; } }

				if (sameString) { return i; }

			}

		}

	}

	return std::string::npos;

}


int CircularMotion::GetIntFromString(std::string baseString, int initialIndex, int finalIndex) {

	int number = -1;
	std::string substring = baseString.substr(initialIndex, finalIndex - initialIndex);
	if (IsNumber(substring)) { number = std::stoi(substring); }
	return number;

}

std::string CircularMotion::GetStringBeforeString(std::string baseString, std::string thresholdString) {

	size_t startingIndex = baseString.find(thresholdString);
	return startingIndex != std::string::npos ? baseString.substr(0, startingIndex) : baseString;

}


std::string CircularMotion::GetStringBeforeLastOfString(std::string baseString, std::string thresholdString) {

	size_t startingIndex = GetIndexOfLastStringInsideString(baseString, thresholdString);
	return startingIndex != std::string::npos ? baseString.substr(0, startingIndex) : baseString;

}


std::string CircularMotion::GetStringAfterString(std::string baseString, std::string thresholdString) {

	size_t startingIndex = baseString.find(thresholdString);
	size_t outLength = thresholdString.size();
	return startingIndex != std::string::npos ? baseString.substr(startingIndex + outLength, baseString.size() - startingIndex - outLength) : baseString;

}


std::string CircularMotion::GetStringAfterLastOfString(std::string baseString, std::string thresholdString) {

	size_t startingIndex = GetIndexOfLastStringInsideString(baseString, thresholdString);
	size_t outLength = thresholdString.size();
	return startingIndex != std::string::npos ? baseString.substr(startingIndex + outLength, baseString.size() - startingIndex - outLength) : baseString;

}


std::string CircularMotion::GetStringWithoutString(std::string baseString, std::string excludingString) {

	std::string newString = baseString;
	size_t startingIndex = baseString.find(excludingString);

	do {

		if (startingIndex != std::string::npos) {

			if (startingIndex > 0) { newString = baseString.substr(0, startingIndex); }
			int finalIndex = startingIndex + excludingString.size();
			newString += baseString.substr(finalIndex, baseString.size() - startingIndex + 1);

		}

		startingIndex = newString.find(excludingString);
		baseString = newString;

	} while (startingIndex != std::string::npos);

	return newString;
}


std::string CircularMotion::GetStringLowerCase(const std::string& baseString) {

	std::string newString = "";
	for (size_t i = 0; i < baseString.size(); i++) { newString += (baseString[i] + (baseString[i] >= 65 && baseString[i] <= 90 ? 32 : 0)); }
	return newString;

}


std::string CircularMotion::GetStringUpperCase(const std::string& baseString) {

	std::string newString = "";
	for (size_t i = 0; i < baseString.size(); i++) { newString += (baseString[i] - (baseString[i] >= 97 && baseString[i] <= 122 ? 32 : 0)); }
	return newString;

}


bool CircularMotion::IsStringOrderEarlierWindowsLike(std::string string1, std::string string2) {

	string1 = GetStringLowerCase(string1);
	string2 = GetStringLowerCase(string2);
	bool isNumber1 = false, isNumber2 = false;

	for (size_t stringIt = 0; stringIt < string1.length(); stringIt++) {

		if (string2.length() <= stringIt) { return false; }

		char c1 = string1[stringIt];
		char c2 = string2[stringIt];
		isNumber1 = IsNumber(c1);
		isNumber2 = IsNumber(c2);
		if (isNumber1 && IsLetter(c2)) { return true; }
		if (isNumber1 && !IsLetter(c2) && !isNumber2) { return false; }
		if (!isNumber1 && !isNumber2 && c1 != c2) { return c1 < c2; }

		if (isNumber1 && isNumber2) {

			int number1 = GetNumberFromMixedString(string1, stringIt);
			int number2 = GetNumberFromMixedString(string2, stringIt);

			if (number1 != number2) { return number1 < number2; }
			stringIt += std::to_string(number1).size() - 1;

		}

		isNumber1 = false;
		isNumber2 = false;

	}

	return false; // Very important that this is false, or else weird crashes start happening, because the map can't reach vaid values, and goes out of scope

}


bool CircularMotion::IsImageFormat(std::string filename) {

	std::string extension = filename.substr(GetIndexOfLastStringInsideString(filename, ".") + 1);
	extension = GetStringLowerCase(extension);

	if (extension == "jpg" || extension == "png" || extension == "avif" || extension == "bmp" || extension == "jfif" || extension == "jpeg"
		|| extension == "webp" || extension == "tif" || extension == "tiff") {
		return true;
	}

	return false;

}



#pragma endregion


#pragma region CollisionFunctions

bool CircularMotion::IsPointInsideQuad(cmf2 point, cmf4 quadXYWH) {

	cmf4& quad = quadXYWH;
	if ((point.a >= quad.a && point.a <= quad.a + quad.c)) { return (point.b >= quad.b && point.b <= quad.b + quad.d); }
	return false;

}

#pragma endregion
