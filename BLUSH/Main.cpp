#include "CircularMotion/CircularMotion.h"
#include "BLUSH.h"

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#pragma comment( lib, "glew-2.1.0/lib/Release/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")

// Screen configuration
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false

SDL_Window* window = nullptr;
SDL_Surface* screenSurface = nullptr;
SDL_GLContext context = nullptr;
int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;

bool InitScreen() {

	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP) < 0) { ret = false; }

	else {

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (WIN_FULLSCREEN == true) { flags |= SDL_WINDOW_FULLSCREEN; }
		if (WIN_RESIZABLE == true) { flags |= SDL_WINDOW_RESIZABLE; }
		if (WIN_BORDERLESS == true) { flags |= SDL_WINDOW_BORDERLESS; }
		if (WIN_FULLSCREEN_DESKTOP == true) { flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; }

		window = SDL_CreateWindow("Circular Motion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL) { ret = false; }
		else { screenSurface = SDL_GetWindowSurface(window); }

	}

	context = SDL_GL_CreateContext(window);
	if (context == NULL) { ret = false; }

	GLenum glewError = glewInit();
	GLenum error = glGetError();
	if (glewError != GLEW_OK || error != GL_NO_ERROR) { return false; }

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = NULL;

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 430");

	return true;
}

void StartFrame() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	glClearColor(0.0f, 0.0f, 0.004f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void EndFrame() {

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);

}

int main(int argc, char** argv) {

	if (!InitScreen()) { return 1; }

	bool mainLoop = true;
	cmi2 windowSize(width, height);
	BLUSH blushApp(window, width, height);

	while (mainLoop == true) {

		SDL_Event e;
		while (SDL_PollEvent(&e)) {

			ImGui_ImplSDL2_ProcessEvent(&e);
			switch (e.type) {

			case SDL_WINDOWEVENT: if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) { SDL_GetWindowSize(window, &windowSize.a, &windowSize.b); } break;
			case SDL_QUIT: mainLoop = false; break;

			}

		}

		StartFrame();

		///////////////////////
		
		blushApp.Update();
		ImGuiBase::DrawErrorMessage();

		///////////////////////

		EndFrame();

	}

	SDL_GL_DeleteContext(context);
	if (window != NULL) { SDL_DestroyWindow(window); }
	SDL_Quit();

	return 0;

}



