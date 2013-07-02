#include "Cphonebook.h"

Cphonebook::Cphonebook()
{
    databaseFile_ = "";
    isDatabaseOpened_ = false;
}

Cphonebook::~Cphonebook()
{
    if (isDatabaseOpened_) {
            sqlite3_close(database_);
    }
}

void 
Cphonebook::waitForDatabase()
{
    sCallbackBusy.lock();
    sCallbackBusy.unlock();
}


void 
Cphonebook::executeSqlQuery(const string& sqlQuery,
                            SQLITE3_CALLBACK callback,
                            void* callbackData) throw(CphonebookException)
{
    char* errorMessage = 0;

    int returnCode = sqlite3_exec(database_,
                                  sqlQuery.c_str(),
                                  callback,
                                  callbackData,
                                  &errorMessage);
    if (returnCode != SQLITE_OK) {
        string exceptionMessage = "SQL error: " + string(errorMessage);
        sqlite3_free(errorMessage);
        exception_.setMessage(exceptionMessage);
        throw(exception_);
    }
}


void 
Cphonebook::createDatabase(const string& databaseFile) throw(CphonebookException)
{

    int returnCode = sqlite3_open(databaseFile.c_str(), &database_);

    if (returnCode) { 
        
        string exceptionMessage = "can't open database: " + 
        string(sqlite3_errmsg(database_));
        
        exception_.setMessage(exceptionMessage);
        throw(exception_);
    }

}

void 
Cphonebook::open(const string& databaseFile) throw(CphonebookException)
{  

    /**
     * Check if database file
     * already exists.
     * */
    bool databaseExists = false;
    if (access(databaseFile.c_str(), F_OK) == 0) {
        databaseExists = true;  
    }

    /***
     *  Open the database, it will be created
     *  if it doesn't exist
     *  */
     createDatabase(databaseFile);

    /***
     * Add phonebook information
     * table if this is the first
     * time creating this phonebook
     **/

     if (!databaseExists) {
        // build query
        string sqlQuery = "CREATE TABLE bookinfo "
                 "(Pages int, "
                 " Contacts int); "
                 "INSERT INTO bookinfo (Pages, Contacts) "
                 "VALUES(0, 0);";
    
        // execute the SQL query
        executeSqlQuery(sqlQuery);

    }

    /** 
     * Get bookinfo for quick aceess
     * */
    string sqlQuery = "SELECT * FROM bookinfo;";
    executeSqlQuery(sqlQuery, callbackGetBookInfo, (void*) &bookInfo_);
    waitForDatabase();

    /** 
     * No issues so far
     * */
    databaseFile_ = databaseFile;
    isDatabaseOpened_ = true;
}


void 
Cphonebook::updateBookInfo()
{
    // building query
    string sqlQuery = "UPDATE bookinfo "
               "SET Pages=" + std::to_string(bookInfo_.pages) +
               ",Contacts=" + std::to_string(bookInfo_.contacts) +
               ";";

     // execute sql query
    executeSqlQuery(sqlQuery);
}


void
Cphonebook::addContact(const unsigned int pageNo,
                const Contact& srcContact) throw(CphonebookException) 
{
    // These if's are looking for common errors
    if (pageNo < 1){
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }

    if (!isDatabaseOpened_){
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }
    
    /*******************************
     * Add contact to page(table)  *
     *                             *
     *******************************/

    char* errorMessage = 0;

    // convert table name from int to string
    string table = std::to_string(pageNo);
   
    // build query
    string sqlQuery = "INSERT INTO '" + table + "' "
                + "VALUES " + "(\"" + srcContact.firstName + "\",\"" +
                srcContact.lastName + "\",\"" +  srcContact.address + "\",\"" + 
                srcContact.phone + "\",\"" + srcContact.email + "\");";
    
    // execute the SQL query
    executeSqlQuery(sqlQuery);
    
    /***
     *  update bookinfo table
     ***/
    bookInfo_.contacts++; // incrementing 
    updateBookInfo();
}


void 
Cphonebook::appendPage() throw(CphonebookException)
{
    // Is the database opened?
    if (!isDatabaseOpened_){
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }
    
    /***
     * Wait for the mutex to be unlocked
     * by the callback function.
     */
    waitForDatabase();

    /***
     *  We got the information 
     *  by this point 
     **/

     /***
     * Get numbers of pages.
     * and increment it
     */
     bookInfo_.pages++;
    
    
    // convert int to string
    string pageNumberString = std::to_string(bookInfo_.pages);

    /** 
     * Now add the new page 
     **/
     
    // build sql query
    string sqlQuery = "CREATE TABLE '" + pageNumberString +
                "'(FNAME vareturnCodehar(255),"
                " LNAME vareturnCodehar(255),"
                " ADDR vareturnCodehar(255),"
                " PHONE vareturnCodehar(64),"
                " EMAIL vareturnCodehar(255));";
                
    // execute sql query
    executeSqlQuery(sqlQuery);

    // update bookinfo
    updateBookInfo();
}


void 
Cphonebook::create(const string& databaseFile,
             const unsigned int nPages) throw(CphonebookException)
{
    char* errorMessage;

    if (access(databaseFile_.c_str(), F_OK)){
        exception_.setMessage("A phonebook exists under the same name");
        throw(exception_);
    }
    
    if(nPages < 1)
    {
        exception_.setMessage("Can't create zero pages");
        throw(exception_);
    }

    if(isDatabaseOpened_)
    {
        exception_.setMessage("A book is already opened. Close it");
        throw(exception_);
    }
   
    /***
     * Create the book(database)
     * 
     * */
    createDatabase(databaseFile);


     //build query
     string sqlQuery = "CREATE TABLE bookinfo "
                 "(Pages int, "
                 " Contacts int); "
                 "INSERT INTO bookinfo (Pages, Contacts) "
                 "VALUES(0, 0);";
    
    // execute the SQL query
    executeSqlQuery(sqlQuery);  

    for (int i = 1; i <= nPages; i++) {
        string pageNumberString = std::to_string(i);

        //build query
        sqlQuery = "CREATE TABLE '" + pageNumberString +
                "' (FNAME vareturnCodehar(255),"
                " LNAME vareturnCodehar(255),"
                " ADDR vareturnCodehar(255),"
                " PHONE vareturnCodehar(64),"
                " EMAIL vareturnCodehar(255));";
        //execute query
        executeSqlQuery(sqlQuery);    
    }
    
    
    bookInfo_.contacts = 0;
    bookInfo_.pages = 0;

    databaseFile_ = databaseFile;
    isDatabaseOpened_ = true;
}


void 
Cphonebook::getContactAt(Contact& dstContact,
                       const unsigned int pageNo,
                       const unsigned int idNo) throw(CphonebookException)
{
    if (!isDatabaseOpened_) {
        exception_.setMessage("Can't get contact, database not opened");
        throw(exception_);
    }

    if (pageNo < 1) {
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }

    if (idNo < 1) {
        exception_.setMessage("Contact id can't be zero");
        throw(exception_);
    }

    // convert to string
    string idNumberString = std::to_string(idNo);
    string pageNumberString =  std::to_string(pageNo);

    // build query
    string sqlQuery = "SELECT "
                      "oid,"
                      "FNAME,"
                      "LNAME,"
                      "ADDR,"
                      "PHONE,"
                      "EMAIL"
                      " FROM '" + pageNumberString +
                      "' WHERE oid=" +  idNumberString + ";";

    //execute query
    executeSqlQuery(sqlQuery, callbackGetContact, (void*) &dstContact);

    /*** 
     * Wait for the callback function to finish
     **/
     waitForDatabase();
}


void 
Cphonebook::setContactAt (const Contact& srcContact,
                          const unsigned int pageNo,
                          const unsigned int idNo) throw(CphonebookException)
{
    char* errorMessage = 0;

    if (pageNo < 1) {
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }

    if (idNo < 1) {
        exception_.setMessage("Contact id can't be zero");
        throw(exception_);
    }

    if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }
    
    // convert to strings
    string pageNumberString =  std::to_string(pageNo);
    string idNumberString = std::to_string(idNo);

    // build query
    string sqlQuery = "UPDATE '" + pageNumberString + "'\n" +
                "SET FNAME=\"" + srcContact.firstName + "\","
                "LNAME=\"" + srcContact.lastName + "\","
                "ADDR=\"" + srcContact.address + "\","
                "PHONE=\"" + srcContact.phone + "\","
                "EMAIL=\"" + srcContact.email + "\"\n"
                "WHERE oid=" + idNumberString + ";";
    
    //execute query
    executeSqlQuery(sqlQuery);
}


void 
Cphonebook::deleteContactAt(const unsigned int pageNo,
                            const unsigned int idNo) throw(CphonebookException)
{   
    if (pageNo < 1) {
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }

    if (idNo < 1) {
        exception_.setMessage("Contact id number can't be zero");
        throw(exception_);
    } 
    
    if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }  

    // convert to strings
    string pageNumberString = std::to_string(pageNo);
    string idNumberString = std::to_string(idNo);

    string sqlQuery = "DELETE FROM '" + pageNumberString + "' " +
                      "WHERE oid=" + idNumberString + ";";
    
    executeSqlQuery(sqlQuery);

    /** Update bookinfo table **/
    // decrement the number of contacts
    bookInfo_.contacts--;
    updateBookInfo();

}


void 
Cphonebook::deletePage(const unsigned int pageNo) throw(CphonebookException)
{   

    if (pageNo < 1) {
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }

    if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }

    /*********************************
     *  Update the bookinfo table    *
     *********************************/
    bookInfo_.pages--;
    /*** 
     * remove the count of the number of contacts
     * that belonged to this page.
     */
    bookInfo_.contacts -= getNumberOfContactsAt(pageNo);
    updateBookInfo();

 // build the query to remove the page(table)
    string sqlQuery = "DROP TABLE '" + std::to_string(pageNo) + "';";

    // run the query
    executeSqlQuery(sqlQuery);

}


int 
Cphonebook::getNumberOfPages() throw(CphonebookException)
{
    if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }

    return bookInfo_.pages;
}


int 
Cphonebook::getNumberOfContactsAt(const unsigned int pageNo)
throw(CphonebookException)
{
    if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }
    
    if (pageNo < 1) {
        exception_.setMessage("Page number can't be zero");
        throw(exception_);
    }
    string sqlQuery = "SELECT COUNT() FROM '" +
                      std::to_string(pageNo)  + "';";
    
    // execute query
    int nContacts;
    executeSqlQuery(sqlQuery, callbackGenericGetNumber, (void*) &nContacts);
    waitForDatabase();

    return nContacts;

}


int 
Cphonebook::getNumberOfContacts() throw (CphonebookException)
{
 if (!isDatabaseOpened_) {
        exception_.setMessage("Database was not opened");
        throw(exception_);
    }

    return bookInfo_.contacts;

}


void Cphonebook::close() throw (CphonebookException)
{

    if(isDatabaseOpened_)
    {
    sqlite3_close(database_);
    }

}

/***************************************
 *                                     *
 * Implementation of helper functions  *
 * *************************************/

int callbackGetBookInfo(void* bookinfo,
                        int argc,
                        char **argv,
                        char **azColName)
{
    //lock mutex first
    sCallbackBusy.lock();

    BookInfo* bookInfo = (BookInfo*) bookinfo;

    string ColumnName;
    

    /***
     * This for loop gets the columns
     * we are interested in only.
     *  They are 'Pages' and 'Contacts'
     *  */
    for(int i = 0; i < argc; i++) {
        string ColumnName = azColName[i];

        if (ColumnName == "Pages") {
            bookInfo->pages = atoi(argv[i]);
        }

        if (ColumnName == "Contacts") {
            
            bookInfo->contacts = atoi(argv[i]); 
        }
    }
    
    // we are done, unlock the mutex
    sCallbackBusy.unlock();

    return 0;
}


int callbackGetContact(void* dstContact,
                       int argc,
                       char** argv,
                       char** azColName)
{
    // lock mutex first
    sCallbackBusy.lock();

    Contact* contact = (Contact*) dstContact;
     
    for (int i = 0; i < argc; i++) {
        string ColumnName = azColName[i];
        if (ColumnName == "rowid") {
            contact->id = atoi(argv[i]);
        }

        if (ColumnName == "FNAME") {
            contact->firstName = argv[i];       
        }

        if (ColumnName == "LNAME") {
            contact->lastName = argv[i];
        }

        if (ColumnName == "ADDR") {
            contact->address = argv[i];
        }

        if (ColumnName == "PHONE") {
            contact->phone = argv[i];
        }

        if (ColumnName == "EMAIL") {
            contact->email = argv[i];
        }
    }
    
    sCallbackBusy.unlock();

    return 0;
}


static int callbackGenericGetNumber(void* dst, int argc,
                                    char** argv,
                                    char** azColName)
{
    int* number = (int*) dst;

    for (int i = 0; i < argc; i++) {
        string columnName = azColName[i];

        if (columnName == "COUNT()") {
            *number = atoi(argv[i]);
        }
    }

    return 0;
}

