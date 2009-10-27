/*
 * ============================================================================
 *  Name     : CSMSSender of SMSSender.h
 *  Part of  : symbian sdk
 *  Created  : 05/06/2008
 *  Version  : 1.0
 *  Copyright:
 * ============================================================================
 */

#ifndef __SMSSENDER_H__
#define __SMSSENDER_H__

#include <coecntrl.h>
#include <http\mhttpdatasupplier.h>
#include <http\mhttptransactioncallback.h>
#include <http\mhttpauthenticationcallback.h>
#include <es_sock.h>

/*
* Forward declarations
*/
class RHTTPSession;
class RHTTPTransaction;

/*
* Provides simple interface to HTTP Client API.
*/
class CSMSSender :	public CBase,
						public MHTTPTransactionCallback,
						public MHTTPDataSupplier,
						public MHTTPAuthenticationCallback
	{
public:
	/*
	* NewL()
	*
	* Create a CSMSSender object.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		A pointer to the created instance of CSMSSender
	*
	*/
	static CSMSSender* NewL();

	/*
	* NewLC()
	*
	* Create a CSMSSender object.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		A pointer to the created instance of CSMSSender
	*
	*/
	static CSMSSender* NewLC();

	/*
	* ~CSMSSender()
	*
	* Destroy the object
	*
	* Params:
	*		-
	*
	* Returns:
	* 		-
	*
	*/
	~CSMSSender();
	
	/*
	* SendMessage()
	*
	* Starts a new SMS send transaction.
	*
	* Params:
	*		aLogin: login of the user in the sms server
	*		aPwd: password of the user
	*		aDest: reciver movile number
	*		aMsg: message to send
	*
	* Returns:
	* 	Response from the SMS server
	*
	*/	
	TBuf8<1024> SendMessage(const TDesC8& aLogin,
			 const TDesC8& aPwd,
			 const TDesC8& aDest,
			 const TDesC8& aMsg);

	/*
	* CancelTransaction()
	*
	* Closes currently running transaction and frees resources related to it.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		-
	*
	*/
	void CancelTransaction();

	/*
	* IsRunning()
	*
	* Checks if the transaction is running.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		ETrue, if transaction is currently running.
	*
	*/
	inline TBool IsRunning() { return iRunning; };

private:
	/*
	* ConstructL()
	*
	* Perform the second phase construction of a CSMSSender object.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		-
	*
	*/
	void ConstructL();

	/*
	* CSMSSender()
	*
	* Perform the first phase of two phase construction.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		-
	*
	*/
	CSMSSender();

	/*
	* SetHeaderL()
	*
	* Sets header value of an HTTP request.
	*
	* Params:
	*		aHeaders:	Headers of the HTTP request
	*		aHdrField:	Enumerated HTTP header field, e.g. HTTP::EUserAgent
	*		aHdrValue:	New value for header field
	*
	* Returns:
	* 		-
	*
	*/
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,
					const TDesC8& aHdrValue);

/*
* From MHTTPSessionEventCallback
*/
private:
	/*
	* MHFRunL()
	*
	* Called by framework to notify about transaction events.
	*
	* Params:
	*		aTransaction: Transaction, where the event occured.
	*		aEvent: Occured event.
	*
	* Returns:
	* 		-
	*
	*/
	void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

	/*
	* MHFRunError()
	*
	* Called by framework when *leave* occurs in handling of transaction event.
	*
	* Params:
	*		aError:			The leave code that occured.
	*		aTransaction:	The transaction that was being processed when leave
	*						occured.
	*		aEvent:			The event that was being processed when leave
	*						occured.
	*
	* Returns:
	* 		KErrNone, if the error was handled. Otherwise the value of aError, or
	*		some other error value. Returning error value causes causes
	*		HTTP-CORE 6 panic.
	*
	*/
	TInt MHFRunError(	TInt aError,
						RHTTPTransaction aTransaction,
						const THTTPEvent& aEvent);

/*
* From MHTTPDataSupplier (needed for HTTP POST)
*/
private:
	/*
	* ReleaseData()
	*
	* Called by framework to allow data supplier to release resources
	* related to previous data part.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		-
	*
	*/
	void ReleaseData();

	/*
	* GetNextDataPart()
	*
	* Called when next data part is needed.
	*
	* Params:
	*		aDataPart: Must be set to point to the next data part.
	*
	* Returns:
	* 		ETrue if the provided data part is the last one. If more data parts
	*		are needed after the provided one, return EFalse.
	*
	*/
	TBool GetNextDataPart(TPtrC8& aDataPart);

	/*
	* Reset()
	*
	* Called by framework to reset the data supplier to its initial state.
	*
	* Params:
	*		-
	*
	* Returns:
	*		KErrNone if successfull.
	*
	*/
	TInt Reset();

	/*
	* OverallDataSize()
	*
	* Called by framework when it needs to know the size of the
	* body data.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		Size of the data, or KErrNotFound (or KErrNotSupported)
	*		if the size of the data is not known.
	*
	*/
	TInt OverallDataSize();
	
	/*
	 * SetupConnectionL()
	 * 
	 * The method set the internet access point and connection setups.
	 * 
	 */
	void SetupConnectionL();
	
/*
* From MHTTPAuthenticationCallback (needed for HTTP authentication)
*/
private:
	/*
	* GetCredentialsL()
	*
	* Called by framework when username and password for requested URI is
	* needed.
	*
	* Params:
	*		aURI: The URI being requested (e.g. "http://host.org")
	*		aRealm: The realm being requested (e.g. "user@host.org")
	*		aAuthenticationType: Authentication type. (e.g. "Basic")
	*		aUsername: Given user name.
	*		aPassword: Given password.
	*
	* Returns:
	* 		A pointer to the created document
	*
	*/
	TBool GetCredentialsL(	const TUriC8& aURI,
							RString aRealm,
							RStringF aAuthenticationType,
							RString& aUsername,
							RString& aPassword);

private:
	// declare members
    RSocketServ		iSocketServ;
    RConnection		iConnection;

	RHTTPSession					iSession;
	RHTTPTransaction			iTransaction;
	CActiveSchedulerWait	iSchedulerWait;

	HBufC8*					iPostData;	// Data for HTTP POST
	TBool						iRunning;	// ETrue, if transaction running	
	TBuf8<1024> 		iResponse;
	
	TBool 					iConnectionSetupDone;
	};

#endif // __SMSSENDER_H__