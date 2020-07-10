// \brief
//		Editor.cpp
//


#include "Editor.h"
#include "OSSystem.h"


namespace XEditor
{
	static const char* kMainWindowTitle = "RealTime Viewer";

	FEditor::FEditor()
	{

	}

	FEditor& FEditor::SharedInstance()
	{
		static FEditor editor;

		return editor;
	}

	bool FEditor::Initialize()
	{
		FOSSystem::SetWorkingDirectory("../../Resource/");

		InitializeWindowSys();

		_MainWindow.Initialize(kMainWindowTitle, 0, 0);
		_AllWindows.push_back(&_MainWindow);
		return true;
	}

	void FEditor::Purge()
	{
		_MainWindow.Uninitialize();
		_AllWindows.clear();
		UninitializeWindowSys();
	}

	void FEditor::Loop()
	{
		_bRequestQuit = false;

		Uint32 LastTicks = SDL_GetTicks();
		while (!_bRequestQuit) 
		{
			SDL_Event event;
			while (SDL_PollEvent(&event)) 
			{
				/* handle your event here */
				switch (event.type)
				{
				case SDL_QUIT:
					RequestQuit();
					break;
				}

				for (size_t i = 0; i < _AllWindows.size(); i++)
				{
					_AllWindows[i]->ProcessEvent(event);
				}
			} // end while
			
			Uint32 CurTicks = SDL_GetTicks();
			float ElapseSeconds = (CurTicks+1 - LastTicks) / 1000.f;
			LastTicks = CurTicks;

			//XLOG(Log_Info, "elapse=%f", ElapseSeconds);
			for (size_t i = 0; i < _AllWindows.size(); i++)
			{
				_AllWindows[i]->Tick(ElapseSeconds);
			}
		} // end while
	}

}
