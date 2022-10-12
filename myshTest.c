#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

char** read_CL(char *passArg[]);
int strngcmp(char *s1, char *s2);

int main()
{
  printf("start\n");
  char shell[] = "mysh$ ";
  char *args2[MAX_LINE];
  char *args[MAX_LINE] = read_CL(args2);
  printf("%s\n", args[0]);

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




char** read_CL(char *passArg[])
{
  char exit[] = "exit";
  char buff[MAX_LINE];
  char tempArray[MAX_LINE];
  int i, j;
  char temp;


  int bytes_read = read(0, buff, MAX_LINE);
  buff[bytes_read] = '\0';
  i = 0;
  int h = 0;

  while(i < bytes_read)
    {
      temp = buff[i];
      j = 0;
      while(temp != ' ' && temp != '\n' && temp != '\0')
        {
          tempArray[j] = temp;
          j++;
          i++;
          temp = buff[i];
        }
      printf("j equals %d\n", j);
      tempArray[j] = '\0';
      j = 0;
      while(tempArray[j] != '\0')
        {
           printf("%c\n", tempArray[j]);
        j++;
        }
      passArg[h] = &tempArray[0];
      h++;
      printf("%s\n", passArg[0]);

      i++;
    }



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
      return passArg;
    }




  return passArg;
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
