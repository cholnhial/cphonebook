#ifndef CPHONEBOOKEXCEPTION_H_
#define CPHONEBOOKEXCEPTION_H_

#include <exception>
#include <string>


class CphonebookException : public exception
{
private:
	std::string string msg;
public:
	CphonebookException(const char* m = "Unknown");

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
