// \brief
//		Main cpp file of Editor.
//

#include "Editor.h"

using namespace XEditor;


int main(int argc, char* argv[])
{
	FEditor& MyEditor = FEditor::SharedInstance();
	
	if (MyEditor.Initialize())
	{
		MyEditor.Loop();
		MyEditor.Purge();
	}

	return 0;
}
