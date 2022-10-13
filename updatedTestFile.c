#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int read_CL(char *passArg[], char tempArray[][MAX_LINE]);
int strngcmp(char *s1, char *s2);

int main()
{
  printf("start\n");
  char shell[] = "mysh$ ";
  char *args[MAX_LINE];
  char tempArray[MAX_LINE][MAX_LINE];
  read_CL(args, tempArray);
  printf("\n");


  printf("%s\n", args[0]);
  printf("%s\n", args[1]);
  printf("%s\n", args[2]);

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




int read_CL(char *passArg[], char tempArray[][MAX_LINE])
{
  char exit[] = "exit";
  char buff[MAX_LINE]; /*buffer of user input*/
  int mainCounter, charCounter, pointerCounter;
  char temp; /*holds the character to be read*/
  
  int bytes_read = read(0, buff, MAX_LINE); /*read user input*/

  printf("bytes read is %i\n", bytes_read);

  buff[bytes_read] = '\0'; /*null terminate buffer*/

  mainCounter = 0;
  pointerCounter = 0;

  while(mainCounter < bytes_read)
    {
      temp = buff[mainCounter];

      charCounter = 0;

      /*grab valid character*/
      while(temp != ' ' && temp != '\n' && temp != '\0')
         {
          tempArray[pointerCounter][charCounter] = temp;
          printf("%c\n", tempArray[pointerCounter][charCounter]);
          charCounter++;
          mainCounter++;
          temp = buff[mainCounter];
         }

      printf("\n");

      printf("character count equals %d\n", charCounter);

      printf("\n");

      tempArray[pointerCounter][charCounter] = '\0';

      passArg[pointerCounter] = &tempArray[pointerCounter][0];

      printf("%s\n", passArg[0]);

      pointerCounter++;

      mainCounter++;

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
      /* return constructed array from arguments*/
      return bytes_read;
    }


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

