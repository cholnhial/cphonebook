#include "Cphonebook.h"

Cphonebook::Cphonebook()
{
	dbfile = "";
	dbase_opened = false;
}

Cphonebook::~Cphonebook()
{
	if(dbase_opened)
	{
		sqlite3_close(&db);
	}
}

void Cphonebook::open(const string dbase);
{
	char* err_msg = 0;
	int rc;
	string e_msg;
	string sql_query;
	bool dbexists = false;

	/**
	 * Check if db file
	 * already exists.
	 * */
	if(access(dbase.c_str(), F_OK))
	{
		dbexists = true;	
	}

	/**
	 *  Open the database, it will be created
	 *  if it doesn't exist
	 *  */
	rc = sqlite3_open(dbase.c_str(), &db);

	if(rc)
	{ 
		
		e_msg = "Can't open database: " + sqlite3_errmsg(db);
		except.set_msg(e_msg);
		throw(except);
	}
	
	/**
	 * Add phonebook information
	 * table if this is the first
	 * time creating this phonebook
	 **/

	 //build query
	 sql_query = "CREATE TABLE book_info "
	             "(Pages int, "
				 " Contacts int); "
				 "INSERT INTO book_info (Pages, Contacts) "
				 "VALUES(0, 0);";
	
	// execute the SQL query
	rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + err_msg;	
		except.set_msg(e_msg);
		throw(except);
	}


	/** 
	 * No issues so far
	 * */
	dbfile = dbase;
	dbase_opened = true;
}

void Cphonebook::add_contact(unsigned int page,
	                  const string& fname,
					  const string& lname,
					  const string& addr,
					  const string& phone,
					  const string& email)
{
	if(page < 1)
	{
		except.set_msg("Page number can't be zero.");
		throw(except);
	}

	if(!dbase_opened)
	{
		except.set_msg("Database was not opened.");
		throw(except);
	}
	
	/***
	 * Add contact to page(table)
	 * */
	string sql_query;
	string table;
	string e_msg;
	char* err_msg = 0;
	int rc;

	stringstream(table) << page;

	// build query
	sql_query = "INSERT INTO " + table + " (FNAME, LNAME, ADDR, PHONE, EMAIL)"
				+ " VALUES " + "(" + fname + "," + lname + "," + addr + "," + 
				phone + "," + email + ");";

	// execute the SQL query
	rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + err_msg;	
		except.set_msg(e_msg);
		throw(except);
	}


}

void Cphonebook::append_page()
{
	string e_msg, sql_query, pagestr;
	char* err_msg = 0;
	book_info_t binfo;
	int page, rc;

	if(!dbase_opened)
	{
    	except.set_msg("Database was not opened.");
		throw(except);
	}

	sql_query = "SELECT * FROM book_info;";

	rc = sqlite3_exec(db, sql_query.c_str(), callback_get_book_info, &binfo, &err_msg);
	if(rc != SQLITE_OK)
	{
	e_msg = "SQL error: " + err_msg;
	slite3_free(err_msg);
	except.set_msg(e_msg);
	throw(except);
	}

	/***
	 *  We got the information 
	 *  by this point 
	 **/
	page = (binfo.pages)++1;
	stringstream(pagestr) << page;
	/** 
	 * Now add the new page 
	 **/

	sql_query = "CREATE TABLE " + page +
				" (FNAME varchar(255),"
		        " LNAME varchar(255),"
				" ADDR varchar(255),"
				" PHONE varchar(64),"
				" EMAIL varchar(255));";

	rc = sqlite3_exc(db, sql_query.c_str(), NULL, NULL, &err_msg);
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + err_msg;
		except.set_msg(e_msg);
		sqlite3_free(err_msg);
		throw(except);
	}


}

