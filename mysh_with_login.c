#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_LINE 80
#define READ_END  0
#define WRITE_END 1


int login();
int read_CL(char *pass_arg[], char *pass_arg2[], char temp_array[][MAX_LINE]);
int strngcmp(char *s1, char *s2);
int create_proc(char *pass_arg[], char *pass_arg2[], int num_arg);
int back_job(char *pass_arg[], int num_arg);
void flush(char *pass_arg[]);
int pipe_arg(char *pass_arg2[], char buff[], int num_arg);

int main()
{
  char shell[] = "mysh$ ";
  char exit_str[] = "exit";
  int run = 1;
  char *args[MAX_LINE];
  char *args2[MAX_LINE];
  char temp_array[MAX_LINE][MAX_LINE];
  int num_arg;
  int succ_login = 0;
  args2[0] = NULL;

  printf("start\n");


  while(!succ_login)
    {
      succ_login = login();
    }

  while(run)
    {
      write(1, shell, 6);
      num_arg = read_CL(args, args2, temp_array);

      if(num_arg != -2)
        {
          if(strngcmp(args[0], exit_str) == 0)
            {
              run = 0;
            }
          else if(num_arg == -1)
            {
              printf("read error\n");
            }
          else
            {
              create_proc(args, args2, num_arg);

            }
        }
      flush(args);
      flush(args2);
    }


  return 0;
}


/*
   Purpose: To create a child process
   Details: Given tokenized input in pass_arg or both pass_arg and pass_arg2 this function
   creates a child process and said child executes the command given. Regular commands and
   background jobs use pass_arg while command pipelines use both pass_arg and pass_arg2.
   This is because command pipelines require two child processes which entails two
   strings for two execve calls.

   Return value: 0 upon normal execution and -1 upon any errors.

*/


int create_proc(char *pass_arg[], char *pass_arg2[], int num_arg)
{
  int pipefd[2];
  int pid1, pid2;
  int child_status;
  char * const envp[] = { NULL };


  pipe(pipefd);

  if(back_job(pass_arg, num_arg))
    {
      pid1 = fork();

      pass_arg[num_arg - 1] = NULL;
      if (pid1 == 0)
        {
          setpgid(0, 0);
          execve(pass_arg[0], pass_arg, envp);
        }
      waitpid(pid1, &child_status, WNOHANG);
    }
  else if(pass_arg2[0] != NULL)
    {
      pid1 = fork();
      /* we must tokenize the input into 2 separate arrays(or more depending on how many arguments we can
         pipe)

       */
      if (pid1 == 0)
        {
          close(pipefd[READ_END]);
          dup2(pipefd[WRITE_END], 1);
          execve(pass_arg[0], pass_arg, envp);
        }

      close(pipefd[WRITE_END]);

      pid2 = fork();

      if (pid2 == 0)
        {
          dup2(pipefd[READ_END], 0);
          execve(pass_arg2[0], pass_arg2, envp);
        }

      close(pipefd[READ_END]);

      waitpid(pid1, &child_status, 0);
      waitpid(pid2, &child_status, 0);


    }
  else
    {
      printf("reg proc\n");
      pid1 = fork();
      if (pid1 == 0)
        {
          execve(pass_arg[0], pass_arg, envp);
        }
      waitpid(pid1, &child_status, 0);
    }

  return 0;

}


/*
   Purpose: To identify if the '&' is in input
   Details: Loops through pass_arg and compares element in array to '&'
   by calling strngcmp()

   Return value: 0 if '&' is NOT in array and 1 otherwise

*/



int back_job(char *pass_arg[], int num_arg)
{
  int flag = 0;
  int i;
  char and[] = "&";

  for(i = 0; pass_arg[i] != NULL && flag == 0 && i < num_arg; i++)
    {
      if(strngcmp(pass_arg[i], and) == 0)
        {
          flag = 1;
          return flag;
        }
    }

  return flag;
}

int pipe_arg(char *pass_arg2[], char buff[], int num_arg)
{
  return 0;
}



/*
   Purpose: Tokenize input
   Details: Upon a successful read this function will tokenize the input in one of two ways.

   1. No command pipeline or IO redirection: In this case the function will tokenize
   the input and store everything in the pass_arg array

   2. Command pipeline or IO redirection seen: The function will read the first part
   of the input until to sees a '|' or '<' or '>' it then stores everythihg into pass_arg
   and then continues reading until '\n' and then stroes everything after the symbol into
   pass_arg2.

   The input is read into a char array called "buff" buff is then tokenized into the 2D array
   "temp_array" and then pass_arg or pass_arg2 is set to point at the first character in each
   string.

   ex. if input is: /bin/ls -al | /usr/bin/wc -l
   temp_array[0][7] = /bin/ls
   pass_arg[0] = /bin/ls
   temp_array[1][3] = -al
   pass_arg[1] = -al
   temp_array[2][11] = /usr/bin/wc
   pass_arg[2] = /usr/bin/wc
   temp_array[3][2] = -l
   pass_arg[3] = -l

   Return value: returns "pointer counter" representing the number of arguments
   and a successful read. -2 if no commands were entered and -1 upon any read errors

*/
int read_CL(char *pass_arg[], char *pass_arg2[], char temp_array[][MAX_LINE])
{
  char buff[MAX_LINE]; /*buffer of user input*/
  int main_counter, char_counter, pointer_counter;
  char temp; /*holds the character to be read*/
  char com_pipe[] = "|";
  int flag = 0;
  int arg2_counter = 0;
  int bytes_read = read(0, buff, MAX_LINE); /*read user input*/

  /* printf("bytes read: %d\n", bytes_read); */


  main_counter = 0;
  pointer_counter = 0;


  /* pointer counter will also tell us how many tokens there are*/


  if(bytes_read == 1)
    {
      printf("enter command\n");
      /* upon return to main read CL should be called again*/
      pointer_counter = -2;
    }

  else if(bytes_read == -1)
    {
      printf("error");
      pointer_counter = -1;
      /* terminate shell*/
    }
  else
    {
      /* return constructed array from arguments*/
      while(main_counter < bytes_read)
        {
          temp = buff[main_counter];
          if(temp == '|')
            {
              flag = 1;
              /* "consume" the | as it does not need to be in arg array*/
              main_counter++;
              /*printf("main count after flag: %d\n", main_counter); */
              temp = buff[main_counter];
              if(temp == ' ')
                {
                  main_counter++;
                  temp = buff[main_counter];
                }
            }

          char_counter = 0;
          /*grab valid character*/
          while(temp != ' ' && temp != '\n')
            {
              temp_array[pointer_counter][char_counter] = temp;
              char_counter++;
              main_counter++;
              temp = buff[main_counter];
            }
          /*
          printf("main count: %d\n", main_counter);
          */
          if(flag == 0)
            {
              temp_array[pointer_counter][char_counter] = '\0';

              pass_arg[pointer_counter] = &temp_array[pointer_counter][0];
            }
          else
            {

              temp_array[pointer_counter][char_counter] = '\0';

              pass_arg2[arg2_counter] = &temp_array[pointer_counter][0];
              arg2_counter++;
            }

          pointer_counter++;
          main_counter++;

        }
      /*printf("main count: %d\n", main_counter);*/

    }



  return pointer_counter;
}


/* compares 2 strings to see if they are equal
   return value of 0 means strings are equal
   anything else means not equal
*/
int strngcmp(char *s1, char *s2)
{
  int i = 0;
  int flag = 0;


  while(s1[i] != '\0' && s2[i] != '\0')
    {

      if(s1[i] != s2[i])
        {
          flag = 1;
          break;
        }
      i++;
    }


  return flag;
}


void flush(char *pass_arg[])
{
  int i;

  for(i = 0; i < MAX_LINE; i++)
    {
      pass_arg[i] = NULL;
    }
}


int login()
{
  char buff[MAX_LINE];
  char buff2[MAX_LINE];
  char user[] = "sprak802\n";
  char password[] = "password123\n";
  int bytes_read;
  int temp;

  printf("Enter username: \n");
  bytes_read = read(0, buff, MAX_LINE);
  if(bytes_read > 1)
    {
      temp = strngcmp(buff, user);
      if(temp == 0)
        {
          /* username matches */
          printf("Enter password: \n");
          bytes_read = read(0, buff2, MAX_LINE);
          if(bytes_read > 1)
            {
              temp = strngcmp(buff2, password);
              if(temp == 0)
                {
                  return 1;
                }
            }
        }

    }
  return 0;
}
