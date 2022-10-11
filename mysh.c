#include <stdio.h>

#define MAX_LINE 80

int read_CL(int argc, char *argv[], char *passArg[]);
int strngcmp(char *s1, char *s2);

int main(int argc, char *argv[])
{

  int i;
  char *passArg[4];

  if(read_CL(argc, argv, passArg) == 2)
    {
      printf("Shell terminated\n");
    }
  else
    {
      printf("\n");
       for(i = 0; i < argc - 1; i++)
        {
          printf("%s\n", passArg[i]);
        }

    }

  return 0;
}




int read_CL(int argc, char *argv[], char *passArg[])
{
  char exit[] = "exit";
  int i = 0;

  if(argc < 2)
    {
      printf("Too few arguments\n");
      return 0;
    }
  else if(strngcmp(argv[1], exit) == 0)
    {
      return 2;
    }
  else
    {
      /* construct a char * const argv1[] structure*/
      while(i < argc)
        {
          passArg[i] = argv[i+1];
          i++;
        }

    }

  return 1;
}


int strngcmp(char *s1, char *s2)
{
  int i = 0;
   while(s1[i] == s2[i] && s1[i] != '\0')
    {
      i++;
    }

  return s1[i] - s2[i];
}



