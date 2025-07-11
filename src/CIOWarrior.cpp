/**
 * \file CIOWarrior.cpp
 *
 * \brief implementation of the IOWarrior40 control class
 * \author Antje Wirth <antje.wirth@h-da.de>
 * \author Holger Frank <holger.frank@h-da.de>
 * \author H. Raza  <hasan.raza@stud.h-da.de>
 */
using namespace std;
#include <iostream>
#include <string>
#include <typeinfo>
#include <SKSLib.h>
#include <cwchar>
#include "CIOWarrior.h"

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * CIOWarrior class implementation
 * You find a detailed description for each method in the following comments.
 * Implement each method after its comment (at the position of the respective comment
 * starting with todo)
 */
/**
 * \brief initializes attributes
 */
// todo implement the constructor() here
CIOWarrior::CIOWarrior(){
	m_handle = NULL;						//Initializing the pointer
	m_reportIn = {0};						//Initializing data structure
	m_reportOut = {0};
	m_lastError = E_OK;
	m_state = S_NOTREADY;
}

/**
 * \brief closes the device
 * \transition
 * S_READY -> S_NOTREADY
 */
// todo implement destructor() here
CIOWarrior::~CIOWarrior(){
	close();
}


/**
 * \brief Opens IOWarrior device.
 *
 * The default value sets all outputs to high. In this state all port pins
 * can also be read as input
 *
 * \exception
 * - no device found
 * - wrong device id (IoW found, but no IoW40)
 *
 * \transition
 * S_NOTREADY -> S_READY
 *
 * \test
 * to test the validation of the IoW device type, connect an IoW24Kit
 * instead of IoW40Kit
 *
 * functional description
 * ======================
 * - skips all steps if already in READY state
 * - opens IoWDevice, sets last error and throws exception if no device found
 * - reads product id, closes device, sets last error and throws exception if not an IoWarrior40 detected
 * - sets all output port pins to logical 1
 * - sets state to READY
 * - sets last error to OK
 */
// todo implement open() here
void CIOWarrior::open(){
	if(m_state == S_READY){
		return;
	}
	m_handle = IowKitOpenDevice();					//Call the API Open Function
	if (m_handle == NULL){
		m_lastError = E_NODEVICE;
		throw CException(CException::SRC_IOWarrior, E_NODEVICE, "No Device found.");
	}
	//Call the API Get Product Id function
	if(IowKitGetProductId(m_handle) != IOWKIT_PRODUCT_ID_IOW40){
		IowKitCloseDevice(m_handle);
		m_handle=NULL;
		m_lastError = E_INVALIDDEVICE;
		throw CException(CException::SRC_IOWarrior, E_INVALIDDEVICE, "Invalid IOWarrior device (use IOWarrior40).");
	}
	m_reportOut.Value = 0xffffffff;					//Sets all output port pins to logical 1
	int res=IowKitWrite(m_handle, IOW_PIPE_IO_PINS, (char*) &m_reportOut, sizeof(m_reportOut));
	if(res != sizeof(m_reportOut)){
		close();
		m_lastError = E_WRITEERRORS;
		throw CException(CException::SRC_IOWarrior, E_WRITEERRORS, "Failed to write.");
	}
	m_state = S_READY;
	m_lastError = E_OK;
}



/**
 * \brief Closes IOWarrior device.
 *
 * the value of finalOutVal should be selected so, that the power
 * consumption of the device is minimized (e.g. all LEDs off). this depends
 * on the attached circuits. 0xffxxxxxx switches built-in LEDs on port 3 off
 * (they are low active)
 *
 * \transition
 * S_READY -> S_NOTREADY
 *
 * functional description
 * ======================
 * - skips if state is NOTREADY
 * - sets all output pins to logical 1
 * - closes IoW device
 * - sets state to NOTREADY
 */
// todo implement close() here
void CIOWarrior::close(){
	if(m_state == S_NOTREADY){
		return;
	}
	IowKitCloseDevice(m_handle);
	m_state = S_NOTREADY;
	m_handle = NULL;
}


/**
 * \brief Writes 8-bit pattern to IOWarriors LED port 3
 * \param data 8-bit pattern shown on LEDs (1-bit switches the LED on, 0-bit switches the LED off)
 *
 * \exception
 * - device not ready
 * - write error
 *
 * functional description
 * ======================
 * - throws an exception if NOTREADY
 * - switches all LEDs on whose bit is set in data
 */
// todo implement writeLEDs() here
void CIOWarrior::writeLEDs(uint8_t data){
	if(m_state == S_NOTREADY){
		m_lastError = E_DEVICENOTREADY;
		throw CException(CException::SRC_IOWarrior, E_DEVICENOTREADY, "Open the Device First");
	}
	m_reportOut.ReportID = 0;					//plain IO ID
	m_reportOut.Value = 0xffffffff;
	m_reportOut.Bytes[3] = ~data;				//4th byte represents the LEDs (Port3). Negation of the data to switch ON LEDs
	int res=IowKitWrite(m_handle, IOW_PIPE_IO_PINS, (char*) &m_reportOut, sizeof(m_reportOut)); //Writing Values of m_reportOut
	if(res != sizeof(m_reportOut)){
		close();
		m_lastError = E_WRITEERRORS;
		throw CException(CException::SRC_IOWarrior, E_WRITEERRORS, "Failed to write.");
	}
}

/**
 * \brief detects if the key was pressed
 *
 * looks if button on the IOWarrior board (connected to port 0 pin 0) is pressed and returns immediately
 * it is not necessary to wait for the user to release the button,
 * because as long as the state of the ports doesn't change, no new report will
 * be generated by the device
 *
 * \return
 * - true: key pressed
 * - false: key released or no change since last scan of the IoW chip
 *
 * \exception
 * - device not ready
 *
 *
 * functional description
 * ======================
 * - throws an exception if NOTREADY
 * - check if new report is available (any input pin has changed)
 * - returns true if the button has been pressed, otherwise returns false
 */
// todo implement keyPressed() here
bool CIOWarrior::keyPressed(){
	if(m_state == S_NOTREADY){
			m_lastError = E_DEVICENOTREADY;
			throw CException(CException::SRC_IOWarrior, E_DEVICENOTREADY, "Open the Device First");
	}
	if(sizeof(IOWKIT40_IO_REPORT) == IowKitReadNonBlocking(m_handle, IOW_PIPE_IO_PINS, (char *) &m_reportIn, sizeof(IOWKIT40_IO_REPORT))){
		cout<<"Port: "<< hex << m_reportIn.Value << endl;
		if((m_reportIn.Bytes[0] & 0x01) == 0){
			m_reportIn.Bytes[0] = 0xff;
			return true;
		}
	}

	return false;
}


/**
 * \brief Prints the current state of IOWarrior instance.
 *
 * functional description
 * ======================
 * prints m_lastError and m_state both as strings and numerical values on
 * the console
 */
// todo implement printState() here
void CIOWarrior::printState(){
	switch(m_state){
	case S_NOTREADY:
		cout<< "Device is not Ready" << endl; break;
	case S_READY:
		cout<< "Device is Ready" << endl; break;
	default:
		cout << "Unknown State" << endl; break;
	}
	switch(m_lastError){
	case E_OK:
		cout<< "No Error Occurred" << endl; break;
	case E_NODEVICE:
		cout<< "No Device Found" << endl; break;
	case E_INVALIDDEVICE:
		cout<< "Invalid Device Found" << endl; break;
	case E_WRITEERRORS:
		cout<< "Could not Write" << endl; break;
	case E_DEVICENOTREADY:
		cout<< "Device is not open" << endl; break;
	case E_NOEXTENSION:
		cout<< "No Extension Board Found" << endl; break;
	default:
		cout<< "Unknown Error" << endl; break;
	}
}
/**
 * \brief delivers current state of IOWarrior object
 * \return state as numerical value
 */
// todo implement getState() here

CIOWarrior::STATES CIOWarrior::getState()
{
	return m_state;
}


/**
 * \brief delivers current state of IOWarrior object as a string
 * \return state as string
 */
// todo implement getStateStr() here
string CIOWarrior::getStateStr(){
	switch(m_state){
	case S_NOTREADY:
		return "Device is not Ready"; break;
	case S_READY:
		return "Device is Ready"; break;
	default:
		return "Unknown State"; break;
	}
}
/**
 * \brief delivers last error of IOWarrior instance
 * \return error number
 */
// todo implement getLastError() here
CIOWarrior::ERRORS CIOWarrior::getLastError(){
	return m_lastError;
}

/**
 * \brief delivers last error of IOWarrior instance
 * \return last error text
 */
// todo implement getLastErrorStr() here
string CIOWarrior::getLastErrorStr(){
	switch(m_lastError){
		case E_OK:
			return "No Error Occurred"; break;
		case E_NODEVICE:
			return "No Device Found"; break;
		case E_INVALIDDEVICE:
			return "Invalid Device Found"; break;
		case E_WRITEERRORS:
			return "Could not Write"; break;
		case E_DEVICENOTREADY:
			return "Device is not open"; break;
		case E_NOEXTENSION:
			return "No Extension Board Found"; break;
		default:
			return "Unknown Error"; break;
		}
}

/**
 * \brief prints device info on console
 *
 * prints several information retrieved from a connected device
 *
 * \exception
 * - device not ready
 *
 *
 * functional description
 * ======================
 * - throws an exception if NOTREADY
 * - determines serial number, product revision and API version
 *   (see the documentation of the IOWarrior API to find the relevant API functions)
 * - prints these data on the console
 */
// todo implement printDeviceInfo() here
void CIOWarrior::printDeviceInfo(){
	if(m_state == S_NOTREADY){
	m_lastError = E_DEVICENOTREADY;
	throw CException(CException::SRC_IOWarrior, E_DEVICENOTREADY, "Open the Device First");
	}
	WCHAR buffer[9];
	IowKitGetSerialNumber(m_handle, buffer);
	cout<< "The Serial Number of The Device" << endl;
	cout << buffer << endl;
	cout << endl << "The Revision of the Firmware of the Device" << endl;
	cout << IowKitGetRevision(m_handle) << endl;
	cout << "The Version of the IOW Device" << endl;
	printf("%s\n", IowKitVersion());
}

/**
 * writes all 4 bytes of m_reportOut on the IoW40 ports
 *
 * \exception
 * - write error
 *
 *
 * functional description
 * ======================
 * - writes m_reportOut to all output pins
 * - checks if write was successful
 * - stores error no., closes IoW and throws exception if not
 */
// todo implement writeReportOut() here
void CIOWarrior::writeReportOut(void){
	int res=IowKitWrite(m_handle, IOW_PIPE_IO_PINS, (char*) &m_reportOut, sizeof(m_reportOut));
	if(res != sizeof(m_reportOut)){
		m_lastError = E_WRITEERRORS;
		close();
		throw CException(CException::SRC_IOWarrior, E_WRITEERRORS, "Failed to write.");
	}
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Extension board class implementation
 * DO NOT CHANGE THIS CODE!
 */
#ifdef CIOW_COMPLETE
CIOWarriorExt::CIOWarriorExt(void) :
		CIOWarrior() {
}

CIOWarriorExt::~CIOWarriorExt() {

}

void CIOWarriorExt::detect(void) {
	// check the state
	if (m_state != S_READY) {
		m_lastError = E_DEVICENOTREADY;
		throw(CException(this, typeid(this).name(), __FUNCTION__, m_lastError,
				getLastErrorStr()));
	}

	ULONG ret = IowKitReadNonBlocking(m_handle, IOW_PIPE_IO_PINS,
			(char*) &m_reportIn, sizeof(IOWKIT40_IO_REPORT));

	// set PORT0.1 LOW
	m_reportOut.Bytes[0] &= ~0x2;
	writeReportOut();

	/*
	 * the status of the ports is read typ. every 8ms (see Datasheet pg. 1)
	 * => wait for 1.5 interval time (i.e. 12ms) for reliable detection of changing state of
	 * Port0.1 (output) on Port0.0 (input)
	 */
	Sleep(12);

	// check if PORT0.0 is LOW
	ret = IowKitReadNonBlocking(m_handle, IOW_PIPE_IO_PINS, (char*) &m_reportIn,
			sizeof(IOWKIT40_IO_REPORT));
	if (sizeof(IOWKIT40_IO_REPORT) == ret) {
		/*
		 * buttons are low active => invert value of port 0 and mask SW1
		 *
		 * extension is available if value is 0
		 */
		if (!((~m_reportIn.Bytes[0]) & BTN_1)) {
			m_lastError = E_NOEXTENSION;
			throw(CException(this, typeid(this).name(), __FUNCTION__,
					m_lastError, "no extension found"));
		}
	} else {
		throw(CException(this, typeid(this).name(), __FUNCTION__, m_lastError,
				getLastErrorStr()));
	}
	// set PORT0.1 HIGH
	m_reportOut.Bytes[0] |= 0x2;
	writeReportOut();
	Sleep(12);
}

void CIOWarriorExt::open(void) {
	CIOWarrior::open();
	// throws exception if no extension detected
	detect();
}

void CIOWarriorExt::printDeviceInfo(void) {
	CIOWarrior::printDeviceInfo();
	cout << "EIT extension: available" << endl;
}

uint8_t CIOWarriorExt::readButtons(uint8_t mask) {
	if (m_state != S_READY) {
		m_lastError = E_DEVICENOTREADY;
		throw(CException(this, typeid(this).name(), __FUNCTION__, m_lastError,
				getLastErrorStr()));
	}

	if (sizeof(IOWKIT40_IO_REPORT)
			== IowKitReadNonBlocking(m_handle, IOW_PIPE_IO_PINS,
					(char*) &m_reportIn, sizeof(IOWKIT40_IO_REPORT))) {
		/*
		 * SW 1 and the button on the base board are connected both to
		 * Port0.0 AND 0.1 => a shift right for one digit moves all
		 */
		return (((~m_reportIn.Bytes[0]) >> 1) & mask);
	}
	/*
	 * IowKitReadNonBlocking returns the size of the chunk (5 for IoW40) only
	 * if a pin state has changed since the last query. Otherwise it returns 0
	 *
	 * => no exception at this point
	 */

	// no button state changed => returning 0x0 (all keys released)
	return 0x0;
}

void CIOWarriorExt::writeLEDs(uint16_t value) {
	// check the state
	if (m_state != S_READY) {
		m_lastError = E_DEVICENOTREADY;
		throw(CException(this, typeid(this).name(), __FUNCTION__, m_lastError,
				getLastErrorStr()));
	}
	/*
	 * set all LEDs on
	 * set all LEDs off whose bit in inverted value is set (LEDs are low active)
	 */
	m_reportOut.Value &= 0xff0000ff;
	m_reportOut.Value |= ((~(uint32_t)value) << 8);
	writeReportOut();
}
#endif
