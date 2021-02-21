#include "libTypes.h"
//user




int main()
{
  char token[INPUT] = {'\0'};
  system("clear");
  printf("Welcome to library 1.0, type \"help\" for more information.\n");
  srand(time(NULL));
  while(1)
  {
    printf("\nlib_1.0> ");
    fgets(token,INPUT,stdin);
    for(int i = 0; i<strlen(token); i++)
      token[i] = tolower(token[i]);
    if(!strcmp(token,"quit\n"))
      break;
    else
    {
      token[strlen(token)-1] = '\0';
      getInput(token);
    }
  }
  printf("Goodbye\n");
  return 0;
}
