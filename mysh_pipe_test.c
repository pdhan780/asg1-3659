#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_LINE 80
#define READ_END  0
#define WRITE_END 1



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
  args2[0] = NULL;

  printf("\n");


  while(run)
    {
      write(1, shell, 6);
      num_arg = read_CL(args, args2, temp_array);
      /* printf("%s\n", args[0]); */
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


int create_proc(char *pass_arg[], char *pass_arg2[], int num_arg)
{
  int pipefd[2];
  int pid1, pid2;
  int child_status;
  char * const envp[] = { NULL };


  printf("in create proc\n");
  pipe(pipefd);

  pid1 = fork();
  printf("forked \n");

  if(back_job(pass_arg, num_arg))
    {
      /*printf("in background\n");*/
      pass_arg[num_arg - 1] = NULL;
      if (pid1 == 0)
        {
          setpgid(0, 0);
          execve(pass_arg[0], pass_arg, envp);
        }
      waitpid(pid1, &child_status, WNOHANG);
    }
  else if(pass_arg2[0] == NULL)
    {
      printf("arg2 is null\n");
    }
  else if(pass_arg2[0] != NULL)
    {
      printf("in command pipe\n");
      /* we must tokenize the input into 2 separate arrays(or more depending on how many argumen

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

      if (pid1 == 0)
        {
          execve(pass_arg[0], pass_arg, envp);
        }
      waitpid(pid1, &child_status, 0);
    }

  return 0;

}

int back_job(char *pass_arg[], int num_arg)
{
  int flag = 0;
  int i;
  char and[] = "&";

  for(i = 0; flag == 0 && i < num_arg; i++)
    {
      if(strngcmp(pass_arg[i], and) == 0)
        {
          flag = 1;
          return flag;
          printf("backjob seen\n");
        }
    }

  printf("backjob not seen\n");
  return flag;
}

int pipe_arg(char *pass_arg2[], char buff[], int num_arg)
{
  return 0;
}



/* modify read so when it encounters a '|' it will parse the first part of the input
   into one array and the second part into a different array
   ex: input = ls -al | wc -l; args1[] = "ls -a;" args2[] = "wc -l"
*/
int read_CL(char *pass_arg[], char *pass_arg2[], char temp_array[][MAX_LINE])
{
  char buff[MAX_LINE]; /*buffer of user input*/
  int main_counter, char_counter, pointer_counter;
  char temp; /*holds the character to be read*/
  char com_pipe[] = "|";
  int flag = 0;
  int bytes_read = read(0, buff, MAX_LINE); /*read user input*/
  printf("bytes read: %d\n", bytes_read);


  for(main_counter = 0; main_counter < bytes_read; main_counter++)
    {
      if(buff[main_counter] == '|')
        {
          printf("command pipe identified\n");
        }
    }

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
              printf("in pass arg1\n");
              temp_array[pointer_counter][char_counter] = '\0';

              pass_arg[pointer_counter] = &temp_array[pointer_counter][0];
              printf("pointer counter: %d\n\n", pointer_counter);
              printf("%s\n\n", pass_arg[pointer_counter]);
            }
          else
            {
              printf("in pass arg2\n");
              temp_array[pointer_counter][char_counter] = '\0';

              printf("inside temp array: \n");
              for(int i = 0; i < char_counter; i++)
                {
                  printf("%c", temp_array[pointer_counter][i]);
                }
              printf("\n");

              pass_arg2[pointer_counter] = &temp_array[pointer_counter][0];
              printf("temp array at char counter index 0: %c\n\n", temp_array[pointer_counter][0
              printf("pointer counter: %d\n\n", pointer_counter);
              printf("inside args2 array: \n");
              printf("%s\n\n", pass_arg[pointer_counter]);
            }

          pointer_counter++;
          main_counter++;

        }
      /*printf("main count: %d\n", main_counter);*/

    }

  printf("end of read function\n");

  /* flush buffer
  for(main_counter = 0; main_counter < MAX_LINE; main_counter++)
    {
      buff[main_counter] = '\0';
      }\*/
  return pointer_counter;
}


/* compares 2 strings to see if they are equal
   return value of 0 means strings are equal
   anything else means not equal
*/
int strngcmp(char *s1, char *s2)
{
  int i = 0;

  while(s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
    {
      i++;
    }

  return s1[i] - s2[i];
}


void flush(char *pass_arg[])
{
  int i;

  for(i = 0; i < MAX_LINE; i++)
    {
      pass_arg[i] = NULL;
    }
}


