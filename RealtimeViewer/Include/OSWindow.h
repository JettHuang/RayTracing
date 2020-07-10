// \brief
//		OSWindow.h
//  Window to render UI & handle event
// 
//

#pragma once

#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>

#include "SDL.h"
#include "SDL_opengl.h"
#ifdef __APPLE__
#	include <OpenGL/glu.h>
#else
#	include <GL/glu.h>
#endif

#include "imgui.h"
#include "imgui_internal.h"

#ifdef WIN32
#	define snprintf _snprintf
#	define putenv _putenv
#endif

#include <vector>

#pragma pack(push, 8)


namespace XEditor
{

	// initialize window system.
	bool InitializeWindowSys();
	void UninitializeWindowSys();

	// save/restore sdl-gl context
	class FRestoreSDLGLContext
	{
	public:
		FRestoreSDLGLContext();
		~FRestoreSDLGLContext();

		SDL_Window* _SavedWindow;
		SDL_GLContext _SavedGLCtx;
	};

	// save/restore imgui context
	class FRestoreImGuiContext
	{
	public:
		FRestoreImGuiContext();
		~FRestoreImGuiContext();

		ImGuiContext* _SavedGuiCtx;
	};

	// class Window
	class FOSWindow
	{
	public:
		FOSWindow();
		virtual ~FOSWindow();

		bool Initialize(const char* InCaption, int32_t InWidth, int32_t InHeight);
		void Uninitialize();

		// process sdl event
		virtual void ProcessEvent(const SDL_Event& InEvent);
		// tick
		virtual void Tick(float InSeconds);

		// Window Message
		virtual void OnKeyDown(const SDL_Event& InEvent) {}
		virtual void OnKeyUp(const SDL_Event& InEvent) {}
		virtual void OnMouseButtonDown(const SDL_Event& InEvent) {}
		virtual void OnMouseButtonUp(const SDL_Event& InEvent) {}
		virtual void OnMouseWheel(const SDL_Event& InEvent) {}
		virtual void OnMouseMove(const SDL_Event& InEvent) {}
		virtual void OnTextInputUFT8(const char* InText) {}

		virtual void OnWndClosed() {}

		// clipboard text
		const char* GetClipboardText() const;
		void SetClipboardText(const char* InText);

		void SetWindowCaption(const char* szCaption);
	protected:
		virtual bool IsRelevantToEvent(const SDL_Event& InEvent);
		virtual void OnPostInitialize() { /* do something */ }
		virtual void OnPrevUninitalize() { /* do something */ }

		// invoke it when you draw imgui
		void BeginUI(float InSeconds);
		void EndUI();

		bool ImGui_ImplOpenGL2_Init();
		void ImGui_ImplOpenGL2_Shutdown();
		void ImGui_ImplOpenGL2_NewFrame();
		void ImGui_ImplOpenGL2_RenderDrawData(ImDrawData* draw_data);

		// Called by Init/NewFrame/Shutdown
		bool ImGui_ImplOpenGL2_CreateFontsTexture();
		void ImGui_ImplOpenGL2_DestroyFontsTexture();
		bool ImGui_ImplOpenGL2_CreateDeviceObjects();
		void ImGui_ImplOpenGL2_DestroyDeviceObjects();

		// platform
		bool ImGui_ImplSDL_Init();
		void ImGui_ImplSDL_Shutdown();
		void ImGui_ImplSDL_NewFrame(float InSeconds);

		void ImGui_ImplSDL_UpdateMousePosAndButtons();
		void ImGui_ImplSDL_UpdateMouseCursor();

		void ImGui_ImplSDL_MouseButtonCallback(const SDL_Event& InEvent);
		void ImGui_ImplSDL_MouseWheelCallback(const SDL_Event& InEvent);
		void ImGui_ImplSDL_KeyCallback(const SDL_Event& InEvent);
		void ImGui_ImplSDL_CharCallback(const SDL_Event& InEvent);

	protected:
		SDL_GLContext	_GLContext;
		SDL_Window	   *_SDLWindow;
		ImGuiContext   *_ImGuiCtx;
		ImGuiIO		   *_ImGuiIO;
		GLuint          _FontTexture;

		// platform resource
		SDL_Cursor*		_MouseCursors[ImGuiMouseCursor_COUNT];
		std::vector<char> _ClipboardText;
	};
}

#pragma pack(pop)

