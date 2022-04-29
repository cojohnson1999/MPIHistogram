//Cody Johnson
//CS415 HW4, Programming Problem 3

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int Get_input(int argc, char* argv[], int n);

int main(int argc, char *argv[]) {
  //Integer to be read in from CLI
  int n = 0;

  //Get the input from the CLI and record the results to the file
  Get_input(argc, argv, n);

  return 0;
}

int Get_input(int argc, char* argv[], int n) {
  //Upper bound of random numbers (adjusted by +11 so that bounds go from 
  //[0, 120] to [-10, 110] when subtracting diffValue)
  int upperBound = 121;
  //Value to subtract by to get our real random value
  int diffValue = 10;

  //Only execute main code if we have the proper amount of args
  if(argc == 2) {
    //If the user supplies a value of 0 or smaller, throw an error
    if((n = atoi(argv[1])) <= 0) {
      fprintf(stderr, "Usage: value of N should be an int greater than 0\n");
      exit(1);
    }
    int temps[n];

    //Give us a random seed for better randomization
    srand(time(NULL));

    for(int i = 0; i < n; i++) {
      //Generate random value from [0, 120] and subtract 10 to get to [-10, 110]
      temps[i] = (rand() % upperBound) - diffValue;

      //Send the numbers to the file we're reading stdout to (temps)
      if(i != n-1) {
        if(i != 0 && i % 20 == 0) fprintf(stdout, "%d\r\n", temps[i]);
        else fprintf(stdout, "%d,", temps[i]);
      }
      else {
        fprintf(stdout, "%d\r\n", temps[i]);
      }
    }
  }

  else {
    fprintf(stderr, "Usage: ./tempReadings [N] >> [temps_file]\n");
    exit(1); 
  }

  return 0;
}
