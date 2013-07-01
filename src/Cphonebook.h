#ifndef CPHONEBOOK_H_
#define CPHONEBOOK_H_

#include <unistd.h>

#include <iostream>
#include <cstdlib>

#include <string>
#include <sstream>
#include <mutex>

#include <sqlite3.h>

#include "CphonebookException.h"

using std::string;
using std::cout;
using std::endl;
using std::stringstream;
using std::mutex;

typedef struct
{
    unsigned int pages;
    unsigned int contacts;
} BookInfo;

typedef struct
{
    unsigned int id;
    string firstName;
    string lastName;
    string address;
    string phone;
    string email;
} Contact;


typedef int (*SQLITE3_CALLBACK)(void*,int,char**,char**);


/***
 *  Helper functions
 * 
 * */

static int callbackGetBookInfo (void* bookinfo, int argc,
                                char** argv, 
                                char** azColName);

/***
 *  A callback function that will be called by
 *  sqlite3_exec() when calling Cphonebook::getContactAt()
 **/
static int callbackGetContact(void* dstContact, int argc,
                              char** argv, 
                              char** azColName);

/***
 *  A generic callback function that will be
 *  called when getting a number from the
 *  database 
 */
static int callbackGenericGetNumber(void* dst, int argc,
                                    char** argv,
                                    char** azColName);
/***
 *  Global mutex used for syncronization
 *  With the callback functions
 *  */
static mutex sCallbackBusy;


class Cphonebook
{
private:
    sqlite3* database_;
    CphonebookException exception_;
    string databaseFile_;
    bool isDatabaseOpened_;
    BookInfo bookInfo_;
    
    void waitForDatabase(); 
    void executeSqlQuery(const string& sqlQuery,
                         SQLITE3_CALLBACK callback = NULL,
                         void* callbackData = NULL) throw(CphonebookException);
    void createDatabase(const string& databaseFile) throw(CphonebookException);
    void updateBookInfo();
public:
    ~Cphonebook ();
     Cphonebook ();


     /***
      *  open()
      *
      *  Will open a sqlite file where
      *  the phonebook data is stored. 
      *  if the file does not exist than
      *  it will be created.
      *
      *  Paramaters:
      *    A phonebook(database) file file as a string.
      *
      *  Returns:
      *    none
      *
      *  Throws:
      *    CphonebookException if there is a 
      *    problem opening the sqlite database.
      * 
      *  
      * */
     void open (const string& databaseFile) throw(CphonebookException);
     

      /***
      *  addContact()
      *
      *  Will add a contact to a page(table) on the book(database).
      * 
      *  Paramaters:
      *    Page number as an int. the last paramater
      *    is a Contact holding the information to add.
      *    
      *
      *  Returns:
      *    none
      *
      *  Throws:
      *     CphonebookException if the database file was not
      *     successfully opened or if the page number is t'0'.
      *
      * */


     void addContact(const unsigned int pageNo,
                     const Contact& srcContact) throw(CphonebookException); 

    /***
     *  appendPage()
     *
     *  Appends a page(table) to the book(database).
     *
     *  Paramaters:
     *    none
     *  
     *  Returns:
     *    none
     *
     *  Throws:
     *   CphonebookException if the database was not opened
     *   or if it wont't able to append the page.
     *   
     *  */
    void appendPage () throw(CphonebookException);


    /***
     *  create()
     *
     *  Creates a book(database) with a number of pages.
     *
     *  Paramaters:
     *    a name for the database file as a string. and a
     *    number of the pages to create.
     *  
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if the file name is "", or if
     *    the number of pages to create is 0, and at last if
     *    there was an issue creating the database.
     *  
     *  */
    void create (const string& databaseFile,
                 const unsigned int nPages) throw(CphonebookException);
    

    /***
     *  getContactAt()
     *
     *  Gets a contact in the book(database)
     *  given the page and id number. The contact
     *  information will be stored in 'dstContact'. 
     *  
     *  Paramaters:
     *    A Contact type where the informatation
     *    will be stored, two last paramaters
     *    specify where the contact is located.
     *
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if PageNo and idNO are
     *    less than one, or if the database was not
     *    opened and at last if there was a problem
     *    locating the contact.
     * 
     * */
    void getContactAt (Contact& dstContact,
                       const unsigned int pageNo,
                       const unsigned int idNo) throw(CphonebookException);
    
    /***
     *  setContactAt ()
     *
     *  Sets a contact to new information
     *  given the page and id number.
     *
     *  Paramaters:
     *    A source contact which has the new contact
     *    information, the last two paramaters specify
     *    where to put that information.
     *
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if either PageNO or idNO
     *    is less than one, or if the book(database) was not 
     *    opened, and at last if there was a problem 
     *    setting the contact to the new data.
     *
     * */
    void setContactAt (const Contact& srcContact,
                       const unsigned int pageNo,
                       const unsigned int idNo) throw(CphonebookException);
    

    /***
     *  deleteContactAt()
     *
     *  Removes a contact given page(table) and id number.
     *
     *  Paramaters:
     *    The page number and id number which the contact
     *    is located at.
     *
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if pageNo or idNO is less than one,
     *    or if the book(database) was not opened, and if there was a 
     *    problem removing the contact(e.g does not exist).
     *   
     * */
    void deleteContactAt (const unsigned int pageNo,
                          const unsigned int idNo) throw(CphonebookException);


    /***
     *  deletePage()
     *
     *  Removes a page(table) given the page
     * 
     *  Paramaters:
     *    The page number as a number.
     *  
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if PageNo is less than one, or if
     *    the database was not opened. And if there's a problem
     *    removing the page.
     * 
     * */
    void deletePage (const unsigned int pageNo) throw(CphonebookException);


    /***
     *
     *   getNumberOfPages()
     *
     *   Returns the number of pages(tables)
     *   in the book(database). 
     *   
     *   Paramaters:
     *     none
     *  
     *   Returns:
     *     A number representing the number of
     *     pages in the book(database) as an integer.
     *   
     *   Throws:
     *     CphonebookException if there was a problem
     *     locating the phonebook info table or other
     *     database related issues.
     *
     * */
    int getNumberOfPages () throw(CphonebookException);


    /***
     *
     *  getNumberOfContactsAt()
     *
     *  Returns th number of contacts
     *  in a page given the page(table) number.
     *
     *  Paramaters:
     *    An integer which contains the page number.
     *
     *  Returns:
     *    An integer representing the count of contacts
     *    in the particular page.
     *
     *  Throws:
     *    CphonebookException if PageNo is less than one, or
     *    if the book(database) was not opened, or if there was a 
     *    problem getting the number of contacts in the page.
     * 
     * */
    int getNumberOfContactsAt (const unsigned int pageNo) throw(CphonebookException);


    /***
     *  
     *  getNumberOfContacts()
     *
     *  Returns the number of contacts in the whole
     *  book(database).
     *
     *  Paramaters:
     *    none
     *
     *  Returns:
     *    An integer representing the number of contacts
     *    in the whole book(database).
     *
     *  Throws:
     *    CphonebookException if the book(database) was not opened,
     *    or if there was problem getting the number of contacts.
     *  
     * */
    int getNumberOfContacts () throw(CphonebookException);


    /***
     *  
     *  close()
     *
     *  Closes the database(the actual sqlite handle)
     *  
     *  Paramaters:
     *    none
     *
     *  Returns:
     *    none
     *
     *  Throws:
     *    CphonebookException if busy. 
     *
     * */
    void close () throw(CphonebookException);
                                
};

#endif
