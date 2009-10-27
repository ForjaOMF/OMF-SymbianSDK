/*
 * ============================================================================
 *  Name     : CSMS20 of SMS20.h
 *  Part of  : symbian sdk
 *  Created  : 22/05/2008
 *  Version  : 1.0
 *  Copyright:
 * ============================================================================
 */

#ifndef __SMS20_H__
#define __SMS20_H__

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
*	Defines a type for the contacts in SMS2.0 API
*/
class CContactoSMS20 : public CBase
{
private:

	TBuf8<256> m_csUserID;
	TBuf8<256> m_csAlias;
	TBool m_bPresente;
	
public:

	/*
	* CContactoSMS20()
	*
	* Create a CContactoSMS20 object with defaults values.
	*
	* Params:
	*			-
	*
	* Returns:
	* 		the created instance of CContactoSMS20
	*
	*/
	CContactoSMS20();
	
	/*
	* CContactoSMS20()
	*
	* Create a CContactoSMS20 object with the passed values.
	*
	* Params:
	*			csUserID: phone numbre of the user
	*			csAlias: alias of the user
	*			bPresente: presente status
	*
	* Returns:
	* 		the created instance of CContactoSMS20
	*
	*/
	CContactoSMS20(const TDesC8& csUserID, const TDesC8& csAlias, TBool bPresente = false);
	
	/*
	* SetUserID()
	*
	* set the user ID of the CContactoSMS20
	*
	* Params:
	*			csUserID: phone numbre of the user
	*
	* Returns:
	* 		-
	*
	*/
	void SetUserID(const TDesC8& csUserID){m_csUserID = csUserID;};
	
	/*
	* SetAlias()
	*
	* set the alias of the CContactoSMS20
	*
	* Params:
	*			csAlias: alias of the user
	*
	* Returns:
	* 		-
	*
	*/
	void SetAlias(const TDesC8& csAlias){m_csAlias = csAlias;};
	
	/*
	* SetPresence()
	*
	* set the presence of the CContactoSMS20
	*
	* Params:
	*			bPresente: presente status
	*
	* Returns:
	* 		-
	*
	*/
	void SetPresence(TBool bPresente){m_bPresente = bPresente;};

	/*
	* GetUserID()
	*
	* get the UserID of the CContactoSMS20
	*
	* Params:
	*			-
	*
	* Returns:
	* 		the UserID of CContactoSMS20
	*
	*/
	TBuf8<256> GetUserID(){return m_csUserID;};
	
	/*
	* GetAlias()
	*
	* get the alias of the CContactoSMS20
	*
	* Params:
	*			-
	*
	* Returns:
	* 		the alias of CContactoSMS20
	*
	*/
	TBuf8<256> GetAlias(){return m_csAlias;};
	
	/*
	* GetPresence()
	*
	* get the presence of the CContactoSMS20
	*
	* Params:
	*			-
	*
	* Returns:
	* 		the presence of CContactoSMS20
	*
	*/
	TBool GetPresence(){return m_bPresente;};
	
};

/*
* Provides simple interface to HTTP Client API.
*/
class CSMS20 :	public CBase,
						public MHTTPTransactionCallback,
						public MHTTPDataSupplier,
						public MHTTPAuthenticationCallback
	{
public:
	/*
	* NewL()
	*
	* Create a CSMS20 object.
	*
	* Params:
	*			-
	*
	* Returns:
	* 		A pointer to the created instance of CSMS20
	*
	*/
	static CSMS20* NewL();

	/*
	* NewLC()
	*
	* Delete a CSMS20 object.
	*
	* Params:
	*		-
	*
	* Returns:
	* 		A pointer to the created instance of CSMS20
	*
	*/
	static CSMS20* NewLC();

	/*
	* ~CSMS20()
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
	~CSMS20();
	
	/*
	* Login()
	*
	* Login into SMS20 platform.
	*
	* Params:
	*		aLogin: login of the user in the SMS20 platform
	*		aPwd: password of the user
	*
	* Returns:
	* 		Session ID
	*
	*/	
	HBufC8* Login(const TDesC8& aLogin,
			 const TDesC8& aPwd);
			 
	/*
	* Connect()
	*
	* Activate session in SMS20 platform.
	*
	* Params:
	*			aAlias: your alias
	* Returns:
	* 		-
	*
	*/	
	RPointerArray<CContactoSMS20> Connect(const TDesC8& aAlias);
	
	/*
	* Polling()
	*
	* Wait for a response of the SMS 2.0 platform
	*
	* Params:
	*			-
	* Returns:
	* 		a string with the response
	*
	*/	
	HBufC8* Polling();

	/*
	* AddContact()
	*
	* Add a contact to the active user contact list
	*
	* Params:
	*			phone number of the contact to add
	* Returns:
	* 		-
	*
	*/	
	HBufC8* AddContact(const TDesC8& aContact);

	/*
	* AuthorizeContact()
	*
	* Autorice a contact to add you to its contact list
	*
	* Params:
	*			aContact: phone number of the contact to authorice
	* Returns:
	* 		-
	*
	*/	
	void AuthorizeContact(const TDesC8& aContact);

	/*
	* DeleteContact()
	*
	* Delete a contact form yours contact list
	*
	* Params:
	*			aContact: phone number of the contact to delete
	* Returns:
	* 		-
	*
	*/	
	void DeleteContact(const TDesC8& aContact);

	/*
	* SendMessage()
	*
	* Send a message to a contact
	*
	* Params:
	*			aDest: phone number of the contact to delete
	*			aMsg: message to send
	* Returns:
	* 		-
	*
	*/	
	void SendMessage(const TDesC8& aDest, const TDesC8& aMsg);
		
	/*
	* Logout()
	*
	* Logout from SMS20 platform.
	*
	* Params:
	*			-
	*
	* Returns:
	* 		-
	*
	*/	
	void Logout();
			 
	/*
	* CancelTransaction()
	*
	* Closes currently running transaction and frees resources related to it.
	*
	* Params:
	*			-
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
	*			-
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
	* Perform the second phase construction of a CSMS20 object.
	*
	* Params:
	*			-
	*
	* Returns:
	* 		-
	*
	*/
	void ConstructL();

	/*
	* CSMS20()
	*
	* Perform the first phase of two phase construction.
	*
	* Params:
	*			-
	*
	* Returns:
	* 		-
	*
	*/
	CSMS20();

	/*
	* SetHeaderL()
	*
	* Sets header value of an HTTP request.
	*
	* Params:
	*			aHeaders:	Headers of the HTTP request
	*			aHdrField:	Enumerated HTTP header field, e.g. HTTP::EUserAgent
	*			aHdrValue:	New value for header field
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
	*			-
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

// help funtions
private:

	/*
	* FindValue()
	*
	* find a value in a xml between the tags
	*
	* Params:
	*			csDatos:	sting to find.
	*			csTag:	tags to find.
	*
	* Returns:
	* 		the string between the tags
	*
	*/
	HBufC8*	FindValue(const TDesC8& csDatos, const TDesC8& csTag);
	
	/*
	* PostHTTP()
	*
	* send a post petition to a server
	*
	* Params:
	*			aBody:	sting to post.
	*
	* Returns:
	* 		-
	*
	*/
	void PostHTTP(const TDesC8& aBody);
	
	/*
	* GetAlias()
	*
	* get the alias from the response
	*
	* Params:
	*			csDatos: sting to find.
	*			csLogin: number for finding the alias
	*
	* Returns:
	* 		alias of the phone number
	*
	*/
	HBufC8* GetAlias(const TDesC8& csDatos, const TDesC8& csLogin);
	
	/*
	* GetContactList()
	*
	* parses a string to get the contact list
	*
	* Params:
	*			csContacts: sting to parse.
	*
	* Returns:
	* 		-
	*
	*/
	RPointerArray<CContactoSMS20> GetContactList();	

public:	
	/*
	* GetPresenceList()
	*
	* parses a string to actualice the presence in the contact list
	*
	* Params:
	*			csContacts: sting to parse.
	*
	* Returns:
	* 		-
	*
	*/
	RPointerArray<CContactoSMS20> GetPresenceList(const TDesC8& csContacts);

private:	
	/*
	* GetMsg()
	*
	* parses a string to get the message
	*
	* Params:
	*			csContacts: sting to parse.
	*
	* Returns:
	* 		string with the message
	*
	*/
	HBufC8* GetMsg(const TDesC8& csDatos);
	
	/*
	* GetUser()
	*
	* parses a string to get the contact
	*
	* Params:
	*			csContacts: sting to parse.
	*
	* Returns:
	* 		string with the contact
	*
	*/
	HBufC8* GetUser(const TDesC8& csDatos);
	
public:

	/*
	* GetSessionID()
	*
	* get the SessionID from the active session
	*
	* Params:
	*			-
	*
	* Returns:
	* 		string with the SessionID
	*
	*/
	TBuf8<256> GetSessionID(){return iSessionId->Des();};
	
	/*
	* GetTransID()
	*
	* get the number of the last transaction
	*
	* Params:
	*			-
	*
	* Returns:
	* 		string with the SessioID
	*
	*/
	TInt GetTransID(){return iTransId;};
	
	/*
	* GetAlias()
	*
	* get the alias of the active session
	*
	* Params:
	*			-
	*
	* Returns:
	* 		string with the alias
	*
	*/
	TBuf8<256> GetAlias(){return iAlias;};

private:
	// declare members
    RSocketServ 			iSocketServ;
    RConnection 			iConnection;

	RHTTPSession					iSession;
	RHTTPTransaction			iTransaction;
	CActiveSchedulerWait	iSchedulerWait;
	
	HBufC8*					iPostData;					// Data for HTTP POST
	TBool						iRunning;						// ETrue, if transaction running
	
	HBufC8* 				iSessionId;
	TInt						iTransId;
	TBuf8<256> 			iAlias;
	TBuf8<256> 			iTlf;
	
	HBufC8*					iResponse;
	
	TBool 					iConnectionSetupDone;
	};

#endif // __SMS20_H__