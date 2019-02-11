
#include "core/logger.h"
#include "os/file.h"
#include "file_details.hpp"

namespace FileDetails {
void TranslateFileAccessFlags(File_Mode modeFlags, char* fileAccesString, int strLength)
{
	ASSERT(fileAccesString != NULL && strLength >= 4);
	memset(fileAccesString, '\0', strLength);
	int index = 0;

	// Read + Write uses w+ then filemode (b or t)
	if(modeFlags & FM_Read && modeFlags & FM_Write)
	{
		fileAccesString[index++] = 'w';
		fileAccesString[index++] = '+';
	}
		// Read + Append uses a+ then filemode (b or t)
	else if(modeFlags & FM_Read && modeFlags & FM_Append)
	{
		fileAccesString[index++] = 'a';
		fileAccesString[index++] = '+';
	} else
	{
		if(modeFlags & FM_Read)
			fileAccesString[index++] = 'r';
		if(modeFlags & FM_Write)
			fileAccesString[index++] = 'w';
		if(modeFlags & FM_Append)
			fileAccesString[index++] = 'a';
	}

	if(modeFlags & FM_Binary)
		fileAccesString[index++] = 'b';
	else
		fileAccesString[index++] = 't';

	fileAccesString[index++] = '\0';
}

}
