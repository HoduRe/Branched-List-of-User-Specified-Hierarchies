#ifndef __CircularMotionStructs__
#define __CircularMotionStructs__

#include "../glew-2.1.0/include/GL/glew.h"
#include "../SDL/include/SDL.h"
#include "../SDL_image/include/SDL_image.h"
#include "../imgui.h"
#include "../imgui_impl_sdl.h"
#include "../imgui_impl_opengl3.h"
#include "../PugiXml/src/pugixml.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>

struct cmi2 {

	cmi2();
	cmi2(int _value);
	cmi2(int _a, int _b);

	int a, b;

};

struct cmf2 {

	cmf2();
	cmf2(float _value);
	cmf2(float _a, float _b);
	cmf2(cmi2 _int);

	void operator=(cmi2 intIn);

	float a, b;

};

struct cmi4 {

	cmi4();
	cmi4(int _value);
	cmi4(int _abc, int _d);
	cmi4(int _a, int _b, int _c, int _d);

	void Clamp(int componentIndex, int floor, int ceiling);
	void Clamp(int floor, int ceiling);

	//void Width();
	//void Height(); // ... wait, is height + or -...
	void RGBAIn255Range(int* r, int* g, int* blue, int* alpha = nullptr);

	int a, b, c, d;

};

struct cmf4 {

	cmf4();
	cmf4(float _value);
	cmf4(float _abc, float _d);
	cmf4(float _a, float _b, float _c, float _d);
	cmf4(cmi4 _int);

	void operator=(cmi4 intIn);
	bool operator==(cmf4 floatIn);
	void operator* (float mult);

	void Clamp(int componentIndex, float floor, float ceiling);
	void Clamp(float floor, float ceiling);

	//void Width();
	//void Height(); // ... wait, is height + or -...
	void RGBAIn255Range(float* r, float* g, float* blue, float* alpha = nullptr);

	float a, b, c, d;

};

struct Texture {

	Texture();
	Texture(int _width, int _height, GLuint _handle = -1);
	~Texture();

	float GetRatio();
	float GetRatioWidthOverHeight();

	GLuint handle;
	int width, height;

};

struct TextureAdv : Texture {

	TextureAdv();
	TextureAdv(int _width, int _height, GLenum _format, GLint _internalFormat, std::string _directory, GLuint _handle = -1);
	~TextureAdv();

	GLenum format;
	GLint internalFormat;
	std::string directory;

};

struct FloatLerper {

	FloatLerper();
	FloatLerper(Uint32 _initialTime, float _interpolationLength, cmf4 _initialValues, cmf4 _finalValues, bool _canGoOver1);

	cmf4 GetInterpolatedValue(Uint32 newTime);
	void ResetValues(Uint32 _initialTime = 0, float _interpolationLength = 0, cmf4 _initialValues = cmf4(), cmf4 _finalValues = cmf4(), bool _canGoOver1 = false);

	Uint32 initialTime;
	float interpolationLength;
	cmf4 initialValues, finalValues;
	bool canGoOver1;

};

struct ComparativeButton {

	bool DrawButton(std::string& nameTermination);
	bool IsNeutralState();
	bool IsLesserThan();
	bool IsLesserEqualThan();
	bool IsEqualTo();
	bool IsNotEqualTo();
	bool IsBiggerThan();
	bool IsBiggerEqualThan();
	int GetState();
	void SetState(int _newState);
	bool CompareValues(int value1, int value2);

private:

	int buttonState;

};

#endif