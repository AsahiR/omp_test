#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int core=0;

int task_limit=1000;/* use task or not */

long time_diff_us(struct timeval st, struct timeval et)
{
  return (et.tv_sec-st.tv_sec)*1000000+(et.tv_usec-st.tv_usec);
}

int init(double *data, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    data[i] = (double)rand() / RAND_MAX;
  }
  return 0;
}

int print(double *data, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    printf("%.4lf ", data[i]);
  }
  printf("\n");
  return 0;
}

/* sort data[s] ... data[e-1] */
int sort(double *data, int s, int e)
{
  int i, j;
  double pivot;

  if (e-s <= 1) {
    /* do nothing */
    return 0;
  }

  /* [Pivot selection] */
  /* select a pivot, one of data[s] ... data[e-1]  */
  /* pivot must not be the minimum value */
  pivot = data[s];
  for (i = s+1; i < e; i++) {
    if (data[i] > pivot) {
      pivot = data[i];
      break;
    }
    else if (data[i] < pivot) {
      break;
    }
  }

  if (i == e) {
    /* all values are same. do nothing */
    return 0;
  }

  /* [Partition data[] into 2 parts] */
  i = s;
  j = e-1;
  while (1) {
    double tmp;
    while (data[i] < pivot)
      i++;

    while (data[j] >= pivot)
      j--;

    if (i >= j) {
      /* partitioning finished */
      break;
    }

    /* swap */
    tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;
    i++;
    j--;
  }
  /* Here, "i" is boundary of 2 parts */

  /* call sort() recursively */
  if(e-s < task_limit){
    sort(data,s,i);
    sort(data,i,e);
  }else{
#pragma omp task
    sort(data, s, i);  /* sort data[s] ... data[i-1] */
#pragma omp task
    sort(data, i, e);  /* sort data[i] ... data[e-1] */
#pragma omp taskwait
  }

  return 0;
}

int check(double *data, int n)
{
  int i;
  int flag = 0;
  for (i = 0; i < n-1; i++) {
    if (data[i] > data[i+1]) {
      printf("Error: data[%d]=%.4lf, data[%d]=%.4lf\n",
	     i, data[i], i+1, data[i+1]);
      flag++;
    }
  }
  if (flag == 0) {
    printf("Data are sorted\n");
  }
  return 0;
}

void sort_wrapper(double *data,int s,int e){
#pragma omp parallel
  {
    core=omp_get_num_threads();
#pragma omp single
    {
      sort(data,s,e);
    }
  }
}


int main(int argc, char *argv[])
{
  int n = 10000;
  int r=0;/* random seed */
  double *data;
  int i;
  int opt;

  while((opt=getopt(argc,argv,"::n:l:r:")) != -1){
    switch (opt) {
      case 'l':
        task_limit=atoi(optarg);
        break;
      case 'n':
        n=atoi(optarg);
        break;
      case 'r':
        r=atoi(optarg);
        break;
      case ':':
        perror("need option argment\n");
        exit(1);
    }
  }

  /*
  if (argc >= 2) {
    n = atol(argv[1]);
    if (argc==3){
      task_limit = atol(argv[2]);
    }
  }
  */

  data = malloc(sizeof(double)*n);

  printf("core,task_limit,name,size,time(us)\n");

  srand(r); /* first data same for same r*/

  for (i = 0; i < 3; i++) {
    struct timeval st;
    struct timeval et;
    long us;
    double res;


    init(data, n);
    /*print(data, n);*/
    gettimeofday(&st, NULL); /* get start time */
    sort_wrapper(data,0,n);
    /*sort(data, 0, n);*/
    gettimeofday(&et, NULL); /* get start time */
    us = time_diff_us(st, et);

    /*
    printf("sorting %d data took %ld us\n",
	   n, us);
     */

    printf("%d,%d, %s,%d,%ld\n", core,task_limit,argv[0],n,us);

    check(data, n);
    /*print(data, n);*/
  }

  free(data);

  return 0;
}
