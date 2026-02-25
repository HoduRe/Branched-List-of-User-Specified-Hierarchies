#include "CircularMotion.h"

cmi2::cmi2() : a(), b() {}

cmi2::cmi2(int _value) : a(_value), b(_value) {}

cmi2::cmi2(int _a, int _b) : a(_a), b(_b) {}

cmf2::cmf2() : a(), b() {}

cmf2::cmf2(float _value) : a(_value), b(_value) {}

cmf2::cmf2(float _a, float _b) : a(_a), b(_b) {}

cmf2::cmf2(cmi2 _int) : a(_int.a), b(_int.b) {}

void cmf2::operator=(cmi2 intIn) { *this = cmf2(intIn.a, intIn.b); }

cmi4::cmi4() : a(-1.0f), b(-1.0f), c(-1.0f), d(-1.0f) {}

cmi4::cmi4(int _value) : a(_value), b(_value), c(_value), d(_value) {}

cmi4::cmi4(int _abc, int _d) : a(_abc), b(_abc), c(_abc), d(_d) {}

cmi4::cmi4(int _a, int _b, int _c, int _d) : a(_a), b(_b), c(_c), d(_d) {}

void cmi4::Clamp(int componentIndex, int floor, int ceiling) {

	int& value = componentIndex == 0 ? a : componentIndex == 1 ? b : componentIndex == 2 ? c : d;
	if (value > ceiling) { value = ceiling; }
	else if (value < floor) { value = floor; }

}


void cmi4::Clamp(int floor, int ceiling) { Clamp(0, floor, ceiling); Clamp(1, floor, ceiling); Clamp(2, floor, ceiling); Clamp(3, floor, ceiling); }


void cmi4::RGBAIn255Range(int* r, int* g, int* blue, int* alpha) {

	*r = a * 255;
	*g = b * 255;
	*blue = c * 255;
	if (alpha != nullptr) { *alpha = d * 255; }

}

cmf4::cmf4() : a(-1.0f), b(-1.0f), c(-1.0f), d(-1.0f) {}

cmf4::cmf4(float _value) : a(_value), b(_value), c(_value), d(_value) {}

cmf4::cmf4(float _abc, float _d) : a(_abc), b(_abc), c(_abc), d(_d) {}

cmf4::cmf4(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}

cmf4::cmf4(cmi4 _int) : a(_int.a), b(_int.b), c(_int.c), d(_int.d) {}

void cmf4::operator=(cmi4 intIn) { *this = cmf4(intIn.a, intIn.b, intIn.c, intIn.d); }

bool cmf4::operator==(cmf4 floatIn) { return (a == floatIn.a && b == floatIn.b && c == floatIn.c && d == floatIn.d); }

void cmf4::operator*(float mult) { a *= mult; b *= mult; c *= mult; d *= mult; }

void cmf4::Clamp(int componentIndex, float floor, float ceiling) {

	float& value = componentIndex == 0 ? a : componentIndex == 1 ? b : componentIndex == 2 ? c : d;
	if (value > ceiling) { value = ceiling; }
	else if (value < floor) { value = floor; }

}


void cmf4::Clamp(float floor, float ceiling) { Clamp(0, floor, ceiling); Clamp(1, floor, ceiling); Clamp(2, floor, ceiling); Clamp(3, floor, ceiling); }


void cmf4::RGBAIn255Range(float* r, float* g, float* blue, float* alpha) {

	*r = a * 255.0f;
	*g = b * 255.0f;
	*blue = c * 255.0f;
	if (alpha != nullptr) { *alpha = d * 255.0f; }

}
