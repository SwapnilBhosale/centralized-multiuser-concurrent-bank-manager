/*
 *  cracker.c
 *
 *  Created on: Oct 7, 2019
 *      Author: Swapnil Bhosale

 *  This is a MPI based brute force MD5 password hash cracker.
 *  1) We take a 6 digit password from the charset [a-z], [A-Z], [0-9] 
 *  2) Calculate MD5 hash of passed it to all the concurrent processes
 *  3) Use MPI to distribute the total combinations i.e 62 ^ 6 = 56 800 235 584
 *
 *
 *  How we distribute the load ?
 *
 *  We use six for loops to generate all the six digit combination from character space of 62.
 *  We distribute the process on the outemost for loop.
 *  If there are 30 process, then process  "1" will check for "abXXXX", "baXXXX" , "aaXXXX" , "bbXXXX" combinations, likewise rest 
 *  processes will also share the load.
 *
 *  Known issue, Not much familiar with MPI so, couldn't shut down system gracefully, Tried adding broadcast msg, but solution is not\
 *  working yet. So program will exit() when the password is found.
 *
 *  Created on: Oct 7, 2019
 *      Author: Swapnil Bhosale
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/usr/include/mpi/mpi.h"
#include <openssl/md5.h>
#include <time.h>
#include <sys/resource.h>
char charset[]= {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

#define N 62

//clock_t begin = clock(), end;
double time_spent;


/**
 * param str: char pointer accepts the string for which MD5 hash to be calculated
 * param length: length of the input string
 * return: returns the MD5 string of 32 hexadecimal numbers ie. 128 bit. (each hex number represented by 4 bit hence 128/4 = 32)
 *
 */
char *str2md5(const char *str, int length) {
  int nn;
  MD5_CTX c;
  unsigned char digest[16];
  char *out = (char*)malloc(33);

  MD5_Init(&c);

  while (length > 0) {
    if (length > 512) {
      MD5_Update(&c, str, 512);
    } else {
      MD5_Update(&c, str, length);
    }
    length -= 512;
    str += 512;
  }

  MD5_Final(digest, &c);

  for (nn = 0; nn < 16; ++nn) {
    snprintf(&(out[nn*2]), 16*2, "%02x", (unsigned int)digest[nn]);
  }

  return out;
}

/*void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
  MPI_Comm communicator) {
  int world_rank;
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;
  MPI_Comm_size(communicator, &world_size);

  if (world_rank == root) {
// If we are the root process, send our data to everyone
int i;
for (i = 0; i < world_size; i++) {
if (i != world_rank) {
MPI_Send(data, count, datatype, i, 0, communicator);
}
}
} else {
// If we are a receiver process, receive the data from the root
MPI_Recv(data, count, datatype, root, 0, communicator,
MPI_STATUS_IGNORE);
}
}*/


/**
 *  Prints the system stats, total user and system time
 *
 */
void print_stats() {
  double user, sys;
  struct rusage   myusage;

  if (getrusage(RUSAGE_SELF, &myusage) < 0)
    printf("Error: getrusage()");

  user = (double) myusage.ru_utime.tv_sec +
    myusage.ru_utime.tv_usec/1000000.0;
  sys = (double) myusage.ru_stime.tv_sec +
    myusage.ru_stime.tv_usec/1000000.0;

  printf("\nuser time = %g, sys time = %g\n", user, sys);
  exit(0);
}


/***
 *  param start: start index in charset char array for outer for loop
 *  param end: end index in charset
 *  param hash: It is a character pointer to the hash value of the input password
 *
 *
 */ 

void doItSixTimes(int start,int end,char *hash){

  int i, j, k, m, n, l;
  //printf("\n start %d end %d ", start, end);
  for(i=start; i<end; i++){ //letters + numbers
    for(j=0; j<N; j++){
      for(k=0; k<N; k++){
        for(l=0; l<N; l++){
          for(m=0; m<N; m++){
            for(n=0; n<N; n++){
              char guess6[] = {charset[i], charset[j], charset[k], charset[l], charset[m], charset[n]};
              //printf("%s\n", guess6);
              char * currHash = str2md5(guess6, strlen(guess6));
              if(strcmp(currHash, hash) == 0){
                printf("We guessed it! Your input was - %s \n", guess6);
                MPI_Send(&guess6, 7, MPI_CHAR, 0 , 0, MPI_COMM_WORLD);

                //end = clock();
                //time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                //printf("Time spent: %f seconds\n", time_spent);
                free(hash);
                free(currHash);
                print_stats();


              }else
                free(currHash);
            }
          }
        }
      }
    }
  }
}

int main(int argc, char* argv[])
{

  int pid, np,
      elements_per_process,
      n_elements_recieved;
  // np -> no. of processes
  // pid -> process id
  MPI_Status status;

  // Creation of parallel processes
  MPI_Init(&argc, &argv);

  // find out process ID,
  // and how many processes were started
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  // master process
  if (pid == 0) {
    int index, i;
    elements_per_process = N / (np-1);

    // check if more than 1 processes are run
    if (np > 1) {
      // distributes the portion of array
      // to child processes to calculate
      // their partial sums
      for (i = 1; i < np-1; i++) {
        int startindex = elements_per_process*(i-1);
        int endindex = elements_per_process*i;
        //printf("\n calculated start end %d %d",startindex, endindex);
        //int length = endindex - startindex;
        MPI_Send(&startindex, 1, MPI_INT, i, 0, MPI_COMM_WORLD);   //Blokcing call
        MPI_Send(&endindex, 1, MPI_INT, i , 0, MPI_COMM_WORLD);
      }

      int startindex = elements_per_process*(np-2);
      int endindex = N;
      MPI_Send(&startindex, 1, MPI_INT, np-1, 0, MPI_COMM_WORLD);   //Blokcing call
      MPI_Send(&endindex, 1, MPI_INT, np-1 , 0, MPI_COMM_WORLD);
    }
    char input[6] = {'\0'};
    printf("\n Enter 6 digit password : ");
    scanf("%s", input);
    //printf("length of ip : %c",input[6]);

    unsigned char *result = (char*)malloc(33);
    result = str2md5(input, strlen(input));
    //result[33] = '\0';
    printf("\n\n*****************************************************\n");
    printf("Computed MD5 digest/hash of password= %s",result);
    printf("\nSit back and relax, Let me take over it now....");
    printf("\n*****************************************************\n\n");

    //printf("\n the data is : ");

    for (i = 1; i < np-1; i++) {

      MPI_Send(result, 32, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    //MPI_Bcast(result, 32, MPI_CHAR, 0, MPI_COMM_WORLD);
    // master process add its own sub array
    char *found_password = (char *)malloc(sizeof(char) * 7);
    MPI_Recv(found_password, 7, MPI_CHAR, MPI_ANY_SOURCE,0,MPI_COMM_WORLD, &status);
    // prints the final sum of array
    printf("Cracked the password : %s\n", found_password);
  }
  // slave processes
  else {
    int startIndex, endIndex;
    char *hash = (char *)malloc (sizeof(char) * 32);
    MPI_Recv(&startIndex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&endIndex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    //printf("got %d %d",startIndex, endIndex);
    int length = endIndex - startIndex;
    char mycharSet [length];
    strncpy(mycharSet, charset + startIndex, length);
    MPI_Recv(hash, 33, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    //printf("\nneed to find %s", hash);
    char found[7] = {'\0'};
    //printf("started printing the substrings");
    //printAllKLength(hash, &found);
    doItSixTimes(startIndex, endIndex, hash);
    //if found send response
  }

  // cleans up all MPI state before exit of process
  MPI_Finalize();

  return 0;
}
