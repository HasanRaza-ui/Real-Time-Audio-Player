/**
 * \file CConsoleIO.h
 * \brief interface of CConsoleIO
 *
 * \date Jan 12, 2023
 * \author A. Wirth <antje.wirth@h-da.de>
 * \author H. Frank <holger.frank@h-da.de>
 * \author H. Raza  <hasan.raza@stud.h-da.de>
 */
#ifndef CCONSOLEIO_H_
#define CCONSOLEIO_H_

#include "CException.h"
#include "CConsoleThread.h"

class CConsoleIO {
private:
	CConsoleThread* m_thread;

public:
	CConsoleIO();
	virtual ~CConsoleIO();
	void writeConsole(const string text);
	double readConsoleNumber();
	string readConsoleString();
};

#endif /* CCONSOLEIO_H_ */
