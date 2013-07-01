#include "CphonebookException.h"

CphonebookException::CphonebookException(const char* message)
{
	message_ = message;
}

const char* CphonebookException::what() const throw()
{
	return message_.c_str();
}

void CphonebookException::setMessage(const string& message)
{
	message_ = message;
}


