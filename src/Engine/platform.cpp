#include "platform.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef   min
#undef   max
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string>

ATP_Status atp_read_file(char const * filename, ATP_File * out_File)
{
    HANDLE fileHandle;
    fileHandle = CreateFile(
		filename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
		printf("unable to open file: %s\n", filename);
		return ATP_ERROR_READ_FILE;
    }

    DWORD filesize = GetFileSize(fileHandle, NULL);
/*
    out_File->data = (uint8_t*)VirtualAlloc(
	NULL,
	(filesize) * sizeof(unsigned char),
	MEM_RESERVE | MEM_COMMIT,
	PAGE_READWRITE);
*/
    out_File->data = (uint8_t*)malloc(filesize*sizeof(uint8_t));
    
    if (out_File->data == NULL) {
		return ATP_ERROR_READ_FILE;
    }
    
    struct _OVERLAPPED ov = { };
    DWORD numBytesRead = 0;
    DWORD error;
    if (ReadFile(fileHandle, out_File->data, filesize, &numBytesRead, NULL) == 0)
    {
		error = GetLastError();
		char errorMsgBuf[256];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			errorMsgBuf, (sizeof(errorMsgBuf) / sizeof(char)), NULL);

		printf("%s\n", errorMsgBuf);
    }
	else
    {
		out_File->size = filesize;
    }
    CloseHandle(fileHandle);

    return ATP_SUCCESS;
}

ATP_Status atp_destroy_file(ATP_File * file)
{
    if (file->data != NULL) {
		free(file->data);
		file->size = 0;

		return ATP_SUCCESS;
    }

    return ATP_ERROR_NO_FILE;
}

std::string atp_get_exe_path(void)
{
    char out_buffer[256];
    int  buffer_size = 256;
    DWORD len = GetModuleFileNameA(NULL, out_buffer, buffer_size);
    if ( !len ) {
	DWORD error = GetLastError();
	char errorMsgBuf[256];
	FormatMessage(
	    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    errorMsgBuf, (sizeof(errorMsgBuf) / sizeof(char)), NULL);
	
	printf("%s\n", errorMsgBuf);
    }

    // strip actual name of the .exe
    char * last = out_buffer + len;
    while ( *last != '\\') {
	*last-- = '\0';
    }

    return std::string(out_buffer);
}

#elif __APPLE__

#elif __linux__

#endif
