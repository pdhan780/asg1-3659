#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_LINE 80

int read_CL(char *pass_arg[], char temp_array[][MAX_LINE]);
int strngcmp(char *s1, char *s2);
int create_proc(char *pass_arg[], int num_arg);
int back_job(char *pass_arg[], int num_arg);
void flush(char *pass_arg[]);

int main()
{
  char shell[] = "mysh$ ";
  char exit_str[] = "exit";
  int run = 1;
  char *args[MAX_LINE];
  char temp_array[MAX_LINE][MAX_LINE];
  int num_arg;

  printf("\n");
  write(1, shell, 6);
  num_arg = read_CL(args, temp_array);


  while(run)
    {
      if(num_arg != -2)
        {
          if(strngcmp(args[0], exit_str) == 0)
            {
              run = 0;
              exit(0);
            }
          else if(num_arg == -1)
            {
              printf("read error\n");
            }
          else
            {
              create_proc(args, num_arg);

            }
        }
      printf("\n");
      flush(args);
      write(1, shell, 6);
      num_arg = read_CL(args, temp_array);
    }


  return 0;
}


int create_proc(char *pass_arg[], int num_arg)
{
  int pipefd[2];
  int pid1;
  int child_status;
  char * const envp[] = { NULL };


  pipe(pipefd);

  pid1 = fork();

  if(back_job(pass_arg, num_arg))
    {
      printf("in background\n");
      pass_arg[num_arg - 1] = NULL;
      if (pid1 == 0)
        {
          setpgid(0, 0);
          execve(pass_arg[0], pass_arg, envp);
        }
      waitpid(pid1, &child_status, WNOHANG);
    }
  else
    {

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
        }
    }

  return flag;
}



int read_CL(char *pass_arg[], char temp_array[][MAX_LINE])
{
  char buff[MAX_LINE]; /*buffer of user input*/
  int main_counter, char_counter, pointer_counter;
  char temp; /*holds the character to be read*/

  int bytes_read = read(0, buff, MAX_LINE); /*read user input*/
  printf("%d\n", bytes_read);

  buff[bytes_read] = '\0'; /*null terminate buffer*/

  main_counter = 0;
  pointer_counter = 0;


  /* pointer counter will also tell us how many tokens there are*/


  if(bytes_read == 1)
    {
      printf("enter command\n");
      /* upon return to main read CL should be called again*/
      return -2;
    }

  else if(bytes_read == -1)
    {
      printf("error");
      return -1;
      /* terminate shell*/
    }
  else
    {
      /* return constructed array from arguments*/
      while(main_counter < bytes_read)
        {
          temp = buff[main_counter];

          char_counter = 0;

          /*grab valid character*/
          while(temp != ' ' && temp != '\n' && temp != '\0')
            {
              temp_array[pointer_counter][char_counter] = temp;
              char_counter++;
              main_counter++;
              temp = buff[main_counter];
            }

          temp_array[pointer_counter][char_counter] = '\0';

          pass_arg[pointer_counter] = &temp_array[pointer_counter][0];
          pointer_counter++;

          main_counter++;

        }

      return pointer_counter;
    }

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


