#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {
  int fd;
  int nread;
  struct sockaddr_in servaddr;
  int BUFSIZE = -1;
  char ip[255] = {'\0'};
  int port = -1;
  
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"BUFSIZE", required_argument, 0, 0},
                                      {"ip", required_argument, 0, 0},
                                      {"port", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;
    
    FILE *fp;
    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        BUFSIZE = atoi(optarg);
        // TODO: your code here
        if (BUFSIZE <= 0 )
        {
            printf("Error: BUFSIZE<=0\n");
            return 0;
        }
        break;
      case 1:
        if (sscanf(optarg, "%s" , ip) < 0 || strcmp(optarg, "--port") == 0)
        {
            printf("Invalid arguments (ip)!\n");
            exit(EXIT_FAILURE);
        }
        break;
      case 2:
        port = atoi(optarg);
        // TODO: your code here
        if (port <= 0 )
        {
            printf("Error: port<=0\n");
            return 0;
        }
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }
  
  if (BUFSIZE == -1 || port == -1 || !strlen(ip)) {
    fprintf(stderr, "Using: %s --BUFSIZE 256 --ip 12.0.0.1 --port 20001\n", argv[0]);
    return 1;
  }
  
  char buf[BUFSIZE];
  
  if (argc < 3) {
    printf("Too few arguments \n");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(port);

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, BUFSIZE)) > 0) {
    if (write(fd, buf, nread) < 0) {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  exit(0);
}
