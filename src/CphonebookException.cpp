#include "CphonebookException.h"

CphonebookException::CphonebookException(const char* m)
{
	msg = m;
}

const char* CphonebookException::what() const throw()
{
	return msg.c_str();
}

void CphonebookException::set_msg(const string& m)
{
	msg = m;
}


