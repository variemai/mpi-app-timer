#include <iostream>
#include <mpi.h>

#define MAX_ARG_STRING_SIZE 2048
#define MAX_ARGS 1024
#define MAX_ARG_SIZE 64

static int ac;
static char *av[MAX_ARGS];

static int ts_rank;
// static double ts_before_init;
static double ts_after_init;
static double ts_before_finalize;
// static double ts_after_finalize;


static void getProcCmdLine(int *ac, char **av) {
#ifdef __linux__
  int i = 0, pid;
  char *inbuf, file[256];
  FILE *infile;
  char *arg_ptr;

  *ac = 0;
  *av = NULL;

  pid = getpid ();
  snprintf (file, 256, "/proc/%d/cmdline", pid);
  infile = fopen (file, "r");

  if (infile != NULL){
    while (!feof (infile)){
      inbuf = (char*) malloc (MAX_ARG_STRING_SIZE);
      memset(inbuf, 0, MAX_ARG_STRING_SIZE);
      if (fread (inbuf, 1, MAX_ARG_STRING_SIZE, infile) > 0){
        arg_ptr = inbuf;
        while (*arg_ptr != 0){
          av[i] = strdup (arg_ptr);
          arg_ptr += strlen (av[i]) + 1;
          i++;
        }
      }
      free(inbuf);
    }
    *ac = i;
    fclose (infile);
  }
  else{
    fprintf(stderr, "MPI APP TIMER: Error opening file %s FILE:LINE = %d\n",
            file, __FILE__, __LINE__);
    exit(1);
  }

#elif __APPLE__
  *ac = 0;
  *av = NULL;
#else
  fprintf(stderr,"MPI APP TIMER: Unsupported platform\n");
  exit(1);
#endif
}

int TIME_MPI_Init(int *argc, char ***argv) {
  int ret;
  //ts_before_init = std::chrono::steady_clock::now();
  ret = PMPI_Init(argc, argv);
  PMPI_Comm_rank(MPI_COMM_WORLD, &ts_rank);
  ts_after_init = MPI_Wtime();
  return ret;
}


int TIME_MPI_Init_thread(int *argc, char ***argv, int required, int *provided) {
  int ret;
  //ts_before_init = std::chrono::steady_clock::now();
  ret = PMPI_Init_thread(argc, argv, required, provided);
  PMPI_Comm_rank(MPI_COMM_WORLD, &ts_rank);
  ts_after_init = PMPI_Wtime();
  return ret;
}

int TIME_MPI_Finalize() {
  int ret;
  ts_before_finalize = PMPI_Wtime();
  ret = PMPI_Finalize();
  //ts_after_finalize = std::chrono::steady_clock::now();
  // now it's time to compute the time
  double net_time = ts_before_finalize - ts_after_init;
  // double mpi_time = ts_after_finalize - ts_before_init;
  //std::chrono::duration<double> mpi_time(ts_before_init - ts_after_finalize);

  std::cout << "@@net_time;" << ts_rank << ";" << net_time << "\n";
  return ret;
}


extern "C" {
  void mpi_init_thread_(int *required, int *provided, int *ierr) {
    char **tmp;
    int ret;
    getProcCmdLine(&ac, av);
    tmp = av;
    ret = TIME_MPI_Init_thread(&ac, &tmp , *required, provided);
    *ierr = ret;
  }
}

extern "C" {
  void mpi_init_(int *ierr) {
    int ret = 0;
    char **tmp;
    getProcCmdLine(&ac, av);
    tmp = av;
    ret = TIME_MPI_Init(&ac, &tmp);
    *ierr = ret;
  }
}


int MPI_Init(int *argc, char ***argv) {
  return TIME_MPI_Init(argc, argv);
}


int MPI_Init_thread(int *argc, char ***argv, int required, int *provided) {
  return TIME_MPI_Init_thread(argc, argv, required, provided);
}


int MPI_Finalize() {
  return TIME_MPI_Finalize();
}
