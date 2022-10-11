#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int read_CL(char *passArg[]);
int strngcmp(char *s1, char *s2);

int main()
{
  printf("start\n");
  char shell[] = "mysh$ ";
  char *args[MAX_LINE];
  int size = read_CL(args);

  /*
  while(1)
    {
      printf("%s", shell);
      read_CL(args);
      printf("%s\n", args[0]);
      return 0;
    }
  */

  return 0;
}




int read_CL(char *passArg[])
{
  char exit[] = "exit";
  int i;

  printf("in read cl\n");

  int bytes_read = read(0, passArg, MAX_LINE);
  if(bytes_read == 0)
    {
      printf("enter command\n");
    }
  else if(bytes_read == -1)
    {
      printf("error");
      /* handle error*/
    }
  else
    {
      /* construct array from arguments*/
    }


  /*
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
      while(i < argc)
        {
          passArg[i] = argv[i+1];
          i++;
        }

    }
  */
  
  return bytes_read;
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
