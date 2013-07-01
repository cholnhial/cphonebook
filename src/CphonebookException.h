#ifndef CPHONEBOOKEXCEPTION_H_
#define CPHONEBOOKEXCEPTION_H_

#include <exception>
#include <string>

using std::exception;
using std::string;

class CphonebookException : public exception
{
private:
	 string message_;
public:
   	CphonebookException() { message_ = "Unknown"; }
	CphonebookException(const char* message);
	virtual ~CphonebookException() throw() { } 
	/**
	 *  Returns the error message
	 *  */
	virtual const char* what() const throw();
 	
	/** 
	 * Sets the error message.
	 * */
	virtual void setMessage(const string& message);
};

#endif
