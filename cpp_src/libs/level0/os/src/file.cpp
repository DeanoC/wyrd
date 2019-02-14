
#include <cstdio>
#include "core/logger.h"
#include "os/file.h"

namespace {
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

EXTERN_C File_Handle File_Open(const char* filename, const File_Mode mode)
{
	char flags[4];
	TranslateFileAccessFlags(mode, flags, 4);
	FILE* fp = fopen(filename, flags);
	return fp;
}

EXTERN_C bool File_close(File_Handle handle)
{
	return (fclose((::FILE*) handle) == 0);
}

EXTERN_C void File_Flush(File_Handle handle)
{
	fflush((::FILE*) handle);
}

EXTERN_C size_t File_Read(File_Handle handle, void* buffer, size_t byteCount)
{
	return fread(buffer,
				 1,
				 byteCount,
				 (::FILE*) handle);
}

EXTERN_C bool File_Seek(File_Handle handle, int64_t offset, File_SeekDir origin)
{
	return fseek((::FILE*) handle, offset, origin) == 0;
}

EXTERN_C int64_t File_Tell(File_Handle handle)
{
	return ftell((::FILE*) handle);
}

EXTERN_C size_t File_Write(File_Handle handle, void const * buffer, size_t byteCount)
{
	return fwrite(buffer,
				  1,
				  byteCount,
				  (::FILE*) handle);
}
