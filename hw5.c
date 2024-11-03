#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define RMAX 4096
#define HMAX 1024
#define BMAX 1024

static char request[RMAX+1];

static int HSIZE = 0;
static char header[HMAX];

static int BSIZE = 0;
static char body[BMAX];


int main(int argc, char * argv[])
{
    assert(argc == 2);
    int port = atoi(argv[1]);

    return 0;
}
