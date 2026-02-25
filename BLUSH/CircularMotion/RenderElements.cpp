#include "CircularMotion.h"

Texture::Texture() : handle(-1), width(0), height(0) {}

Texture::Texture(int _width, int _height, GLuint _handle) : handle(_handle), width(_width), height(_height) {}

Texture::~Texture() {}

float Texture::GetRatio() { return float(height) / float(width); }

float Texture::GetRatioWidthOverHeight() { return float(width) / float(height); }

TextureAdv::TextureAdv() : Texture(), format(0), internalFormat(0), directory("") {}

TextureAdv::TextureAdv(int _width, int _height, GLenum _format, GLint _internalFormat, std::string _directory, GLuint _handle) :
	Texture(_width, _height, _handle), format(_format), internalFormat(_internalFormat), directory(_directory) {}

TextureAdv::~TextureAdv() {}

FloatLerper::FloatLerper() : initialTime(), interpolationLength(), initialValues(), finalValues(), canGoOver1(false) {}

FloatLerper::FloatLerper(Uint32 _initialTime, float _interpolationLength, cmf4 _initialValues, cmf4 _finalValues, bool _canGoOver1) :
	initialTime(_initialTime), interpolationLength(_interpolationLength), initialValues(_initialValues), finalValues(_finalValues), canGoOver1(_canGoOver1) {}

cmf4 FloatLerper::GetInterpolatedValue(Uint32 newTime) {

	cmf4 newValue;
	float alpha = (newTime - initialTime) / interpolationLength;

	if (!canGoOver1 && alpha > 1.0f) { alpha = 1.0f; }
	newValue.a = CircularMotion::Lerp(initialValues.a, finalValues.a, alpha);
	newValue.b = CircularMotion::Lerp(initialValues.b, finalValues.b, alpha);
	newValue.c = CircularMotion::Lerp(initialValues.c, finalValues.c, alpha);
	newValue.d = CircularMotion::Lerp(initialValues.d, finalValues.d, alpha);

	return newValue;

}

void FloatLerper::ResetValues(Uint32 _initialTime, float _interpolationLength, cmf4 _initialValues, cmf4 _finalValues, bool _canGoOver1) {

	initialTime = _initialTime;
	interpolationLength = _interpolationLength;
	initialValues = _initialValues;
	finalValues = _finalValues;
	canGoOver1 = _canGoOver1;

}

