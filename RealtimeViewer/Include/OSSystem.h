// \brief
//		OSSystem.h
// 
//

#pragma once

#include <cstdio>
#include <vector>
#include <string>


#pragma pack(push, 8)


namespace XEditor
{

	class FOSSystem
	{
	public:

		/** The file picker mode. */
		enum class EPicker
		{
			Load,	  ///< Load an existing file.
			Directory, ///< open or create a directory.
			Save	   ///< Save to a new or existing file.
		};

		/** Present a filesystem document picker to the user, using native controls.
		 \param mode the type of item to ask to the user (load, save, directory)
		 \param startDir the initial directory when the picker is opened
		 \param outPath the path to the item selected by the user
		 \param extensions (optional) the extensions allowed, separated by "," or ";"
		 \return true if the user picked an item, false if cancel.
		*/
		static bool ShowFilePicker(EPicker mode, const std::string & startDir, std::string & outPath, const std::string & extensions = "");

		static void SetWorkingDirectory(const std::string &InDir);
		static std::string GetWorkingDirectory();

	};
}

#pragma pack(pop)
