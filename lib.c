#include "libTypes.h"


/*
  - cleanup UI with prompting the user.
*/

void getInput(char* input)
{
  system("clear");
  if(!strcmp(input,"show-users"))
    listUsers();
  else if(!strcmp(input,"add-user"))
    addUser();
  else if(!strcmp(input,"find-user"))
    findUser();
  else if(!strcmp(input,"delete-user"))
    deleteUser();
  else if(!strcmp(input,"checkout"))
    checkout();
  else if(!strcmp(input,"find-book"))
    findBook();
  else if(!strcmp(input,"show-books"))
    listBooks();
  else if(!strcmp(input,"add-book"))
    addBook();
  else if(!strcmp(input,"delete-book"))
    deleteBook();
  else if(!strcmp(input,"help"))
    printOptions();
  else if(!strcmp(input,"return-book"))
    returnBook();
  else if(!strcmp(input,"clear"))
    system("clear");
  else
    printf("Unrecognized command, type \"help\" for a list of commands");
}


void addUser()
{
  char first[50],last[50];
  user_t user;
  promptUserInfo(first,last);
  strcpy(user.firstName,first);
  strcpy(user.lastName,last);
  user.numBooks = 0;
  user.valid = true;
  save_user(&user,0,"ab");
  printf("%s %s has been added to the library database.\n",user.firstName,user.lastName);
}


/*List the book titles that the user owns*/
void findUser()
{
  char fName[51],lName[51];
  long int index = 0;
  user_t user;
  printf("--Find user--\n");
  promptUserInfo(fName,lName);
  if(query_user(fName,lName,&index,&user))
  {
    printf("\n----Queried result---\n");
    printf("\nFirst name: %s\nLast name: %s\nNumber of books rented: %d\n\n",
    user.firstName,user.lastName,user.numBooks);
    printf("\n###List of rented books###\n\n");
    for(int i = 0; i<user.numBooks; i++)
    {
      printf("%s: %s\n",user.books[i].title,user.books[i].author);
      printf("Date of return: %s\n",user.books[i].dateOfReturn);
      printf("------------------------\n");
    }
  }
  else
    printf("Unable to find \"%s %s\" in the database.\n",fName,lName);
}


/*Remove a user from the library database, return all their rented books*/
void deleteUser()
{
  char first[51], last[51];
  user_t user;
  book_t book;
  long int index = 0,index2 = 0;
  printf("\nPlease enter the following fields to delete a user\n");
  promptUserInfo(first,last);
  if(query_user(first,last,&index,&user))
  {
    user.valid = false;
    for(int i  =0; i<user.numBooks; i++)
    {
      query_book(user.books[i].title,user.books[i].author,&index2,&book);
      strcpy(book.renter,"None");
      strcpy(book.dateOfReturn,"None");
      save_book(&book,index2,"r+b");
    }
    save_user(&user,index,"r+b");
    printf("\"%s %s\" has been removed from the library database\n",user.firstName,user.lastName);
  }
  else
    printf("User \"%s %s\" does not exist in the database\n",first,last);
}


/*Retrieve a book from the database and add it to the user account.*/
void checkout()
{
  char first[51], last[51], title[51],
  author[51], dateOfReturn[11];
  long int idx1 = 0, idx2 = 0;
  user_t user; book_t book;
  printf("\n#####Book checkout#####\n");
  printf("\nPlease enter the following fields.\n\n");
  promptUserInfo(first,last);
  promptBookInfo(title, author);
  if(query_user(first,last,&idx1,&user))
  {
    if(query_book(title,author,&idx2,&book)&&!strcmp(book.renter,"None"))
    {
      if(user.numBooks <=99)
      {
        sprintf(dateOfReturn,"%d-%d-%d",rand()%12+1,rand()%31+1,rand()%5+2021);
        strcpy(user.books[user.numBooks].title,book.title);
        strcpy(user.books[user.numBooks].author,book.author);
        strcpy(user.books[user.numBooks].renter,user.firstName);
        strcpy(book.renter,user.firstName);
        strcpy(user.books[user.numBooks].dateOfReturn,dateOfReturn);
        strcpy(book.dateOfReturn,dateOfReturn);
        user.numBooks++;
        save_user(&user,idx1,"r+b");
        save_book(&book,idx2, "r+b");
        printf("\"%s\" by %s has been checked out.\nPlease return by %s.\n Enjoy your read!\n",
        book.title, book.author,book.dateOfReturn);
      }
      else
        printf("User \"%s %s\" has exceeded the number of books to rent (100)",first,last);
    }
    else
      printf("\"%s\" by %s is already being rented by another user.\n",book.title,book.author);
  }
  else
    printf("\"%s %s\" does not exist in database.",first,last);

}

/*Handles creating and updating users in the database*/
void save_user(user_t* user, long int offset, const char* mode)
{
  FILE* f = fopen("Users.bin",mode);
  if(f == NULL)
  {
    printf("Unable to open Users.bin\n");
    exit(1);
  }
  if(offset>0)
    fseek(f,offset-sizeof(user_t),SEEK_SET);
  fwrite(user,sizeof(user_t),1,f);
  fclose(f);
}


/*Reads from the Users.bin database file, returns true if users exists.*/
bool query_user(const char* first, const char* last,long int* pos, user_t* user)
{
  FILE* f = fopen("Users.bin","rb");
  bool found  = false;
  while(!feof(f))
  {
    fread(user,sizeof(user_t),1,f);
    if(!strcmp(user->firstName,first)&&!strcmp(user->lastName,last)
    &&user->valid)
    {
      *pos = ftell(f);
      found = true;
      break;
    }
  }
  fclose(f);
  return found;
}


/*Print a list of users in the database by lastname,firstname*/
void listUsers()
{
  FILE* fp = fopen("Users.bin","rb");
  user_t user;
  int size = getFileSize(fp)/sizeof(user);
  printf("---List of users(%d)---\n",size);
  for(int i = 0; i<size; i++)
  {
      fread(&user,sizeof(user_t),1,fp);
      if(user.valid)
        printf("%s, %s\n",user.lastName,user.firstName);
  }
  fclose(fp);
}


/*Prompt the user to add a certain book and save it to the books binary file.*/
void addBook()
{
  book_t newBook;
  char title[51],author[51];
  promptBookInfo(title,author);
  strcpy(newBook.title,title);
  strcpy(newBook.author,author);
  strcpy(newBook.dateOfReturn,"None");
  newBook.valid = true;
  strcpy(newBook.renter,"None");
  save_book(&newBook,0,"ab");
  printf("\"%s\" by %s has been added to the library,\n",newBook.title,newBook.author);

}


/*Prompts the user for first and last name, works as a helper function.*/
void promptUserInfo(char* first, char* last)
{
  printf("First name: ");
  fgets(first,50,stdin);
  first[strlen(first)-1] = '\0';
  printf("Last name: ");
  fgets(last,50,stdin);
  last[strlen(last)-1] = '\0';
}


/*Prompts the user for book info, works as a helper function.*/
void promptBookInfo(char* title, char* author)
{
  printf("Book title: ");
  fgets(title,50,stdin);
  title[strlen(title)-1] = '\0';
  printf("Author: ");
  fgets(author,50,stdin);
  author[strlen(author)-1] = '\0';
}


/*Save book structure to a binary file Update/Create*/
void save_book(book_t* item, long int index, const char* mode)
{
  FILE* f = fopen("Books.bin",mode);
  if(f == NULL)
  {
    printf("Unable to open Books.bin\n");
    exit(1);
  }
  if(index>0)
    fseek(f,index-sizeof(book_t),SEEK_SET);
  fwrite(item,sizeof(book_t),1,f);
  fclose(f);
}


/*Retrieves a book from the database and prints its contents for the user to see.*/
void findBook()
{
  char title[51],author[51];
  book_t book;
  long int index = 0;
  promptBookInfo(title,author);
  if(!query_book(title,author,&index,&book))
    printf("\"%s\" by %s does not exist in the library database\n",title, author);
  else
  {
    printf("\n----Queried Result---\n");
    printf("Title: %s\nAuthor: %s\nRenter: %s\nDate of return: %s\n",
    book.title,book.author,book.renter,book.dateOfReturn);
  }
}


/*Functionality for returning a book to the database.*/
void returnBook()
{
  char first[51], last[51],
  title[51], author[51];
  user_t user;
  book_t book;
  long int pos1 = 0, pos2 = 0;
  promptUserInfo(first,last);
  promptBookInfo(title,author);
  printf("-----Book Return-----\n");
  if(query_user(first,last,&pos1,&user))
  {
    if(query_book(title,author,&pos2,&book))
    {
      strcpy(book.dateOfReturn,"None");
      strcpy(book.renter,"None");
      user.numBooks--;
      save_user(&user,pos1,"r+b");
      save_book(&book,pos2,"r+b");
      printf("\"%s\" by %s has been returned to the library\n",book.title,book.author);
    }
    else
      printf("Unable to find \"%s\" by %s in the database\n",title, author);
  }
  else
    printf("\"%s %s\" cannot be found in the database",first,last);

}


/*Search Books.bin for a specific book in the library, Read in CRUD*/
bool query_book(const char* title,const char* author,long int* index, book_t* book)
{
  FILE* fp = fopen("Books.bin","rb");
  bool found = false;
  if(fp == NULL)
  {
    printf("Unable to open Books.bin\n");
    exit(1);
  }
  while(!feof(fp))
  {
    fread(book,sizeof(book_t),1,fp);
    if(!strcmp(book->title,title)
    && book->valid&&!strcmp(book->author,author))
      {
        *index = ftell(fp);
        found = true;break;
      }
  }
  fclose(fp);
  return found;
}



/*Print out a list of books in the library.*/
void listBooks()
{
  book_t book;
  FILE* fp = fopen("Books.bin","rb");
  int size = getFileSize(fp)/sizeof(book_t);
  printf("\n---list of book titles (%d)---\n",size);
  for(int i = 0; i<size; i++)
  {
    fread(&book,sizeof(book_t),1,fp);
    if(book.valid)
      printf("%s : %s \n",book.title, book.author);
  }
  fclose(fp);
}


/*Remove a book from the library system by updating the valid bit to false.*/
void deleteBook()
{
  char title[51],author[51];
  long int index = 0;
  book_t book;
  printf("Please enter a book title to remove from the library\n");
  promptBookInfo(title,author);
  if(query_book(title,author,&index,&book))
  {
    book.valid = false;
    save_book(&book,index,"r+b");
    printf("\"%s\" by %s has been successfully deleted from the library shelf.\n",book.title,book.author);
  }
  else
    printf("\"%s\" by %s does not exist in the database\n",title, author);
}


/*Returns the size of a binary file*/
long int getFileSize(FILE* fp)
{
  long int size;
  fseek(fp,0L,SEEK_END);
  size = ftell(fp);
  fseek(fp,0L,SEEK_SET);
  return size;
}


/*Prints all the commands for the user to navigate around the library.*/
void printOptions()
{
  printf("\n-----###LIST OF COMMANDS###-----\n");
  printf("show-users\n");
  printf("add-user\n");
  printf("find-user\n");
  printf("delete-user\n");
  printf("checkout\n");
  printf("find-book\n");
  printf("show-books\n");
  printf("add-book\n");
  printf("delete-book\n");
  printf("return-book\n");
  printf("help\n\n");
}
