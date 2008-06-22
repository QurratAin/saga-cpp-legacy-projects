#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cnet.h>

int cnet_init(int port) {
  int yes=1;
  int rc;
  int sfd;
  struct sockaddr_in cnet_saddr;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
    return(-1);
  }

  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
    perror("setsockopt");
    return(-1);
  }

  
  cnet_saddr.sin_family = AF_INET;
  cnet_saddr.sin_addr.s_addr = INADDR_ANY;
  cnet_saddr.sin_port = htons(port);
  
  rc = bind(sfd, (struct sockaddr *) &cnet_saddr, sizeof(cnet_saddr));
  if (rc < 0) {
    perror("bind");
    return(-1);
  }

  rc = listen(sfd, 64);
  if (rc < 0) {
    perror("listen");
    return(-1);
  }

  return(sfd);
}

int cnet_accept(int sfd) {
  int clen;
  struct sockaddr_in cnet_caddr;

  clen = sizeof(cnet_caddr);
  return(accept(sfd, (struct sockaddr *)&cnet_caddr, &clen));
}

int cnet_close(int sfd) {
  return(close(sfd));
}

int cnet_connect(char *hostname, int port, int *sd) {
  int sfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  struct linger ls;

  ls.l_onoff = 1;
  ls.l_linger = 1;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
    return(-1);
  }
      
  /*
  if (setsockopt(sfd, SOL_SOCKET, SO_LINGER, &ls, sizeof(struct linger)) < 0) {
    perror("setsockopt");
    return(-1);
  }
  */

  server = gethostbyname(hostname);
  if (server == NULL) {
    perror("gethostbyname");
    return(-1);
  }
      
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    //    printf("HOSTNAME: %s port %d\n", hostname, port);
    perror("connect");
    return(-1);
  }

  *sd = sfd;
   
  return(0);
}
