#if !defined(HTTP_PARSER_H)
#define HTTP_PARSER_H



int parse_request(const char* request, char* method, size_t method_size, char* path, size_t path_size);
int parse_headers(const char* request, char* headers, size_t headers_size);
int parse_body(const char* request, char* body_recieved, size_t body_size);
int extract_content_length(const char* request);
void parse_body_upto(const char* request, char* body_recieved, size_t length_to_copy);


#endif