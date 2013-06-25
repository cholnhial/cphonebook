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

	/**
	 *  Open the database, it will be created
	 *  if it doesn't exist
	 *  */
	rc = sqlite3_open(dbase.c_str(), &db);

	if(rc)
	{ 
		
		e_msg = "Can't open database: " + sqlite3_errmsg(&db) + "\n";
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

	stringstream(table) << page;

	// build query
	sql_query = "INSERT INTO" + table + " (FNAME, LNAME, ADDR, PHONE, EMAIL)"
				+ "VALUES " + "("+ fname + "," + lname + "," + addr + "," + 
				phone + "," + email + ");";




}



