#include <iostream>
#include <mpi.h>

static int ts_rank;
// static double ts_before_init;
static double ts_after_init;
static double ts_before_finalize;
// static double ts_after_finalize;



int TIME_MPI_Init(int *argc, char ***argv) {
  int ret;
  //ts_before_init = std::chrono::steady_clock::now();
  ret = MPI_Init(argc, argv);
  PMPI_Comm_rank(MPI_COMM_WORLD, &ts_rank);
  ts_after_init = MPI_Wtime();
  return ret;
}

int TIME_MPI_Finalize() {
  int ret;
  ts_before_finalize = MPI_Wtime();
  ret = MPI_Finalize();
  //ts_after_finalize = std::chrono::steady_clock::now();
  // now it's time to compute the time
  double net_time = ts_before_finalize - ts_after_init;
  // double mpi_time = ts_after_finalize - ts_before_init;
  //std::chrono::duration<double> mpi_time(ts_before_init - ts_after_finalize);

  std::cout << "@@net_time;" << ts_rank << ";" << net_time << "\n";
  return ret;
}


int MPI_Init(int *argc, char ***argv) {
    return TIME_MPI_Init(argc, argv);
}

int MPI_Finalize() {
    return TIME_MPI_Finalize();
}
