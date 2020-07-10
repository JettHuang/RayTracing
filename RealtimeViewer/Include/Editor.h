// \brief
//		Editor.h
//
//

#pragma once

#include "MainWindow.h"

#pragma pack(push, 8)

namespace XEditor
{
	// editor
	class FEditor
	{
	public:
		static FEditor& SharedInstance();

		bool Initialize();
		void Purge();
		void Loop();
		void RequestQuit() { _bRequestQuit = true; }

	protected:
		FEditor();

	protected:
		bool		_bRequestQuit;
		FMainWindow	_MainWindow;

		std::vector<FOSWindow*> _AllWindows;
	};
}

#pragma pack(pop)