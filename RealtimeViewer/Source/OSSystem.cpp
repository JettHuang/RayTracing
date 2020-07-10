// \brief
//		OSWindow.cpp
//

#include "OSSystem.h"
#include "nfd.h"
#include "nfd_common.h"
#include <Windows.h>
#include <iostream>


namespace XEditor
{

	bool FOSSystem::ShowFilePicker(EPicker mode, const std::string & startDir, std::string & outPath, const std::string & extensions)
	{
		nfdchar_t * outPathRaw = nullptr;
		nfdresult_t result = NFD_CANCEL;
		outPath = "";

#ifdef _WIN32
		(void)startDir;
		const std::string internalStartPath;
#else
		const std::string internalStartPath = startDir;
#endif
		if (mode == EPicker::Load) {
			result = NFD_OpenDialog(extensions.empty() ? nullptr : extensions.c_str(), internalStartPath.c_str(), &outPathRaw);
		}
		else if (mode == EPicker::Save) {
			result = NFD_SaveDialog(extensions.empty() ? nullptr : extensions.c_str(), internalStartPath.c_str(), &outPathRaw);
		}
		else if (mode == EPicker::Directory) {
			result = NFD_PickFolder(internalStartPath.c_str(), &outPathRaw);
		}

		if (result == NFD_OKAY) {
			outPath = std::string(outPathRaw);
			NFDi_Free(outPathRaw);
		}
		else if (result == NFD_CANCEL) {
			// Canceled by user, nothing to do.
		}
		else {
			std::cerr << "ShowFilePicker Error: " << NFD_GetError() << std::endl;
		}
		
		return (result == NFD_OKAY);
	}

	void FOSSystem::SetWorkingDirectory(const std::string &InDir)
	{
		::SetCurrentDirectoryA(InDir.c_str());
	}

	std::string FOSSystem::GetWorkingDirectory()
	{
		char szBuf[MAX_PATH+1];

		::GetCurrentDirectoryA(sizeof(szBuf), szBuf);
		return std::string(szBuf);
	}
}

