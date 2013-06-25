#ifndef CPHONEBOOK_H_
#define CPHONEBOOK_H_
#include <string>
#include <sstream>
#include <iostream>
#include <sqlite3.h>
#include <unistd.h>
#include "CphonebookException"

using std::string;
using std::cout;
using std::endl;
using std::stringstream;

typedef struct
{
unsigned int pages;
unsigned int contacts;
} book_info_t;

typedef struct
{
int id;
string fname,
string lname,
string addr,
string email
} contact_t;

/***
 *  Helper functions
 * 
 * */

static int callback_get_book_info(void * bookinfo, int argc, char **argv, char **azColName);
static int calback_get_contact_info(void* contact, int argc, char** argv, char** azColName);

class Cphonebook
{
private:
    sqlite3 *db;
	CphonebookException except;
	string dbfile;
	bool dbase_opened;

public:
	~Cphonebook();
	 Cphonebook();

	 /***
	  *  Will open a sqlite file where
	  *  the phonebook data is stored. If
	  *  it wasn't foud a new db is created under
	  *  the name passed.
	  *  */
	 void open(const string& dbase) throw(CphonebookException);

	 /***
	  * Will add a contact to a page(table) on the book(database).
	  * Throws an exception if the page was not found or if the
	  * database was not succesfully opened.
	  * or if the page is less than 1.
	  * */
	 void add_contact(unsigned int page,
	                  const string& fname,
					  const string& lname,
					  const string& addr,
					  const string& phone,
					  const string& email) throw(CphonebookException);

	/***
	 *  Appends a page(table) to the book(database).
	 *  If the database was not opened succesfully
	 *  an exception is thrown.
	 *  */
	void append_page() throw(CphonebookException);

	/***
	 *  Creates a book(database) with a number of pages.
	 *  Throws an exception if there was an error creating
	 *  the database. 
	 *  */
	void create(const string& db, unsigned int npages) throw(CphonebookException);
	
	/***
	 * Gets a contact in the book(database)
	 * given the page and id. Throws an 
	 * exception if the contact id does not
	 * exist or the page, and also if the dbase
	 * was not opened.
	 * */
	void get_contact_at(contact_t& dst,
						unsigned int page,
						unsigned int id); throw(CphonebookException);
	
	/***
	 * Sets a contact to new information
	 * given the page and id. Throws an exception
	 * on common errors.
	 * */
	void set_contact_at(const contact_t& src,
						unsigned int page,
						usigned int id) throw(CphonebookException);
	
	/***
	 * Removes a contact given page and id.
	 *  Throws an exception if page or id 
	 *  do not exist, or if the database
	 *  was not opened.
	 *  */
	void delete_contact_at(unsigned int page,
						   unsigned int id) throw(CphonebookException);

	/***
	 * Removes a page(table) given the
	 * number. throws an exception
	 * if the page does not exist 
	 * or if the dbase was not opened.
	 * */
	void delete_page(unsigned int page); throw(CphonebookException);

	/***
	 * Returns the number of pages(tables)
	 * in the book(database). Throws an 
	 * exception if the dbase was not opened.
	 * */
	int get_n_page(); throw(CphonebookException);

	/***
	 * Returns th number of contacts
	 * in a page given the page(table) number.
	 * Throws an exception if the page does not
	 * exist or dbase was not opened.
	 * */
	int get_n_contacts_at(unsigned int page); throw(CphonebookException);

	/***
	 * Returns the number of contacts in the whole
	 * book(datavase). Throws an exception if
	 *  the dbase was not opened.
	 *  */
	int get_n_contacts();

	/***
	 * Closes the database(the actual sqlite handle)
	 * */
	void close(); throw(CphonebookException);
	 							
};

#endif
