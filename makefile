# Compiler and options
OPTS=-fno-pie -no-pie -fno-builtin -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror -std=c17 -Wpedantic -O0 -g

# Target executable
all: hw5

# Build the executable by linking all object files
hw5: hw5.o network_utils.o http_parser.o http_response.o http_errors.o
	gcc $^ -o $@ $(OPTS)

# Compile main file
hw5.o: hw5.c constants.h
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

clean:
	rm -f *.o hw5