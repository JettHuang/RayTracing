// \brief
//	Main Window of Editor
//

#pragma once

#include "OSWindow.h"

#pragma pack(push, 8)


namespace XEditor
{

	// Main Window
	class FMainWindow : public FOSWindow
	{
	public:
		typedef FOSWindow Super;

		FMainWindow();
		virtual ~FMainWindow();

		// process sdl event
		virtual void ProcessEvent(const SDL_Event& InEvent) override;
		// tick
		virtual void Tick(float InSeconds) override;

		virtual void OnPostInitialize() override;
		virtual void OnPrevUninitalize() override;
		// handle window close
		virtual void OnWndClosed() override;

	protected:
		// world scene
		void RenderScene(float InDelta);
		// ui windows...
		void RenderUI(float InDelta);

		// draw tool window
		void RenderToolWindow();

		void ToolTip(const char* InText);
	protected:
		ImVec4 clear_color;
	};
}

#pragma pack(pop)
