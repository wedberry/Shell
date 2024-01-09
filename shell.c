#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int MAX_BUFFER_SIZE = 64;

int main(void) {
  char input[MAX_BUFFER_SIZE];
  char command[16][MAX_BUFFER_SIZE];
  int keep_running = 1;
  char buffer[MAX_BUFFER_SIZE];


  //Write basic loop that displays a prompt and reads from user input
  printf("shell$ \n");
  printf("\n");

  while(keep_running==1){
    
    //reset command array
    for(int i = 0; i < 16; i++){
      strcpy(command[i], "NULL");
    }
    
    printf("> ");
    int rc = scanf("%[^\n]%*c", input);
    if(rc < 0){
      perror("Something went wrong.");
      return 0;
    }

    char* token = strtok(input, " ");

    //Use NULL as the first argument to find subsequent tokens
    //Use index to fill input array with each subsequent token
    int index = 0;
    while(token) {
      strcpy(command[index], token);
      token = strtok(NULL, " ");
      index ++; 
    }

    //Implement the three built-in commands: exit, pwd, and cd
    if(strcmp(input, "exit")==0){
      keep_running = 0;
    }
    
    if(strcmp(input, "pwd")==0){
      if(getcwd(buffer, sizeof(buffer)) == NULL){
        perror("getcwd() error");
        return 0;
      }else{
        printf("%s\n", buffer);
      }
    }
   
    if(strcmp(input, "cd")==0){
      if(chdir(command[1]) < 0){
        perror("chdir error");
        return 0;
      }
    }

    //this runs if command is not exit, pwd, or cd using fork() and execvp
    else if((strcmp(input, "cd")!=0) && (strcmp(input, "pwd")!=0) && (strcmp(input, "exit")!=0)) {
      int other_rc = fork();
      //rc from fork is 0 which means that this is the child process
    // Child executes another program (sleep) via execvp
      if(other_rc == 0) {
        //while loop to fill args with commands
        char *args[16];
        int aindecks = 0;
        int cindecks = 0;
        
        while(strcmp(command[cindecks], "NULL") != 0){
          if(strcmp(command[cindecks], ">") == 0){
            args[aindecks] = NULL;
            //printf("N copied\n");
            aindecks++;
            args[aindecks] = command[cindecks];
            //printf("%s copied to args[%d]\n", command[indecks], indecks);
          }else{
            args[aindecks] = command[cindecks];
            //printf("%s copied to args[%d]\n", command[indecks], indecks);
          }
          aindecks++;
          cindecks++;
        }
        args[aindecks] = NULL;
        
        if(strcmp(args[aindecks -2], ">") == 0){
          //Close stdout, freeing its filedescriptor
          close(1);

          //Open another file, which gets the lowest numbered file 
          //descriptor available: the one just freed by closing stdout
          open(args[aindecks - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
          /*
          // testing this
          char* temporary;
          strcpy(args[indecks-2], temporary);
          strcpy(args[indecks-1], temporary);
          */
        }
        execvp(args[0], args);
        
        // We should never reach here
        perror("execvp");
        return -1;
      }
        
      //rc isn't 0 which means this is the parent process.
      //Parent waits for child to finish
      else if(rc > 0) {
        
        //The basic wait system call 
        int rc_other = wait(NULL);
        if(rc_other < 0) {
          perror("wait");
          return -1;
        }
      }
    
      else {
       perror("fork");
      }
    }
    // ls, sleep, mkdir, ps, -u, rm, -l, cat, -rf, -a, 
  //Make the parent wait until the command finishes
  //Add support for redirection
  }
    return 0;
}