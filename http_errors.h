#ifndef HTTP_ERRORS_H
#define HTTP_ERRORS_H

#include "constants.h"

void raise_http_error(int error_code, int* HSIZE, int* BSIZE, char* header, char* body);

#endif