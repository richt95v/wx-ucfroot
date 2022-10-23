
/*************************************************************************************************************************** 
 
    Overview:	This file defines a general purpose interface to system level IO connections such as Rs232, TCP, files, etc
 
					1.	Provisions are made for implement IO trace monitors using the 'iStdIO' interface. 

			    Do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/
/*************************************************************************************************************************************
 

*/

#ifndef ISTREAMIO
#define ISTREAMIO

#include <iStdOut.hpp>
#include <ConnectionID.hpp>


class iStreamIO {
/*  this class defines a general purpose interface to a system level IO stream
    1.	tcp/udp
    2.	gpib/vxi
    3.	usb / serial
    4.	etc
    */

	public:

	/************************************************************************************************************************
		system / framework ops
	*/

	virtual bool Acquire()=0;
	virtual bool Release()=0;
	/* Lock or release the object for use by the current thread. */

    //virtual void AssignTraceIO (iStdOut *trace)=0;
	/*	Assigh an output for optional trace / debug level messages */

	virtual ConnectionID& GetConnectionID()=0;
	 /* Get the record that defines the connection parameters */


	/************************************************************************************************************************
		primitive ops
	*/

	virtual bool ValidateConnection()=0;
	/*  verify that a connection can be made
	    1.	ping
	    2.	open/close a serial port
	    3.	open/close a file
	    */

	virtual bool Open(bool non_blocking=true)=0;
	virtual bool IsOpen()=0;
	 /* Open, or query the open status of the IO connection */

	virtual bool Close()=0;
	/*	Close the connection */

	virtual unsigned Read(char *buffer,unsigned buffer_length)=0;
	virtual unsigned Read(unsigned char *buffer,unsigned buffer_length)=0;
	/*	Read data from the connnection
		1.	buffer  The buffer to hold the data read from the device
		2.	buffer_length The length in bytes of the read buffer.
		4.	return unsigned  number of bytes read. 0 on error.
		*/

	virtual unsigned Write(const char *msg, int msg_length=-1, unsigned wait_ms=0)=0;
	virtual unsigned Write(const unsigned char *msg, int msg_length=-1, unsigned wait_ms=0)=0;
	/*	Write string data to the connection
		1.	msg The string data to be written.
		2.	msg_length The number of bytes in the string. If 0, the length will be determined by a call to strlen.
	  	3.	return unsigned Number of bytes written. 0 on error.
		*/

	/************************************************************************************************************************
		error handling ops
	*/

	virtual bool InError()=0;
	virtual const char* GetErrorMsg()=0;
	/*	Query the connection to find if an error has occurred */

	virtual ~iStreamIO() {};
};

#endif
