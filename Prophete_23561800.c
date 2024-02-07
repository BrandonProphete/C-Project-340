//Brandon Prophete
//Compilation command: gcc Prophete_23561800.c -o Prophete_23561800.exe -lrt
//Execution command: ./Prophete_23561800.exe SRC_FILE TGT_FILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int CHUNCK_SIZE = 128;

int BUFFER_SIZE = 24;

int main(int argc, char * argv[]) {
 char *inputFile = "sourceFile.txt", *outputFile = "targetFile.txt";

 int f1, f2;// For storing file descriptors of source and target files
  
 int fp, fp2, fp3; // Initializers to hold my pointers
  
 int p[2]; //For storing file decriptors of pipes
  
 pid_t child;
  
 int fileSize;// Our fileSize to check if the progrma is being read
  
 int p_line;// We intitalize for a pointer to our pointer
  
 
 // Have other varaibles to capture size and chunk 
 // Check if 2 arguments (source, destination files) are given
    if (argc != 3)
    {
        printf("Wrong number of command line arguments\n");
        return 1;
    }

    // Access and check if source file is accessible
    if ((f1 = open(inputFile, O_RDONLY, 0)) == -1)
    {
        printf("Can't open %s\n", argv[1]);
        return 2;
    }

    // Check if target file is writable and does not exists
    if ((f2 = creat(outputFile, 0644) == -1))
    {
        printf("Can't create %s\n", argv[2]);
        return 3;
    }

    // Open output file for writing
    if ((f2 = open(outputFile, O_WRONLY)) == -1) {
        printf("Can't open file %s for writing data", argv[2]);
        return 4;
    }
  //Helps us change positon of our read/write pointers in the file description
  fileSize = lseek(f1, 0, SEEK_END);
  lseek(f1, 0, SEEK_SET);

   //Our pointers to our buffer, in, and out
   char  *myBuffer = "MyBuffer";
   char * myBufferPointer;
  
   char *myIn = "Inptr";
   int * inptr;

   char *myOut = "Outptr"; 
   int * outptr;
  
  fp = shm_open("MyBuffer",O_CREAT | O_RDWR,0666); // configure the size of the shared memory object
  // the size here will be buffer size * chunk size  
  
  
  ftruncate(fp, sizeof(int)); // memory map the shared memory object

  //Here we just do the same thing for in and out, but their memory map is turned into a int instead of a char
  myBufferPointer = (char *)mmap(0, CHUNCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);

  fp2 = shm_open("Inptr", O_CREAT | O_RDWR, 0666);
  ftruncate(fp2, sizeof(int));

  inptr = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fp2, 0);

  fp3 = shm_open("Outptr", O_CREAT | O_RDWR, 0666);
  ftruncate(fp3, sizeof(int));

  outptr = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fp3, 0);
  
  *inptr = 0;
  *outptr = 0;
  //Create Pipe
  printf("Inptr = %d\n", *inptr);
  if(pipe(p) == -1){
    printf("Creation of pipe failed\n");
    return 5;
  }
  //create a new child process
  child = fork();

  if(child > 0){
        int next_produced;
        int shMenPrntCharCount = 0;
    //Parent process to read the file from the source file
    
    while((next_produced = read(f1, myBufferPointer + (*inptr + CHUNCK_SIZE), CHUNCK_SIZE))){ 

       if((*inptr * CHUNCK_SIZE) % CHUNCK_SIZE-1 == *outptr * CHUNCK_SIZE ){
         //do nothing here since my buffer and is waiting for the child consumer read and write out to the customer
       }
      shMenPrntCharCount += next_produced;
      char *parent_out;
      //Here we use a write function for console output 
       
      write(STDOUT_FILENO, "PARENT: IN = %d\n ", *inptr);
      write(STDOUT_FILENO, parent_out, sizeof(parent_out));
      write(STDOUT_FILENO, "\n", 1);

      
      write(STDOUT_FILENO, myBufferPointer +(*inptr * CHUNCK_SIZE), CHUNCK_SIZE);
      write(STDOUT_FILENO, "PARENT: ITEM = %s%\n", CHUNCK_SIZE);
      *inptr = (*inptr + 1) % CHUNCK_SIZE;
    }
    //Here our pipe process each time the producer puts a chuck of source file content
    char line[p_line];
    
    close(p[0]);
    write(STDOUT_FILENO, "PARENT: The producer value of shMemPrntCharCount  = %d\n", f1);
    write(STDOUT_FILENO, line, p_line);
    write(STDOUT_FILENO, "\n", 1);
    write(p[1], line, p_line); // Write data to the pipe
       
  }
    
  else{
      int next_consumed;
      int shMenChidCharCount = 0;
     //Child process code to read from shared memory and write to output file

    while(shMenChidCharCount < fileSize){ // It suppose to check if the count if the child count is bigger than output size than the pointers are
         if(*inptr == *outptr){
           //Buffer is empty so do nothing
         }
    next_consumed = write(f2, myBufferPointer + (*outptr * CHUNCK_SIZE), CHUNCK_SIZE);
    shMenChidCharCount += next_consumed;
    char *child_out;  

    //Here we use a write function for console output  
    
     
    write(STDOUT_FILENO, "CHILD: OUT = %d\n ", *outptr); 
    write(STDOUT_FILENO, child_out, sizeof(child_out));
    write(STDOUT_FILENO, "\n", 1);  

    
    write(STDOUT_FILENO, myBufferPointer +(*outptr * CHUNCK_SIZE), CHUNCK_SIZE);
    write(STDOUT_FILENO, "CHILD: ITEM = %s%\n", CHUNCK_SIZE);  
    *outptr = (*outptr + 1) % CHUNCK_SIZE;  
      
    }
  int z;
    
  char line[p_line];
    
  close(p[1]); //Close write end of the pipe
    
  z = read(p[0], line, p_line); // Read data to the pipe
  write(STDOUT_FILENO, "CHILD: The producer value of shMemPrntCharCount  = %d\n", f2);
  write(STDOUT_FILENO, line, p_line);
  write(STDOUT_FILENO, "\n", 2);
  write(p[1], line, p_line);  

  
  
  write(STDOUT_FILENO, "CHILD: The consumer value of shMemChidCharCount  = %d\n", f2);
  write(STDOUT_FILENO, line, p_line);
  write(STDOUT_FILENO, "\n", 2);
    
  
  }
  close(f1);
  close(f2);
  shm_unlink("MyBuffer");
  shm_unlink("Inptr");
  shm_unlink("Outptr");
  
         
 return 0; 
}