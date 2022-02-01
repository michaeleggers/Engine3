#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdint.h>
#include <string>

enum ATP_Status
{
    ATP_SUCCESS,
    ATP_ERROR_READ_FILE,
    ATP_ERROR_NO_FILE
};

struct ATP_File
{
    uint8_t  * data;
    uint32_t   size;
};


ATP_Status  atp_read_file(char const * filename, ATP_File * out_File);
ATP_Status  atp_destroy_file(ATP_File * file);
std::string atp_get_exe_path(void);

#endif
