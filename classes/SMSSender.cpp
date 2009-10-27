/*
 * ============================================================================
 *  Name     : CSMSSender of SMSSender.cpp
 *  Part of  : symbian sdk
 *  Created  : 05/06/2008
 *  Version  : 1.0
 *  Copyright:
 * ============================================================================
 */

#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <uri8.h>
#include <http.h>
#include <chttpformencoder.h>
#include <HttpStringConstants.h>
#include <http\RHTTPTransaction.h>
#include <http\RHTTPSession.h>
#include <http\RHTTPHeaders.h>
#include <SMSSenderExample.rsg>

#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <CommDbConnPref.h>

#include "SMSSender.h"

// Used user agent for requests
_LIT8(KUserAgent, "SimpleClient 1.0");

// headers
_LIT8(KAccept, "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*");
_LIT8(KContentType, "application/x-www-form-urlencoded");
_LIT8(KHost, "opensms.movistar.es");
_LIT8(KConnection, "Keep-Alive");
_LIT8(KEncoding, "gzip, deflate");

// url
_LIT8(KUrl, "https://opensms.movistar.es/aplicacionpost/loginEnvio.jsp");

_LIT8(KNull, "");

const TInt KStatustextBufferSize = 32;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;
const TInt KDefaultBufferSize = 256;

// ----------------------------------------------------------------------------
// CSMSSender::NewL()
//
// Creates instance of CSMSSender.
// ----------------------------------------------------------------------------
CSMSSender* CSMSSender::NewL()
	{
    CSMSSender* self = CSMSSender::NewLC();
    CleanupStack::Pop(self);
    return self;
	}


// ----------------------------------------------------------------------------
// CSMSSender::NewLC()
//
// Creates instance of CSMSSender.
// ----------------------------------------------------------------------------
CSMSSender* CSMSSender::NewLC()
	{
    CSMSSender* self = new (ELeave) CSMSSender();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


// ----------------------------------------------------------------------------
// CSMSSender::CSMSSender()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CSMSSender::CSMSSender()
:	iPostData(NULL),
	iRunning(EFalse)
	{
	}


// ----------------------------------------------------------------------------
// CSMSSender::~CSMSSender()
//
// Destructor.
// ----------------------------------------------------------------------------
CSMSSender::~CSMSSender()
	{	
	iSession.Close();

    // and finally close handles
    iConnection.Close();
    iSocketServ.Close();

	delete iPostData;
	}


// ----------------------------------------------------------------------------
// CSMSSender::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CSMSSender::ConstructL()
	{

	// Open RHTTPSession with default protocol ("HTTP/TCP")
	TRAPD(err, iSession.OpenL());
	if(err != KErrNone) 
		{
		// Most common error; no access point configured, and session creation
		// leaves with KErrNotFound.
		_LIT(KErrMsg,
			"Cannot create session. Is internet access point configured?");
		_LIT(KExitingApp, "Exiting app.");
		CEikonEnv::Static()->InfoWinL(KErrMsg, KExitingApp);
		User::Leave(err);
		}

	// Install this class as the callback for authentication requests. When
	// page requires authentication the framework calls GetCredentialsL to get
	// user name and password.
	InstallAuthenticationL(iSession);
	iResponse = (KNull);

	}


// ----------------------------------------------------------------------------
// CSMSSender::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CSMSSender::SetHeaderL(RHTTPHeaders aHeaders,
							 TInt aHdrField,
							 const TDesC8& aHdrValue)
	{
	RStringF valStr = iSession.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSession.StringPool().StringF(aHdrField,
		RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(); // valStr
	}


// ----------------------------------------------------------------------------
// CSMSSender::SendMessage()
//
// Starts a new SMS send transaction.
// ----------------------------------------------------------------------------
TBuf8<1024> CSMSSender::SendMessage(const TDesC8& aLogin,
								 const TDesC8& aPwd,
								 const TDesC8& aDest,
								 const TDesC8& aMsg)
	{
	SetupConnectionL();
	
	// mount the body	
	TBuf8<KDefaultBufferSize> text;	
	
	// Login
	text=(_L8( "TM_ACTION=AUTHENTICATE&TM_LOGIN="));
	text+= (aLogin);
	
	// password
	text+=(_L8("&TM_PASSWORD="));
	text+= (aPwd);
	
	// to
	text+=(_L8("&to="));
	text+= (aDest);
	
	// messsage
	text+=(_L8("&message="));
	text+= (aMsg);
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(KUrl);
	
	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EPOST,
		RHTTPSession::GetTable());
	
	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// Set headers for request
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EContentType, KContentType);
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EConnection, KConnection);
	
	// Copy data to be posted into member variable; iPostData is used later in
	// methods inherited from MHTTPDataSupplier.
	delete iPostData;
	iPostData = text.AllocL();

	// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
	// are called when framework needs to send body data.
	MHTTPDataSupplier* dataSupplier = this;
	iTransaction.Request().SetBody(*dataSupplier);

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iRunning = ETrue;
	iResponse.Copy(KNull);
		
	iSchedulerWait.Start();
	
	return iResponse;
	}


// ----------------------------------------------------------------------------
// CSMSSender::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CSMSSender::CancelTransaction()
	{
	if(!iRunning)
		return;

	// Close() also cancels transaction (Cancel() can also be used but
	// resources allocated by transaction must be still freed with Close())
	iTransaction.Close();
	
	// Not running anymore
	iRunning = EFalse;
	iSchedulerWait.AsyncStop();
	}


// ----------------------------------------------------------------------------
// CSMSSender::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CSMSSender::MHFRunL(RHTTPTransaction aTransaction,
						  const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseBodyData:
			{
			// Part (or all) of response's body data received. Use
			// aTransaction.Response().Body()->GetNextDataPart() to get the actual
			// body data.
	
			// Get the body data supplier
			MHTTPDataSupplier* body = aTransaction.Response().Body();
			TPtrC8 dataChunk;

			// GetNextDataPart() returns ETrue, if the received part is the last
			// one.
			body->GetNextDataPart(dataChunk);
			iResponse.Append(dataChunk);

			// Always remember to release the body data.
			body->ReleaseData();
			}
			break;

		case THTTPEvent::ESucceeded:
			{
			// Transaction can be closed now. It's not needed anymore.
			aTransaction.Close();
			iRunning = EFalse;			
			iSchedulerWait.AsyncStop();
			}
			break;

		case THTTPEvent::EFailed:
			{
			// Transaction completed with failure.
			aTransaction.Close();
			iRunning = EFalse;
			iSchedulerWait.AsyncStop();
			}
			break;

		default:
			// There are more events in THTTPEvent, but they are not usually
			// needed. However, event status smaller than zero should be handled
			// correctly since it's error.
			{
			TBuf<KInfotextBufferSize> text;
			if (aEvent.iStatus < 0)
				{				
				_LIT(KErrorStr, "Error: %d");
				text.Format(KErrorStr, aEvent.iStatus);
				// Just close the transaction on errors
				aTransaction.Close();
				iRunning = EFalse;				
				iSchedulerWait.AsyncStop();
				}
			}
			break;
		}
	}


// ----------------------------------------------------------------------------
// CSMSSender::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CSMSSender::MHFRunError(TInt aError,
							  RHTTPTransaction /*aTransaction*/,
							  const THTTPEvent& /*aEvent*/)
	{
	// Just notify about the error and return KErrNone.
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CSMSSender::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CSMSSender::GetNextDataPart(TPtrC8& aDataPart)
	{
	if(iPostData)
		{
		// Provide pointer to next chunk of data (return ETrue, if last chunk)
		// Usually only one chunk is needed, but sending big file could require
		// loading the file in small parts.
		aDataPart.Set(iPostData->Des());
		}
	return ETrue;
	}


// ----------------------------------------------------------------------------
// CSMSSender::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers)
// ----------------------------------------------------------------------------
void CSMSSender::ReleaseData()
	{
	// It's safe to delete iPostData now.
	delete iPostData;
	iPostData = NULL;
	}

// ----------------------------------------------------------------------------
// CSMSSender::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data
// supplier that it should return to the first part of the data.
// In practise an error has occured while sending data, and framework needs to
// resend data.
// ----------------------------------------------------------------------------
TInt CSMSSender::Reset()
	{
	// Nothing needed since iPostData still exists and contains all the data.
	// (If a file is used and read in small parts we should seek to beginning
	// of file and provide the first chunk again in GetNextDataPart() )
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CSMSSender::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound (it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content
// length in header).
// ----------------------------------------------------------------------------
TInt CSMSSender::OverallDataSize()
	{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
	}

// ----------------------------------------------------------------------------
// CSMSSender::GetCredentialsL()
//
// Inherited from MHTTPAuthenticationCallback
// Called by framework when we requested authenticated page and framework
// needs to know username and password.
// ----------------------------------------------------------------------------
TBool CSMSSender::GetCredentialsL(const TUriC8& aURI,
								   RString aRealm,
								   RStringF aAuthenticationType,
								   RString& aUsername,
								   RString& aPassword)
	{
	// aURI, aReal and aAuthenticationType are informational only. We only need
	// to set aUsername and aPassword and return ETrue, if aUsername and
	// aPassword are provided by user.

	// Informational only
	TBuf<KURIBufferSize> authType;
	TBuf<KURIBufferSize> uri;
	TBuf<KDefaultBufferSize> authText;
	authType.Copy(aAuthenticationType.DesC());
	uri.Copy(aURI.UriDes());
	_LIT(KAuthRequired, "%S requires %S authentication.");
	authText.Format(KAuthRequired, &uri, &authType);
	_LIT(KAuthNote, "Authentication required.");
	CEikonEnv::Static()->InfoWinL(KAuthNote, authText);

	// Query user name and password
	TBuf<KDefaultBufferSize> userName;
	TBuf<KDefaultBufferSize> password;

	// Set aUsername and aPassword
	TBuf8<KDefaultBufferSize> temp;
	temp.Copy(userName);
	TRAPD(err, aUsername = aRealm.Pool().OpenStringL(temp));
	if (!err)
		{
		temp.Copy(password);
		TRAP(err, aPassword = aRealm.Pool().OpenStringL(temp));
		if (!err) return ETrue;
		}

	// Return ETrue if user has given credentials (username and password),
	// otherwise EFlase
	return EFalse;
	}
	
// ----------------------------------------------------------------------------
// CSMSSender::SetupConnectionL()
//
// The method set the internet access point and connection setups.
// ----------------------------------------------------------------------------	
void CSMSSender::SetupConnectionL()
    {
    if( iConnectionSetupDone )
        return;
    
    iConnectionSetupDone = ETrue;
    
    //open socket server and start the connection
    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iConnection.Open(iSocketServ));
    
    // open the IAP communications database 
	CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(commDB);

	// initialize a view 
	CCommsDbConnectionPrefTableView* commDBView = 
	commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);

	// go to the first record 
	User::LeaveIfError(commDBView->GotoFirstRecord());

	// Declare a prefTableView Object.
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;

	// read the connection preferences 
	commDBView->ReadConnectionPreferenceL(pref);
	TUint32 iapID = pref.iBearer.iIapId; 

	// pop and destroy the IAP View 
	CleanupStack::PopAndDestroy(commDBView);

	// pop and destroy the database object
	CleanupStack::PopAndDestroy(commDB);

	// Now we have the iap Id. Use it to connect for the connection.
	// Create a connection preference variable.
	TCommDbConnPref connectPref;

	// setup preferences 
	connectPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connectPref.SetDirection(ECommDbConnectionDirectionUnknown);
	connectPref.SetBearerSet(ECommDbBearerGPRS);
	//Sets the CommDb ID of the IAP to use for this connection
	connectPref.SetIapId(iapID);
    
    User::LeaveIfError(iConnection.Start(connectPref));
	
    //set the sessions connection info
    RStringPool strPool = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    
    //to use our socket server and connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,
        RHTTPSession::GetTable() ), THTTPHdrVal (iSocketServ.Handle()) );

    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable() ), 
        THTTPHdrVal (REINTERPRET_CAST(TInt, &(iConnection))) );
    }

// end of file