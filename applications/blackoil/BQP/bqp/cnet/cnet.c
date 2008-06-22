#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cnet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

int cnet_init(int port) {
  int yes=1;
  int rc;
  int cnet_sfd;
  struct sockaddr_in cnet_saddr;

  cnet_sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cnet_sfd < 0) {
    perror("socket");
    return(-1);
  }

  if (setsockopt(cnet_sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
    perror("setsockopt");
    return(-1);
  }

  
  cnet_saddr.sin_family = AF_INET;
  cnet_saddr.sin_addr.s_addr = INADDR_ANY;
  cnet_saddr.sin_port = htons(port);
  
  rc = bind(cnet_sfd, (struct sockaddr *) &cnet_saddr, sizeof(cnet_saddr));
  if (rc < 0) {
    perror("bind");
    return(-1);
  }

  rc = listen(cnet_sfd, 64);
  if (rc < 0) {
    perror("listen");
    return(-1);
  }

  return(cnet_sfd);
}

int cnet_accept(int cnet_sfd, int tosec) {
  int clen, rc, numfds;
  fd_set rfds;
  struct timeval timeout;
  struct sockaddr_in cnet_caddr;

  // set up read fds struct and timeout for select()
  FD_ZERO(&rfds);
  FD_SET(cnet_sfd, &rfds);
  timeout.tv_sec = tosec;
  timeout.tv_usec = 0;
  numfds = cnet_sfd + 1;

  rc = select(numfds, &rfds, NULL, NULL, &timeout);
  if (rc <= 0) {
    return(-1);
  } else {
    clen = sizeof(cnet_caddr);
    return(accept(cnet_sfd, (struct sockaddr *)&cnet_caddr, (socklen_t *)&clen));
  }
}

int cnet_close(int cnet_sfd) {
  close(cnet_sfd);
  return(0);
}

int cnet_connect(char *hostname, int port, int *sd, int tosec) {
  int sfd, flags, rc, numfds;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  struct timeval timeout;
  fd_set wfds;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
    return(-1);
  }
  *sd = sfd;
  
  server = gethostbyname(hostname);
  if (server == NULL) {
    perror("gethostbyname");
    return(-1);
  }

  // got the socket, figured out the host addr, lets init some variables
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(port);

  FD_ZERO(&wfds);
  FD_SET(sfd, &wfds);
  numfds = sfd + 1;
  timeout.tv_sec = tosec;
  timeout.tv_usec = 0;
  
  flags = fcntl(sfd, F_GETFL);
  if (flags < 0) {
    perror("getflags fcntl");
    return(-1);
  }

  rc = fcntl(sfd, F_SETFL, flags | O_NONBLOCK);
  if (rc < 0) {
    perror("nonblock fcntl");
    return(-1);
  }
  
  
  // now that we're non-blocking, this should just go through
  rc = connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
  /* non blocking version */
  /*
  if (connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) <= 0) {
    printf("ERNO: %d\n", errno);
    if (errno != EINPROGRESS) {
      perror("connect");
      return(-1);
    }
  }
  */

  // now we actually wait for the connect to complete
  rc = select(numfds, NULL, &wfds, NULL, &timeout);
  if (rc <= 0) {
    fprintf(stderr, "select timed out on connect\n");
    return(-1);
  }

  // reset socket to blockin
  rc = fcntl(sfd, F_SETFL, flags);
  if (rc < 0) {
    perror("resetflags fcntl");
    return(-1);
  }

  
  return(0);
}

int cnet_send(int sfd, const void *buf, int bytes, int tosec) {
  int rc, writebytes, numfds;
  fd_set wfds;
  struct timeval timeout;
  int done;
  
  FD_ZERO(&wfds);
  FD_SET(sfd, &wfds);
  numfds = sfd + 1;
  
  timeout.tv_sec = tosec;
  timeout.tv_usec = 0;
  
  done=0;
  while(!done) {
    rc = select(numfds, NULL, &wfds, NULL, &timeout);
    if (rc <= 0) {
      if (errno == EINTR) {
      } else {
	fprintf(stderr, "ERROR: select timed out in send\n");
	return(-1);
      }
    } else {
      done++;
    }
  }
  
  done=0;
  while(!done) {
    rc = send(sfd, buf, bytes, 0);
    if (rc < 0) {
      if (errno == EINTR) {
      } else {
	perror("send");
	return(-1);
      }
    } else {
      writebytes = rc;
      done++;
    }
  }
  
  
  return(writebytes);
   
}

int cnet_recv(int sfd, void *buf, int bytes, int tosec) {
  int rc, readbytes, numfds;
  fd_set rfds;
  struct timeval timeout;
  int done;
  
  FD_ZERO(&rfds);
  FD_SET(sfd, &rfds);
  numfds = sfd + 1;
  
  timeout.tv_sec = tosec;
  timeout.tv_usec = 0;
  
  done=0;
  while(!done) {
    rc = select(numfds, &rfds, NULL, NULL, &timeout);
    if (rc <= 0) {
      if (errno == EINTR) {
      } else {
	fprintf(stderr, "ERROR: select timed out in recv\n");
	return(-1);
      }
    } else {
      done++;
    }
  }
  
  done=0;
  while(!done) {
    rc = recv(sfd, buf, bytes, 0);
    if (rc < 0) {
      if (errno == EINTR) {
      } else {
	perror("recv");
	return(-1);
      }
    } else {
      readbytes = rc;
      done++;
    }
  }
  return(readbytes);
}

int cnet_sendbuf(int sfd, const void *buf, int bytes, int tosec) {
  int writebytes, rc;

  writebytes = 0;
  while(writebytes < bytes) {
    rc = cnet_send(sfd, (char *)buf+writebytes, bytes-writebytes, tosec);
    if (rc <= 0) {
      return(rc);
    }
    writebytes += rc;
  }
  
  return(writebytes);
}

int cnet_recvbuf(int sfd, void *buf, int bytes, int tosec) {
  int readbytes, rc;

  readbytes = 0;
  while(readbytes < bytes) {
    rc = cnet_recv(sfd, (char *)buf+readbytes, bytes-readbytes, tosec);
    //    printf("%d--", rc, buf);
    if (rc < 0) {
      return(rc);
    } else if (rc == 0) {
      return(readbytes);
    }
    readbytes += rc;
  }
  
  return(readbytes);

}

int cnet_protsend(int sfd, const void *buf, int bytes, int tosec) {
  int rc;
  char sizebuf[16];
  
  bzero(sizebuf, 16);
  sprintf(sizebuf, "%d", bytes);
  rc = cnet_sendbuf(sfd, sizebuf, 16, tosec);
  if (rc < 0) {
    return(-1);
  }

  rc = cnet_sendbuf(sfd, buf, bytes, tosec);
  if (rc != bytes) {
    return(-1);
  }
  
  return(rc);
}

int cnet_protrecv(int sfd, void **buf, int tosec) {
  int rc, rsize;
  char sizebuf[16];
  
  bzero(sizebuf, 16);
  rc = cnet_recvbuf(sfd, sizebuf, 16, tosec);
  if (rc != 16) {
    return(-1);
  }
  rsize = atoi(sizebuf);

  *buf = (char *)malloc(rsize+1);
  bzero(*buf, rsize+1);
  rc = cnet_recvbuf(sfd, *buf, rsize, tosec);
  if (rc != rsize) {
    return(-1);
  }
  
  return(rc);  
}

#if 0
struct hostent *mygethostbyname (char *host)
{
  struct hostent hostbuf, *hp;
  size_t hstbuflen;
  char *tmphstbuf;
  int res;
  int herr;
          
  hstbuflen = 1024;
  /* Allocate buffer, remember to free it to avoid memory leakage.  */
  tmphstbuf = malloc (hstbuflen);
          
  while ((res = gethostbyname_r (host, &hostbuf, tmphstbuf, hstbuflen,
				 &hp, &herr)) == ERANGE)
    {
      /* Enlarge the buffer.  */
      hstbuflen *= 2;
      tmphstbuf = realloc (tmphstbuf, hstbuflen);
    }
  /*  Check for errors.  */
  if (res || hp == NULL)
    return NULL;
  return hp;
}
#endif
