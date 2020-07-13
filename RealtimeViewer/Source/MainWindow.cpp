// \brief
//	MainWindow.cpp
//

#include "MainWindow.h"
#include "Editor.h"
#include "OSSystem.h"
#include "SOIL.h"


namespace XEditor
{
	FMainWindow::FMainWindow()
	{
		clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	}

	FMainWindow::~FMainWindow()
	{

	}

	// process sdl event
	void FMainWindow::ProcessEvent(const SDL_Event& InEvent)
	{
		if (!IsRelevantToEvent(InEvent))
		{
			return;
		}

		Super::ProcessEvent(InEvent);
	}

	// tick
	void FMainWindow::Tick(float InSeconds)
	{
		Super::Tick(InSeconds);

		int w, h;
		SDL_GL_GetDrawableSize(_SDLWindow, &w, &h);
		glViewport(0, 0, w, h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// render scene
		RenderScene(InSeconds);

		// render ui
		RenderUI(InSeconds);

		SDL_GL_SwapWindow(_SDLWindow);
	}

	void FMainWindow::OnPostInitialize() 
	{
		/* do something */ 
	}

	void FMainWindow::OnPrevUninitalize() 
	{ 
		/* do something */ 
	}

	void FMainWindow::OnWndClosed()
	{
		SDL_Event event;

		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}

	void FMainWindow::RenderUI(float InDelta)
	{
		BeginUI(InDelta);

		RenderToolWindow();

		EndUI();
	}

	void FMainWindow::ToolTip(const char* InText)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(InText);
			ImGui::EndTooltip();
		}
	}

	void FMainWindow::RenderToolWindow()
	{
		const float DISTANCE = 10.0f;
		static int corner = 1; // 0: left, 1:right, -1:movable
		bool bOpened = true;

		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("Tool Window", &bOpened, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
			ImGui::Separator();
		}
		ImGui::End();
	}

	void FMainWindow::RenderScene(float InDelta)
	{

	}
}