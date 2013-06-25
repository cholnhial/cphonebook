#include "CphonebookException.h"

CphonebookException::CphonebookException(const char* m = "Unknown");
{
	msg = m;
}

const char* CphonebookException::what()
{
	return msg.c_str();
}

void CphonebookException::set_msg(const stirng& m)
{
	msg = m;
}


