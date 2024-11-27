#ifndef HTTP_ERRORS_H
#define HTTP_ERRORS_H

#include "constants.h"
#include "client_session.h"

void raise_http_error(int error_code, client_session_t* client_info);

#endif