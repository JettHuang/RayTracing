// \brief
//		OSWindow.cpp
//

#include "OSWindow.h"
#include "SDL_syswm.h"
#include <iostream>
#include <algorithm>


namespace XEditor
{
	// initialize window system.
	bool InitializeWindowSys()
	{
		// Init SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::cerr << "Could not initialize SDL.\nError: " << SDL_GetError() << std::endl;
			return false;
		}

		// Enable depth buffer.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		// Set color channel depth.
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		// 4x MSAA.
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		// setup dear imgui
		IMGUI_CHECKVERSION();

		return true;
	}

	void UninitializeWindowSys()
	{
		SDL_Quit();
	}

	FRestoreSDLGLContext::FRestoreSDLGLContext()
	{
		_SavedWindow = SDL_GL_GetCurrentWindow();
		_SavedGLCtx = SDL_GL_GetCurrentContext();
	}

	FRestoreSDLGLContext::~FRestoreSDLGLContext()
	{
		SDL_GL_MakeCurrent(_SavedWindow, _SavedGLCtx);
	}

	FRestoreImGuiContext::FRestoreImGuiContext()
	{
		_SavedGuiCtx = ImGui::GetCurrentContext();
	}

	FRestoreImGuiContext::~FRestoreImGuiContext()
	{
		ImGui::SetCurrentContext(_SavedGuiCtx);
	}

///////////////////////////////////////////////////////////////////////////////////
	FOSWindow::FOSWindow()
		: _GLContext(nullptr)
		, _SDLWindow(nullptr)
		, _ImGuiCtx(nullptr)
		, _ImGuiIO(nullptr)
		, _FontTexture(0)
	{

	}
	 
	FOSWindow::~FOSWindow()
	{
		assert(_GLContext == nullptr);
		assert(_SDLWindow == nullptr);
		assert(_ImGuiCtx == nullptr);
		assert(_ImGuiIO == nullptr);
		assert(_FontTexture == 0);
	}

	bool FOSWindow::Initialize(const char* InCaption, int32_t InWidth, int32_t InHeight)
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		if (InWidth <= 0 || InHeight <= 0)
		{
			SDL_DisplayMode displayMode;
			SDL_GetCurrentDisplayMode(0, &displayMode);

			bool presentationMode = false;
			if (presentationMode)
			{
				// Create a fullscreen window at the native resolution.
				InWidth = displayMode.w;
				InHeight = displayMode.h;
				flags |= SDL_WINDOW_FULLSCREEN;
			}
			else
			{
				float aspect = 16.0f / 9.0f;
				InWidth = std::min(displayMode.w, (int)(displayMode.h * aspect)) - 80;
				InHeight = displayMode.h - 80;
			}
		}

		if (!InCaption)
		{
			InCaption = "Window";
		}
		_SDLWindow = SDL_CreateWindow(InCaption, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, InWidth, InHeight, flags);

		if (_SDLWindow == NULL)
		{
			std::cerr << "Could not initialise SDL opengl\nError: " << SDL_GetError() << std::endl;
			return false;
		}

		FRestoreSDLGLContext  SavedGL;
		FRestoreImGuiContext SavedIm;
		{
			// create gl-contex && make current context
			{
				_GLContext = SDL_GL_CreateContext(_SDLWindow);
			}

			// create imgui context
			{
				_ImGuiCtx = ImGui::CreateContext();
				ImGui::SetCurrentContext(_ImGuiCtx);
				ImGui::StyleColorsDark();

				_ImGuiIO = &ImGui::GetIO();
				ImFont* font = _ImGuiIO->Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);

				// create imgui renderer.
				ImGui_ImplOpenGL2_Init();
				ImGui_ImplSDL_Init();
			}

			OnPostInitialize();
		}

		return true;
	}

	void FOSWindow::Uninitialize()
	{
		FRestoreSDLGLContext  SavedGL;
		FRestoreImGuiContext SavedIm;

		SDL_GL_MakeCurrent(_SDLWindow, _GLContext);
		ImGui::SetCurrentContext(_ImGuiCtx);

		// do prev uninialize
		OnPrevUninitalize();

		ImGui_ImplSDL_Shutdown();
		ImGui_ImplOpenGL2_Shutdown();
		// destory imgui
		if (_ImGuiCtx)
		{
			ImGui::DestroyContext(_ImGuiCtx);
			_ImGuiCtx = nullptr;
			_ImGuiIO = nullptr;
		}

		// destory gl-context && windows
		if (_GLContext)
		{
			SDL_GL_DeleteContext(_GLContext);
			_GLContext = nullptr;
		}
		if (_SDLWindow)
		{
			SDL_DestroyWindow(_SDLWindow);
			_SDLWindow = nullptr;
		}
	}

	/* refer to http://wiki.libsdl.org/SDL_Event */
	bool FOSWindow::IsRelevantToEvent(const SDL_Event& InEvent)
	{
		if (_SDLWindow == nullptr)
		{
			return false;
		}

		Uint32 windowId = -1;
		switch (InEvent.type)
		{
		case SDL_DROPFILE:
		case SDL_DROPTEXT:
		case SDL_DROPBEGIN:
		case SDL_DROPCOMPLETE:
			windowId = InEvent.drop.windowID;
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			windowId = InEvent.key.windowID;
			break;

		case SDL_MOUSEMOTION:
			windowId = InEvent.motion.windowID;
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			windowId = InEvent.button.windowID;
			break;

		case SDL_MOUSEWHEEL:
			windowId = InEvent.wheel.windowID;
			break;
		
		case SDL_TEXTEDITING:
			windowId = InEvent.edit.windowID;
			break;

		case SDL_TEXTINPUT:
			windowId = InEvent.text.windowID;
			break;

		case SDL_WINDOWEVENT:
			windowId = InEvent.window.windowID;
			break;
		default:
			break;
		}

		return (SDL_GetWindowID(_SDLWindow) == windowId);
	}

	// process sdl event
	void FOSWindow::ProcessEvent(const SDL_Event& InEvent)
	{
		if (!IsRelevantToEvent(InEvent))
		{
			return;
		}

		switch (InEvent.type)
		{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			ImGui_ImplSDL_KeyCallback(InEvent);
			if (!_ImGuiIO->WantCaptureKeyboard)
			{
				(InEvent.type == SDL_KEYDOWN) ? OnKeyDown(InEvent) : OnKeyUp(InEvent);
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			ImGui_ImplSDL_MouseButtonCallback(InEvent);
			if (!_ImGuiIO->WantCaptureMouse)
			{
				(InEvent.type == SDL_MOUSEBUTTONDOWN) ? OnMouseButtonDown(InEvent) : OnMouseButtonUp(InEvent);
			}
			break;

		case SDL_MOUSEWHEEL:
			ImGui_ImplSDL_MouseWheelCallback(InEvent);
			if (!_ImGuiIO->WantCaptureMouse)
			{
				OnMouseWheel(InEvent);
			}
			break;

		case SDL_MOUSEMOTION:
			if (!_ImGuiIO->WantCaptureMouse)
			{
				OnMouseMove(InEvent);
			}
			break;
		case SDL_TEXTINPUT:
			if (_ImGuiIO->WantTextInput)
			{
				ImGui_ImplSDL_CharCallback(InEvent);
			}
			else
			{
				OnTextInputUFT8(InEvent.text.text);
			}
			break;
		case SDL_WINDOWEVENT:
			{
				switch (InEvent.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					OnWndClosed();
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}

	}

	void FOSWindow::Tick(float InSeconds)
	{
		SDL_GL_MakeCurrent(_SDLWindow, _GLContext);
		ImGui::SetCurrentContext(_ImGuiCtx);
	}

	void FOSWindow::BeginUI(float InSeconds)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL_NewFrame(InSeconds);
		ImGui::NewFrame();
	}

	void FOSWindow::EndUI()
	{
		ImGui::EndFrame();
		ImGui::Render();

		int w, h;
		SDL_GL_GetDrawableSize(_SDLWindow, &w, &h);
		glViewport(0, 0, w, h);
		// rendering...
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}

///////////////////////////////////////////////////////////////////////////////////////
// ImGui Backend renderer
// Functions
	bool FOSWindow::ImGui_ImplOpenGL2_Init()
	{
		// Setup back-end capabilities flags
		assert(_ImGuiIO);
		_ImGuiIO->BackendRendererName = "imgui_impl_opengl2";
		return true;
	}

	void FOSWindow::ImGui_ImplOpenGL2_Shutdown()
	{
		ImGui_ImplOpenGL2_DestroyDeviceObjects();
	}

	void FOSWindow::ImGui_ImplOpenGL2_NewFrame()
	{
		if (!_FontTexture)
			ImGui_ImplOpenGL2_CreateDeviceObjects();
	}

	static void ImGui_ImplOpenGL2_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height)
	{
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_SCISSOR_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
		// you may need to backup/reset/restore current shader using the lines below. DO NOT MODIFY THIS FILE! Add the code in your calling function:
		//  GLint last_program;
		//  glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		//  glUseProgram(0);
		//  ImGui_ImplOpenGL2_RenderDrawData(...);
		//  glUseProgram(last_program)

		// Setup viewport, orthographic projection matrix
		// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
		glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x, draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y, -1.0f, +1.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	// OpenGL2 Render function.
	// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
	// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
	void FOSWindow::ImGui_ImplOpenGL2_RenderDrawData(ImDrawData* draw_data)
	{
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;

		// Backup GL state
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);

		// Setup desired GL state
		ImGui_ImplOpenGL2_SetupRenderState(draw_data, fb_width, fb_height);

		// Will project scissor/clipping rectangles into framebuffer space
		ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
		ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
			glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, pos)));
			glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, uv)));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, col)));

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					// User callback, registered via ImDrawList::AddCallback()
					// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
					if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
						ImGui_ImplOpenGL2_SetupRenderState(draw_data, fb_width, fb_height);
					else
						pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					// Project scissor/clipping rectangles into framebuffer space
					ImVec4 clip_rect;
					clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
					clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
					clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
					clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

					if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
					{
						// Apply scissor/clipping rectangle
						glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

						// Bind texture, Draw
						glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
					}
				}
				idx_buffer += pcmd->ElemCount;
			}
		}

		// Restore modified GL state
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
		glPolygonMode(GL_FRONT, (GLenum)last_polygon_mode[0]); glPolygonMode(GL_BACK, (GLenum)last_polygon_mode[1]);
		glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
	}

	bool FOSWindow::ImGui_ImplOpenGL2_CreateFontsTexture()
	{
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

		// Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &_FontTexture);
		glBindTexture(GL_TEXTURE_2D, _FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (ImTextureID)(intptr_t)_FontTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture);

		return true;
	}

	void FOSWindow::ImGui_ImplOpenGL2_DestroyFontsTexture()
	{
		if (_FontTexture)
		{
			ImGuiIO& io = ImGui::GetIO();
			glDeleteTextures(1, &_FontTexture);
			io.Fonts->TexID = 0;
			_FontTexture = 0;
		}
	}

	bool FOSWindow::ImGui_ImplOpenGL2_CreateDeviceObjects()
	{
		return ImGui_ImplOpenGL2_CreateFontsTexture();
	}

	void FOSWindow::ImGui_ImplOpenGL2_DestroyDeviceObjects()
	{
		ImGui_ImplOpenGL2_DestroyFontsTexture();
	}

///////////////////////////////////////////////////////////////////////////////////////////
// platform
	static const char* ImGui_ImplSDL_GetClipboardText(void* user_data)
	{
		FOSWindow* pWindow = (FOSWindow*)user_data;
		char* Text = SDL_GetClipboardText();
		pWindow->SetClipboardText(Text);
		if (Text)
		{
			SDL_free(Text);
		}

		return pWindow->GetClipboardText();
	}

	static void ImGui_ImplSDL_SetClipboardText(void* user_data, const char* text)
	{
		SDL_SetClipboardText(text);
	}

	// clipboard text
	const char* FOSWindow::GetClipboardText() const
	{
		if (_ClipboardText.empty())
		{
			return nullptr;
		}

		return _ClipboardText.data();
	}

	void FOSWindow::SetClipboardText(const char* InText)
	{
		if (InText)
		{
			size_t len = strlen(InText) + 1;
			if (_ClipboardText.size() < len)
			{
				_ClipboardText.resize(len);
			}

			strcpy(_ClipboardText.data(), InText);
		}
		else
		{
			_ClipboardText.clear();
		}
	}

	void FOSWindow::SetWindowCaption(const char* szCaption)
	{
		SDL_SetWindowTitle(_SDLWindow, szCaption);
	}

	bool FOSWindow::ImGui_ImplSDL_Init()
	{
		// Setup back-end capabilities flags
		ImGuiIO& io = *_ImGuiIO;

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.BackendPlatformName = "imgui_impl_sdl";

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
		io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
		io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
		io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
		io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
		io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
		io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
		io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
		io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
		io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

		io.SetClipboardTextFn = ImGui_ImplSDL_SetClipboardText;
		io.GetClipboardTextFn = ImGui_ImplSDL_GetClipboardText;
		io.ClipboardUserData = this;
#if defined(_WIN32)
		SDL_SysWMinfo info;
		if (SDL_GetWindowWMInfo(_SDLWindow, &info))
		{
			io.ImeWindowHandle = (void*)info.info.win.window;
		}
#endif

		_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW); 
		_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
		_MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

		return true;
	}

	void FOSWindow::ImGui_ImplSDL_Shutdown()
	{
		for (int32_t i = 0; i < ImGuiMouseCursor_COUNT; i++)
		{
			SDL_FreeCursor(_MouseCursors[i]);
			_MouseCursors[i] = nullptr;
		}
	}

	void FOSWindow::ImGui_ImplSDL_NewFrame(float InSeconds)
	{
		ImGuiIO &io = *_ImGuiIO;

		// setup display size
		int w, h, dw, dh;
		SDL_GetWindowSize(_SDLWindow, &w, &h);
		SDL_GL_GetDrawableSize(_SDLWindow, &dw, &dh);
		io.DisplaySize = ImVec2((float)w, (float)h);
		if (w > 0 && h > 0)
		{
			io.DisplayFramebufferScale = ImVec2((float)dw / (float)w, (float)dh / (float)h);
		}

		// setup time step
		io.DeltaTime = InSeconds;

		ImGui_ImplSDL_UpdateMousePosAndButtons();
		ImGui_ImplSDL_UpdateMouseCursor();
	}

	void FOSWindow::ImGui_ImplSDL_UpdateMousePosAndButtons()
	{
		ImGuiIO& io = *_ImGuiIO;

		// Update mouse position
		const ImVec2 mouse_pos_backup = io.MousePos;
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
		const bool focused = true; // Emscripten
#else
		const bool focused = SDL_GetMouseFocus() == _SDLWindow;
#endif
		if (focused)
		{
			if (io.WantSetMousePos)
			{
				SDL_WarpMouseInWindow(_SDLWindow, (int)mouse_pos_backup.x, (int)mouse_pos_backup.y);
			}
			else
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				io.MousePos = ImVec2((float)x, (float)y);
			}
		}
	}

	void FOSWindow::ImGui_ImplSDL_UpdateMouseCursor()
	{
		ImGuiIO& io = *_ImGuiIO;
		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
		{
			return;
		}

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			SDL_ShowCursor(SDL_DISABLE);
		}
		else
		{
			// Show OS mouse cursor
			// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
			SDL_SetCursor(_MouseCursors[imgui_cursor] ? _MouseCursors[imgui_cursor] : _MouseCursors[ImGuiMouseCursor_Arrow]);
			SDL_ShowCursor(SDL_ENABLE);
		}
	}

	static int32_t SDLMouseBtnToImGui(int32_t InBtn)
	{
		int32_t Index = InBtn;
		switch (InBtn)
		{
		case SDL_BUTTON_LEFT:
			Index = 0;
			break;
		case SDL_BUTTON_RIGHT:
			Index = 1;
			break;
		case SDL_BUTTON_MIDDLE:
			Index = 2;
			break;
		default:
			break;
		}

		return Index;
	}

	void FOSWindow::ImGui_ImplSDL_MouseButtonCallback(const SDL_Event& InEvent)
	{
		ImGuiIO& io = *_ImGuiIO;

		int32_t Index = SDLMouseBtnToImGui(InEvent.button.button);
		if (Index >= IM_ARRAYSIZE(io.MouseDown))
		{
			return;
		}

		switch (InEvent.button.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			io.MouseDown[Index] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			io.MouseDown[Index] = false;
		default:
			break;
		}
	}

	void FOSWindow::ImGui_ImplSDL_MouseWheelCallback(const SDL_Event& InEvent)
	{
		ImGuiIO& io = *_ImGuiIO;

		io.MouseWheel += InEvent.wheel.y;
		io.MouseWheelH += InEvent.wheel.x;
	}

	void FOSWindow::ImGui_ImplSDL_KeyCallback(const SDL_Event& InEvent)
	{
		ImGuiIO& io = *_ImGuiIO;

		SDL_Scancode Key = InEvent.key.keysym.scancode;
		Uint16 Modifier = InEvent.key.keysym.mod;
		switch (InEvent.key.type)
		{
		case SDL_KEYDOWN:
			io.KeysDown[Key] = true;
			break;
		case SDL_KEYUP:
			io.KeysDown[Key] = false;
			break;
		default:
			break;
		}

		io.KeyCtrl = Modifier & KMOD_CTRL;
		io.KeyShift = Modifier & KMOD_SHIFT;
		io.KeyAlt = Modifier & KMOD_ALT;
	}

	void FOSWindow::ImGui_ImplSDL_CharCallback(const SDL_Event& InEvent)
	{
		ImGuiIO& io = *_ImGuiIO;

		io.AddInputCharactersUTF8(InEvent.text.text);
	}
}
