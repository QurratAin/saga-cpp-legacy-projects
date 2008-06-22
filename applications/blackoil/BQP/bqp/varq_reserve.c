#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bqp_tools.h>


#define ARGS "q:n:r:hm:p:d:"
char *usage = "varq_reserve -m <machine> -q <queue> -n <nodes> -r <max_walltime> -d <start_deadline> -p <success_probability>\n";

int main(int argc, char **argv) {
  int rc, id, nnodes=0, nreqtime=0, ndeadline=0, wait, realrtime;
  int c;
  double ret, nprob=0.0, realprob;
  char *nmach=NULL, *nqueue=NULL;
  
  while((c = getopt(argc, argv, ARGS)) != EOF) {
    switch(c) {
    case 'm':
      nmach = strdup(optarg);
      break;
    case 'q':
      nqueue = strdup(optarg);
      break;
    case 'n':
      nnodes = atoi(optarg);
      if (nnodes < 1) {
	printf("%s", usage);
	exit(1);
      }
      break;
    case 'r':
      nreqtime = atoi(optarg);
      if (nreqtime < 1) {
	printf("%s", usage);
	exit(1);
      }
      break;
    case 'd':
      ndeadline = atoi(optarg);
      if (ndeadline < 1) {
	printf("%s", usage);
	exit(1);
      }
      break;
    case 'p':
      nprob = atof(optarg);
      if (nprob <= 0.0 || nprob >= 1.0) {
	printf("%s", usage);
	exit(1);
      }
      break;
    default:
      printf("%s", usage);
      exit(1);
      break;
    }
  }

  if (nmach == NULL || nqueue == NULL || nnodes == 0 || nreqtime == 0 || ndeadline == 0 || nprob == 0.0) {
    printf("%s", usage);
    exit(1);
  }

  rc = varq_request(nmach, nqueue, nnodes, nreqtime, ndeadline, nprob, &wait, &realrtime, &realprob);
  if (realprob < nprob) {
    printf("%-7s %-7s %-11s %-8s %-6s %-8s %-9s %-8s\n","#Action","ToWait","Mach","Queue","Nodes","MaxWall","OvrAlloc","SuccProb");
    printf("%-7s %-7d %-11s %-8s %-6d %-8d %-9d %-8f\n", "FAIL", 0, nmach, nqueue, nnodes, 0, 0, 0.00);
  } else {
    rc = varq_reserve_block(nmach, nqueue, nnodes, nreqtime, ndeadline, nprob, &wait, &realrtime, &realprob);
    printf("%-7s %-7s %-11s %-8s %-6s %-8s %-9s %-8s\n","#Action","ToWait","Mach","Queue","Nodes","MaxWall","OvrAlloc","SuccProb");
    printf("%-7s %-7d %-11s %-8s %-6d %-8d %-9d %-8f\n", "BIND", wait, nmach, nqueue, nnodes, realrtime, realrtime - nreqtime, realprob);
  }
  exit(1);
}
