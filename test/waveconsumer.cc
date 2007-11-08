// Waveconsumer
// 
// This is a placeholder for a data sink application interfaced
// to Music



#include <mpi.h>
#include <music.hh>

#define DATA_SIZE 1000


double data[DATA_SIZE];

int
main (int nargs, char* argv[])
{
  double time;
  int rank;

  MUSIC::setup* setup = new MUSIC::setup (nargs, argv);

  // Find out who we are
  rank = setup->communicator ().Get_rank();

  // Subscribe to data from the wavegenerator application
  setup->subscribe (new MUSIC::array_data (data, MPI::DOUBLE,
					   new MUSIC::linear_index (DATA_SIZE,
								    DATA_SIZE*rank)),
		    "Wavedata");

  MUSIC::runtime* runtime = setup->done ();

  for (time = 0.0; time < 1.0; time += 0.1) {
    if (rank == 0) {
      // Generate original data on master node
      int i;

      for (i = 0; i < DATA_SIZE; ++i)
	data[i] = sin(time * i/1000.0);
    }

    // Broadcast these data out to all nodes
    runtime->communicator ().Bcast (data, DATA_SIZE, MPI::DOUBLE, 0);

    // Retrieve data from other program
    runtime->tick (time);
  }

  runtime->finalize ();

  return 0;
}