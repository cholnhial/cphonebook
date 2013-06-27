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
		sqlite3_close(db);
	}
}

void Cphonebook::open(const string& dbase) throw(CphonebookException)
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
	if(access(dbase.c_str(), F_OK) == 0)
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
		
		e_msg = "Can't open database: " + string(sqlite3_errmsg(db));
		except.set_msg(e_msg);
		throw(except);
	}
	
	/**
	 * Add phonebook information
	 * table if this is the first
	 * time creating this phonebook
	 **/

	 //build query
	 if(!dbexists)
	 {
	 	sql_query = "CREATE TABLE book_info "
	             "(Pages int, "
				 " Contacts int); "
				 "INSERT INTO book_info (Pages, Contacts) "
				 "VALUES(0, 0);";
	
		// execute the SQL query
		rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	
		if(rc != SQLITE_OK)
		{
			e_msg = "SQL error: " + string(err_msg);
			except.set_msg(e_msg);
			throw(except);
		}

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
					  const string& email) throw(CphonebookException)
{
	if(page < 1)
	{
		except.set_msg("Page number can't be zero");
		throw(except);
	}

	if(!dbase_opened)
	{
		except.set_msg("Can't add contact. Database was not opened");
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

	// convert table name from int to string
	table = std::to_string(page);
   
	// build query
	sql_query = "INSERT INTO '" + table + "' "
				+ "VALUES " + "(\"" + fname + "\",\"" + lname + "\",\"" +  addr + "\",\"" + 
				phone + "\",\"" + email + "\");";
    
	cout << "QUERY: " << sql_query << endl;
	// execute the SQL query
	rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + string(err_msg);
		sqlite3_free(err_msg);
		except.set_msg(e_msg);
		throw(except);
	}


}

void Cphonebook::append_page() throw(CphonebookException)
{
	string e_msg, sql_query, pagestr;
	char* err_msg = 0;
	book_info_t binfo;
	int page, rc;

	if(!dbase_opened)
	{
    	except.set_msg("Can't append page. Database was not opened");
		throw(except);
	}

	sql_query = "SELECT * FROM book_info;";

	rc = sqlite3_exec(db, sql_query.c_str(), callback_get_book_info, &binfo, &err_msg);
	if(rc != SQLITE_OK)
	{
	e_msg = "SQL error: " + string(err_msg);
	sqlite3_free(err_msg);
	except.set_msg(e_msg);
	throw(except);
	}
	
	/***
	 * Wait for the mutex to be unlocked
	 */
	 callback_busy.lock();
	 callback_busy.unlock();

	/***
	 *  We got the information 
	 *  by this point 
	 **/
	page = binfo.pages;
    page++;
	stringstream ss;
	ss << page;
	ss >> pagestr;
	/** 
	 * Now add the new page 
	 **/
    cout << "PAGE: " << pagestr << endl;
	sql_query = "CREATE TABLE '" + pagestr +
	            "'(FNAME varchar(255),"
		        " LNAME varchar(255),"
				" ADDR varchar(255),"
				" PHONE varchar(64),"
				" EMAIL varchar(255));";
	cout << "QUERY: " << sql_query << endl;
	rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + string(err_msg);
		except.set_msg(e_msg);
		sqlite3_free(err_msg);
		throw(except);
	}


}

void Cphonebook::create(const string& db_file, unsigned int npages) throw(CphonebookException)
{
	string pagestr, sql_query, e_msg;
	char* err_msg;
	int rc;

	if(access(db_file.c_str(), F_OK))
	{
		except.set_msg("A phonebook exists under the same name");
		throw(except);
	}
	
	if(npages < 1)
	{
		except.set_msg("Can't create zero pages");
		throw(except);
	}

	if(dbase_opened)
	{
		except.set_msg("A book is already opened. Close it");
		throw(except);
	}
   
    /***
	 * Create the book(database)
	 * 
	 * */

	rc = sqlite3_open(db_file.c_str(), &db);
	if(rc)
	{
		e_msg = "Can't create database: " + string(sqlite3_errmsg(db));
		except.set_msg(e_msg);
		throw(except);
	}

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
		e_msg = "SQL error: " + string(err_msg);
		except.set_msg(e_msg);
		throw(except);
	}
	

	for(int i = 1; i <= npages; i++)
	{
		stringstream(pagestr) << i;

		//build query
		sql_query = "CREATE TABLE " + pagestr +
				" (FNAME varchar(255),"
		        " LNAME varchar(255),"
				" ADDR varchar(255),"
				" PHONE varchar(64),"
				" EMAIL varchar(255));";
		
		rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
		if(rc != SQLITE_OK)
		{
			e_msg = "SQL error: " + string(err_msg);
			except.set_msg(e_msg);
			sqlite3_free(err_msg);
			throw(except);
		}

	}
}

void Cphonebook::get_contact_at(contact_t& dst,
								unsigned int page,
								unsigned int id) throw(CphonebookException)
{
	string e_msg, sql_query, idstr, pagestr;
	char* err_msg;
	int rc;

	if(!dbase_opened)
	{
		except.set_msg("Can't get contact, database not opened");
		throw(except);
	}

	if(page < 1)
	{
		except.set_msg("Page number can't be zero");
		throw(except);
	}

	if(id < 1)
	{
   		except.set_msg("Contact id can't be zero");
		throw(except);
	}

	// convert to string
	idstr = std::to_string(id);
	pagestr =  std::to_string(page);

	// build query
	sql_query = "SELECT oid, FNAME, LNAME, ADDR, PHONE, EMAIL FROM '" + pagestr +
		    "' WHERE oid=" +  idstr + ";";

	//execute query
	rc = sqlite3_exec(db, sql_query.c_str(), callback_get_contact_info, (void*) &dst, &err_msg);
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + string(err_msg);
		sqlite3_free(err_msg);
		except.set_msg(e_msg);
		throw(except);
	}

	/*** 
	 * Wait for the callback function to finish
	 **/
	 callback_busy.lock();
	 callback_busy.unlock();
}

void Cphonebook::set_contact_at(const contact_t& src,
					unsigned int page,
					unsigned int id) throw(CphonebookException)
{
	string e_msg, sql_query, pagestr, idstr;
	char* err_msg;
	int rc;

	if(page < 1)
	{
	except.set_msg("Page number can't be zero");
	throw(except);
	}

	if(id < 1)
	{
	except.set_msg("Contact id can't be zero");
	throw(except);
	}

	if(!dbase_opened)
	{
    except.set_msg("Can't set contact. Database was not opened");
	throw(except);
	}
	
	// convert to strings
	pagestr =  std::to_string(page);
	idstr = std::to_string(id);

	// build query
	sql_query = "UPDATE '" + pagestr + "'\n" +
				"SET FNAME=\"" + src.fname + "\","
				"LNAME=\"" + src.lname + "\","
				"ADDR=\"" + src.addr + "\","
				"PHONE=\"" + src.phone + "\","
				"EMAIL=\"" + src.email + "\"\n"
				"WHERE oid=" + idstr + ";";
	
	//execute query
	rc = sqlite3_exec(db, sql_query.c_str(), NULL, NULL, &err_msg);
	if(rc != SQLITE_OK)
	{
		e_msg = "SQL error: " + string(err_msg);
		sqlite3_free(err_msg);
		except.set_msg(e_msg);
		throw(except);
	}
}

void Cphonebook::delete_contact_at(unsigned int page,
					   unsigned int id) throw(CphonebookException)
{


}

void Cphonebook::delete_page(unsigned int page) throw(CphonebookException)
{

}

int Cphonebook::get_n_page() throw(CphonebookException)
{

}

int Cphonebook::get_n_contacts_at(unsigned int page) throw(CphonebookException)
{


}

int Cphonebook::get_n_contacts() throw (CphonebookException)
{

}

void Cphonebook::close() throw (CphonebookException)
{

	if(dbase_opened)
	{
	sqlite3_close(db);
	}

}

/***************************************
 *                                     *
 * Implementation of helper functions  *
 * *************************************/

int callback_get_book_info(void * bookinfo, int argc, char **argv, char **azColName)
{
	//lock mutex first
	callback_busy.lock();

	book_info_t* binfo = (book_info_t*) bookinfo;
	string colname;

	for(int i = 0; i < argc; i++)
	{
		colname = azColName[i];
		if(colname == "Pages")
		{
			binfo->pages = atoi(argv[i]);
		}

		if(colname == "contacts")
		{
			binfo->contacts = atoi(argv[i]);	
		}
	}
	
	callback_busy.unlock();
	return 0;
}

int callback_get_contact_info(void* contact, int argc, char** argv, char** azColName)
{
	// lock mutex first
	callback_busy.lock();

	string colname;
	contact_t* cntct = (contact_t*) contact;

	for(int i = 0; i < argc; i++)
	{
		colname = azColName[i];
		if(colname == "rowid")
		{
			cntct->id = atoi(argv[i]);
		}
		if(colname == "FNAME")
		{
			cntct->fname = argv[i];		
		}

		if(colname == "LNAME")
		{
			cntct->lname = argv[i];
		}

		if(colname == "ADDR")
		{
			cntct->addr = argv[i];
		}

		if(colname == "PHONE")
		{
			cntct->phone = argv[i];
		}

		if(colname == "EMAIL")
		{
			cntct->email = argv[i];
		}
	}
	
	callback_busy.unlock();

	return 0;
}
