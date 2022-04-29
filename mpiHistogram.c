//Cody Johnson
//CS415 HW4, Programming Problem 4

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

void Get_input(int argc, char* argv[], int id, int* data_count);
void Find_bin(float* local_data, int num_sent, int bin_count, int* local_bin_counts, float* bin_maxes, int min_meas);

int main(int argc, char* argv[]) {
  int num_procs, id; 
  int data_count = -1;                 //Length of data from input file
  int data_size = 0;                   //Var to hold the index count of the data array
  float* data = NULL;
  //float* data_appended = NULL;

  int min_meas = -10, max_meas = 110, bin_count = 12;
  int bin_width = (max_meas - min_meas)/bin_count;
  //Each per-process bins to be summed to bin_counts
  int local_bin_counts[bin_count];
  //Total bin sizes for each bucket
  int bin_counts[bin_count];

  //Initialize and fill the bin_maxes array
  float bin_maxes[bin_count];
  for(int i = 0; i < bin_count; i++) {
    bin_maxes[i] = min_meas + bin_width*(i+1);
    local_bin_counts[i] = 0;
  }



  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  Get_input(argc, argv, id, &data_count);
 


  if(id == 0) {

    data = malloc(sizeof(float) * data_count);

    //Hold the line we're scanning
    char* line = NULL;
    char* token;
    size_t len;

    //Read all lines from stdin
    //while((getline(&line, &len, stdin)) != -1 && data_size < data_count) {
    //  while((token = strtok_r(line, ",", &line)) != NULL && data_size < data_count) {
        //printf("data[%d] = %f\n", data_size, (float)atof(token));
        //Convert the tokenized string to a float
    //    data[data_size] = (float)atof(token);
    //    data_size++;
    //  }
    //}

    for(int i = 0; i < data_count; i++) {
      scanf("%d", &data[i]);
    }

    //free(line);

    //Create appended array to hold only elements that were read in from stdin
    //(Extras are appended on from user's entered value for data_count)
    //This is done to prevent skewing histogram with tons of extra 0 values
    //data_appended = malloc(sizeof(float) * data_size);
    //memcpy(data_appended, data, sizeof(float) * data_size);
    //data = realloc(data, data_size);

    //for(int i = 0; i < data_size; i++) {
    //  data_appended[i] = data[i];
      //printf("data_appended[%d] = %f\n", i, data_appended[i]);
    //}
  }   


  
  //Broadcast the value of data_size to all processes
  MPI_Bcast(&data_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
 
  //Create num of elements and arrays to be scattered to each process
  int num_sent = data_size / num_procs;
  //float local_data[num_sent];
  float* local_data = malloc(sizeof(float) * num_sent);

  //Send the portion of the appended data to all processes in the communicator
  MPI_Scatter(data, num_sent, MPI_FLOAT, local_data, num_sent, MPI_FLOAT, 0, MPI_COMM_WORLD);

  //Find the bins for each value in local_bin_counts
  Find_bin(local_data, num_sent, bin_count, local_bin_counts, bin_maxes, min_meas);

  //Reduce the local bin counts back down to the global bin counts
  MPI_Reduce(local_bin_counts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  //Wait for all processes to finish
  MPI_Barrier(MPI_COMM_WORLD);



  //Print out the bin counts to stdout
  if(id == 0) {
    fprintf(stdout, "Histogram for %d values from stdin:\n", data_size);
    for(int i = 0; i < bin_count; i++) {
      if(i == 0) {
        fprintf(stdout, "Bin %2d: %10d - %10f = %9d values\n", i, min_meas, bin_maxes[i], bin_counts[i]);
      }
      else if(i == bin_count-1) {
        fprintf(stdout, "Bin %2d: %10f - %10f = %9d values\n\n\n", i, bin_maxes[i-1], bin_maxes[i], bin_counts[i]);
      }
      else {
        fprintf(stdout, "Bin %2d: %10f - %10f = %9d values\n", i, bin_maxes[i-1], bin_maxes[i], bin_counts[i]);
      }
    }

    free(data);
    //free(data_appended);
  }

  free(local_data);
  MPI_Finalize();
  return 0;
}



void Get_input(int argc, char* argv[], int id, int* data_count) {
  if(id == 0) {
    //Only execute main code if we have the proper amount of args
    if(argc == 2) {
      //If the user supplies a value of 0 or smaller, throw an error
      if((*data_count = atoi(argv[1])) <= 0) {
        fprintf(stderr, "Usage: mpiexec -n [num_procs] ./mpiHistogram [data_count] < [data_file] >> [histogram_file]\n");
        exit(-1);
      }
    }
    else {
      fprintf(stderr, "Usage: mpiexec -n [num_procs] ./mpiHistogram [data_count] < [data_file] >> [histogram_file]\n");
      exit(-1); 
    }
  }

  MPI_Bcast(data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
}



void Find_bin(float* local_data, int num_sent, int bin_count, int* local_bin_counts, float* bin_maxes, int min_meas) {
  for(int i = 0; i < num_sent; i++) {
    //printf("%f\n", local_data[i]);
    for(int j = 0; j < bin_count; j++) {
      //printf("i = %d, j = %d\n", i, j);
      //Edge case for first group
      if(j == 0) {
        //Match to this bin, so iterate local_bin_counts for that index
        //and move to the next value in the local array
        if(min_meas <= local_data[i] && local_data[i] < bin_maxes[j]) {
          local_bin_counts[j]++;
          break;
        }
      }
      else {
        //printf("%d, %f <= %f < %f\n", (bin_maxes[j-1] <= local_data[i] && local_data[i] < bin_maxes[j]), bin_maxes[j-1], local_data[i], bin_maxes[j]);
        if(bin_maxes[j-1] <= local_data[i] && local_data[i] < bin_maxes[j]) {
          local_bin_counts[j]++;
          break;
        }
      }
    }
  }

  return;
}


