#ifndef CPHONEBOOKEXCEPTION_H_
#define CPHONEBOOKEXCEPTION_H_

#include <exception>
#include <string>

using std::exception;
using std::string;

class CphonebookException : public exception
{
private:
	 string msg;
public:
   	CphonebookException() { msg = "Unknown"; }
	CphonebookException(const char* m);
	virtual ~CphonebookException() throw() { } 
	/**
	 *  Returns the error message
	 *  */
	virtual const char* what() const throw();
 	
	/** 
	 * Sets the error message.
	 * */
	virtual void set_msg(const string& m);
};

#endif
