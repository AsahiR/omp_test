#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define TIME_STEP 2
#define TRUE 1


/* global variable*/
float **data[TIME_STEP];
int NX=8192,NY=8192; /* defaut NX,NY. you can set it from commandline */
int core=0;

void my_malloc(int nx,int ny){
  int i,j;
  for(i=0;i<TIME_STEP;i++){
      data[i]=(float **)malloc(sizeof(float **)*ny);
      if (data[i]==NULL){
        perror("malloc failed");
      }
    for(j=0;j<ny;j++){
      data[i][j]=(float *)malloc(sizeof(float *)*nx);
      if (data[i][j]==NULL){
        perror("malloc failed");
      }
    }
  }
}

void malloc_check(void *p){
  if(p==NULL){
    perror("malloc failed\n");
    exit(1);
  }
}

void write_data(char filename[],float **data,int nx,int ny,char *mode){
  /* input must be one dimensional */
  int i,j;
  FILE *fp;
  /*char *dir=dirname(strdup(filename));*/
  char *dir=dirname(strdup(filename));
  struct stat st;

  if (stat(dir,&st) != 0 &&
      mkdir(dir,0777) != 0){
    perror("mkdir error\n");
    exit(1);
  }

  if((fp = fopen(filename,mode))==NULL){
    perror("cannot write data\n");
    exit(1);
  }
  for(i=0;i<ny;i++){
    for(j=0;j<nx;j++){
      fprintf(fp,"%4.3f ",data[i][j]);
    }
    fprintf(fp,"\n");
  }
  fprintf(fp,"\n");
  fclose(fp);
}

/* in microseconds (us) */
double get_elapsed_time(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec - begin->tv_sec) * 1000000
            + (end->tv_usec - begin->tv_usec);
}

void init()
{
  int x, y;
  int cx = NX/2, cy = 0; /* center of ink */
  int rad = (NX+NY)/8; /* radius of ink */

  for(y = 0; y < NY; y++) {
    for(x = 0; x < NX; x++) {
      float v = 0.0;
      if (((x-cx)*(x-cx)+(y-cy)*(y-cy)) < rad*rad) {
	v = 1.0;
      }
      data[0][y][x] = v;
      data[1][y][x] = v;
    }
  }
  return;
}

/* Calculate for one time step */
/* Input: data[t%2], Output: data[(t+1)%2] */
void calc(int nt)
{
  int t, x, y;

  for (t = 0; t < nt; t++) {
    int from = t%2;
    int to = (t+1)%2;

#if 1
    /*
    printf("step %d\n", t);
    */
    fflush(0);
#endif
    
#pragma omp parallel for private(x)
    for (y = 1; y < NY-1; y++) {
      core=omp_get_num_threads();
      for (x = 1; x < NX-1; x++) {
	data[to][y][x] = 0.2 * (data[from][y][x]
				+ data[from][y][x-1]
				+ data[from][y][x+1]
				+ data[from][y-1][x]
				+ data[from][y+1][x]);
      }
    }
  }

  return;
}

int  main(int argc, char *argv[])
{
  struct timeval t1, t2;
  int nt = 20; /* number of time steps */
  int log=TRUE-TRUE; /* log data for answer check or not */
  int opt;

  while((opt=getopt(argc,argv,"::lx:y:t:")) != -1){
    switch (opt) {
      case 'l':
        log=TRUE;
        break;
      case 'x':
        NX=atoi(optarg);
        break;
      case 'y':
        NY=atoi(optarg);
        break;
      case 't':
        nt=atoi(optarg);
        break;
      case ':':
        perror("need option argment\n");
        exit(1);
    }
  }
  
  /*
  if (argc >= 2) {
      nt = atoi(argv[1]);
      if(argc == 4){
        NX=atoi(argv[2]);
        NY=atoi(argv[3]);
      }
  }
  */
  my_malloc(NX,NY);

  init();
  int i;
  gettimeofday(&t1, NULL);

  calc(nt);

  gettimeofday(&t2, NULL);

  {
      double us;
      double gflops;
      int op_per_point = 5; // 4 add & 1 multiply per point

      us = get_elapsed_time(&t1, &t2);
      gflops = ((double)NX*NY*nt*op_per_point)/us/1000.0;

      /*
      printf("Elapsed time: %.3lf sec\n", us/1000000.0);
      printf("Speed: %.3lf GFlops\n", gflops);
      */

      printf("Core,Name,NX,NY,Elapsed time(sec),Speed(GFlops)\n");
      printf("%d, %s, %d, %d, %.3lf , %.3lf\n", core,argv[0],NX,NY,us/1000000.0,gflops);
  }

  char filename[]="";
  sprintf(filename,"%s%s_%d_%d_%d","output/",basename(strdup(argv[0])),nt,NX,NY);

  if(log==TRUE){
    write_data(filename,data[0],NX,NY,"w");
    write_data(filename,data[1],NX,NY,"a");
    printf("log suceed\n");
  }
  return 0;
}
