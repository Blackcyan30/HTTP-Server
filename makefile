# Compiler and options
OPTS=-fno-pie -no-pie -fno-builtin -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror -std=c17 -Wpedantic -O0 -g

# Target executable
all: main

# Build the executable by linking all object files
main: main.o server_config.o network_utils.o http_parser.o http_response.o http_errors.o http_method_handler.o storage.o
    gcc $^ -o $@ $(OPTS)

# Compile main file
main.o: main.c constants.h 
    gcc $< -c -o $@ $(OPTS)

server_config.o: server_config.c client_session.h
    gcc $< -c -o $@ $(OPTS)

# Compile individual modules
network_utils.o: network_utils.c constants.h 
    gcc $< -c -o $@ $(OPTS)

http_parser.o: http_parser.c constants.h 
    gcc $< -c -o $@ $(OPTS)

http_response.o: http_response.c constants.h 
    gcc $< -c -o $@ $(OPTS)

http_errors.o: http_errors.c constants.h 
    gcc $< -c -o $@ $(OPTS)

http_method_handler.o: http_method_handler.c constants.h 
    gcc $< -c -o $@ $(OPTS)

storage.o: storage.c constants.h 
    gcc $< -c -o $@ $(OPTS)

clean:
    rm -f *.o main