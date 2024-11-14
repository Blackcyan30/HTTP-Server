#if !defined(HTTP_PARSER_H)
#define HTTP_PARSER_H



int parse_request(const char* request, char* method, size_t method_size, char* path, size_t path_size);

int parse_headers(const char* request, char* headers, size_t headers_size);


#endif