#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#ifndef LIB_H
#define LIB_H
#define INPUT 31

/*Structure representing a book in the library*/
typedef struct Books
{

  char renter[51];
  char dateOfReturn[11];
  char title[51];
  char author[51];
  bool valid;
}book_t;


/*Structure for representing a user in the library*/
typedef struct User
{
  int numBooks;
  char firstName[51];
  char lastName[51];
  bool valid;
  book_t books[100];
}user_t;


void getInput(char * input);
void printOptions();


/*Terminal UI for managing users*/
void addUser();
void listUsers();
void findUser();
void deleteUser();
void checkout();
void promptUserInfo(char* first, char* last);

/*Database commands managing users.*/
bool query_user(const char* first, const char* last,long int* pos, user_t* userObj);
void save_user(user_t* user,long int offset, const char* mode);

/*Terminal UI for managing books*/
void addBook();
void findBook();
void deleteBook();
void listBooks();
void returnBook();
void promptBookInfo(char* title, char* author);

/*Database commands for managing books*/
bool query_book(const char* title,const char* author, long int* index, book_t* book);
void save_book(book_t* item, long int index, const char* mode); //TODO: Fix the params so they line up with the book query


long int getFileSize(FILE* f);

#endif
