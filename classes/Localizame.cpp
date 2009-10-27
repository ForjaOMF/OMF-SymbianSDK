/*
 * ============================================================================
 *  Name     : CLocalizame of Localizame.cpp
 *  Part of  : symbian sdk
 *  Created  : 22/05/2008
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
#include <LocalizameExample.rsg>

#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <CommDbConnPref.h>

#include "Localizame.h"

// Used user agent for requests
_LIT8(KUserAgent, "SimpleClient 1.0");

// headers
_LIT8(KAccept, "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*");
_LIT8(KContentType, "application/x-www-form-urlencoded");
_LIT8(KHost, "www.localizame.movistar.es");
_LIT8(KConnection, "Keep-Alive");
_LIT8(KEncoding, "identity");

// location const
_LIT8(KLocStr, "kilometros.");
_LIT8(KLocStr2, "\"");

// petitions
_LIT(KNull, "");
_LIT8(KNull8, "");
_LIT(KPetLogin, "Login");
_LIT(KPetNuevoUsuario, "NuevoUsuario");
_LIT(KPetLocaliza, "Localiza");
_LIT(KPetAutoriza, "Autoriza");
_LIT(KPetDesautoriza, "Desautoriza");
_LIT(KPetLogout, "Logout");

_LIT8(KPetRestridted, "Restringido");

// buffer sizes
const TInt KStatustextBufferSize = 32;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;
const TInt KDefaultBufferSize = 256;

// ----------------------------------------------------------------------------
// CLocalizame::NewL()
//
// Creates instance of CLocalizame.
// ----------------------------------------------------------------------------
CLocalizame* CLocalizame::NewL()
	{
    CLocalizame* self = CLocalizame::NewLC();
    CleanupStack::Pop(self);
    return self;
	}


// ----------------------------------------------------------------------------
// CLocalizame::NewLC()
//
// Creates instance of CLocalizame.
// ----------------------------------------------------------------------------
CLocalizame* CLocalizame::NewLC()
	{
    CLocalizame* self = new (ELeave) CLocalizame();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


// ----------------------------------------------------------------------------
// CLocalizame::CLocalizame()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CLocalizame::CLocalizame()
:	iPostData(NULL),
	iRunning(EFalse)
	{
		aCookie = (KNull);
		iPetition = (KNull);
    iPetitionSuccessful = EFalse;	
    iTlf = (KNull8);
	}


// ----------------------------------------------------------------------------
// CLocalizame::~CLocalizame()
//
// Destructor.
// ----------------------------------------------------------------------------
CLocalizame::~CLocalizame()
	{	
	iSession.Close();

    // and finally close handles
    iConnection.Close();
    iSocketServ.Close();

	delete iPostData;
	}


// ----------------------------------------------------------------------------
// CLocalizame::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CLocalizame::ConstructL()
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
	iResponse = (KNull8);
	iLocating = EFalse;
	}


// ----------------------------------------------------------------------------
// CLocalizame::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CLocalizame::SetHeaderL(RHTTPHeaders aHeaders,
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
// CLocalizame::SetCookie()
//
// Set Cookie header value of an HTTP request.
// ----------------------------------------------------------------------------	
void CLocalizame::SetCookie(RHTTPHeaders aHeaders)
	{
		_LIT8(KCookie, "JSESSIONID");
		
		TBuf8<KDefaultBufferSize> text3;
		text3.Copy(aCookie);
		
		RStringPool string_pool = iSession.StringPool();
	
		RStringF cookie_rsf = string_pool.StringF(HTTP::ECookie,RHTTPSession::GetTable());
		THTTPHdrVal field_value;
		field_value.SetStrF (cookie_rsf);
		aHeaders.SetFieldL(cookie_rsf, field_value);
		
		THTTPHdrVal cookie_hdr;
		RStringF cookie_name_rsf = string_pool.StringF(HTTP::ECookieName,RHTTPSession::GetTable());
		RStringF name = string_pool.OpenFStringL (KCookie);
		cookie_hdr.SetStrF (name);
		aHeaders.SetParamL (cookie_rsf, cookie_name_rsf, cookie_hdr, 0);
		
		RStringF cookie_value_rsf = string_pool.StringF(HTTP::ECookieValue,RHTTPSession::GetTable());
		RStringF value = string_pool.OpenFStringL (text3);
		cookie_hdr.SetStrF (value);
		aHeaders.SetParamL (cookie_rsf, cookie_value_rsf, cookie_hdr, 0);
	}


// ----------------------------------------------------------------------------
// CLocalizame::Login()
//
// Login into Localizame platform.
// ----------------------------------------------------------------------------
void CLocalizame::Login(const TDesC8& aLogin,
								 const TDesC8& aPwd)
	{
	SetupConnectionL();
	
	iPetition= (KPetLogin);
	iPetitionSuccessful = ETrue;
	
	// mount the body	
	TBuf8<KDefaultBufferSize> text;	
	
	// Login
	text=(_L8( "usuario="));
	text+= (aLogin);
	
	// password
	text+=(_L8("&clave="));
	text+= (aPwd);

	// ending
	text+=(_L8("&submit.x=36&submit.y=6"));
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/login.do"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
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
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = EFalse;
		
	iSchedulerWait.Start();
	
	NuevoUsuario();
	}


// ----------------------------------------------------------------------------
// CLocalizame::NuevoUsuario()
//
// get ready to be found by Localizame platform.
// ----------------------------------------------------------------------------
void CLocalizame::NuevoUsuario()
	{
	SetupConnectionL();
	
	iPetition= (KPetNuevoUsuario);
	iPetitionSuccessful = ETrue;
	
	// URL
	TBuf8<KDefaultBufferSize> aUrl;
	
	// NuevoUsuario
	aUrl= (_L8("/nuevousuario.do"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EGET,
		RHTTPSession::GetTable());
	
	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// Set headers for request
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);	
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EConnection, KConnection);
	SetHeaderL(hdr, HTTP::EReferer, aUrl);
	
	// cookie
	SetCookie(hdr);	

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iRunning = ETrue;
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = EFalse;
	
	iSchedulerWait.Start();
	}
	
	
// ----------------------------------------------------------------------------
// CLocalizame::Locate()
//
// Find a movile phone in Localizame platform.
// ----------------------------------------------------------------------------
TBuf8<1024> CLocalizame::Locate(const TDesC8& aTlf)
	{
	SetupConnectionL();
	
	iPetition= (KPetLocaliza);	

	iTlf.Copy(aTlf);
	iTlf.Append(_L(" "));
	
	// mount the body	
	TBuf8<KDefaultBufferSize> text;	
	
	// tlf
	text=(_L8( "telefono="));
	text+= (aTlf);
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/buscar.do"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
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
	
	// cookie
	SetCookie(hdr);
	
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
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = ETrue;
	
	iSchedulerWait.Start();
	
	// set iResult
	TInt pos = iResponse.Find(iTlf);
	if (pos != KErrNotFound)
		{
		TInt pos2 = iResponse.Find(KLocStr);
		if (pos2 != KErrNotFound)
			{
			iResult = iResponse.Mid(pos, pos2+11-pos);
			}
		}
		
	if (iResult == KNull8)
	{
		iResult.Copy(_L8("Location Error"));
	}
	
	iLocating = EFalse;
	iResponse.Copy(KNull8);
	
	return iResult;
	}
	
	
// ----------------------------------------------------------------------------
// CLocalizame::Authorize()
//
// autorize a movile phone to find us.
// ----------------------------------------------------------------------------
void CLocalizame::Authorize(const TDesC8& aTlf)
	{
	SetupConnectionL();
	
	iPetition= (KPetAutoriza);
	iPetitionSuccessful = ETrue;
	
	// mount the body	
	TBuf8<KDefaultBufferSize> aReferer;
	aReferer= (KHost);
	aReferer+= (_L8("/buscalocalizadorespermisos.do"));
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/insertalocalizador.do"));
	
	// tlf
	aUrl+=(_L8( "?telefono="));
	aUrl+= (aTlf);
	
	// ending
	aUrl+=(_L8("&submit.x=40&submit.y=5"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EGET,
		RHTTPSession::GetTable());
	
	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// Set headers for request
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EConnection, KConnection);
	SetHeaderL(hdr, HTTP::EReferer, aReferer);
	
	// cookie
	SetCookie(hdr);

	// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
	// are called when framework needs to send body data.
	//MHTTPDataSupplier* dataSupplier = this;
	//iTransaction.Request().SetBody(*dataSupplier);

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iRunning = ETrue;
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = EFalse;
	
	iSchedulerWait.Start();
	}
	
	
// ----------------------------------------------------------------------------
// CLocalizame::Unauthorize()
//
// unautorize a movile phone to find us.
// ----------------------------------------------------------------------------
void CLocalizame::Unauthorize(const TDesC8& aTlf)
	{
	SetupConnectionL();
		
	iPetition= (KPetDesautoriza);
	iPetitionSuccessful = ETrue;
	
	// mount the body	
	TBuf8<KDefaultBufferSize> aReferer;
	aReferer= (KHost);
	aReferer+= (_L8("/buscalocalizadorespermisos.do"));
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/borralocalizador.do"));
	
	// tlf
	aUrl+=(_L8( "?telefono="));
	aUrl+= (aTlf);
	
	// ending
	aUrl+=(_L8("&submit.x=44&submit.y=8"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EGET,
		RHTTPSession::GetTable());
	
	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// Set headers for request
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EConnection, KConnection);
	SetHeaderL(hdr, HTTP::EReferer, aReferer);
	
	// cookie
	SetCookie(hdr);

	// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
	// are called when framework needs to send body data.
	//MHTTPDataSupplier* dataSupplier = this;
	//iTransaction.Request().SetBody(*dataSupplier);

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iRunning = ETrue;
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = EFalse;
	
	iSchedulerWait.Start();
	}
	
	
// ----------------------------------------------------------------------------
// CLocalizame::Logout()
//
// Logout from Localizame platform.
// ----------------------------------------------------------------------------
void CLocalizame::Logout()
	{
	SetupConnectionL();
		
	iPetition= (KPetLogout);
	iPetitionSuccessful = ETrue;
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/logout.do"));
	
	// Parse string to URI
	TUriParser8 uri;
	uri.Parse(aUrl);
	
	// Get request method string for HTTP POST
	RStringF method = iSession.StringPool().StringF(HTTP::EGET,
		RHTTPSession::GetTable());
	
	// Open transaction with previous method and parsed uri. This class will
	// receive transaction events in MHFRunL and MHFRunError.
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// Set headers for request
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EAccept, KAccept);
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EConnection, KConnection);
	
	// cookie
	SetCookie(hdr);	

	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();

	iRunning = ETrue;
	iResponse.Copy(KNull8);
	iResult.Copy(KNull8);
	iLocating = EFalse;
	
	aCookie = (KNull);
	
	iSchedulerWait.Start();
	}
	

// ----------------------------------------------------------------------------
// CLocalizame::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CLocalizame::CancelTransaction()
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
// CLocalizame::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CLocalizame::MHFRunL(RHTTPTransaction aTransaction,
						  const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. Use
			// aTransaction.Response() to get the response. However, it's not
			// necessary to do anything with the response when this event occurs.

			// Get HTTP status code from header (e.g. 200)
			RHTTPResponse resp = aTransaction.Response();
						
			// get headers
			RStringPool strP = aTransaction.Session().StringPool();
			RHTTPHeaders hdr = resp.GetHeaderCollection();
			THTTPHdrFieldIter it = hdr.Fields();
			RStringF wwwAuth;
			
			while (it.AtEnd() == EFalse)
			    {
			    // Get the name of the next header field
			    RStringTokenF fieldName = it();
			    RStringF fieldNameStr = strP.StringF(fieldName);
			
			    // Check it does indeed exist
			    THTTPHdrVal fieldVal;
			    if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
			        {
			        // Display header			        
			        wwwAuth = strP.StringF(HTTP::ESetCookie,RHTTPSession::GetTable());
			        if (fieldNameStr == wwwAuth)
			            {
									// sacar valor
									RStringF fieldCokie = strP.StringF(HTTP::ESetCookie,RHTTPSession::GetTable());
									THTTPHdrVal fieldVal;
									THTTPHdrVal cookie;
									
									if (hdr.GetField(fieldCokie,0,fieldVal) == KErrNone){											
											RStringF basic = strP.StringF(HTTP::ECookie,RHTTPSession::GetTable());
											RStringF realm = strP.StringF(HTTP::ECookieValue,RHTTPSession::GetTable());
											
											if ((fieldVal.StrF() == basic) &&
											(!hdr.GetParam(fieldCokie, realm, cookie))){
												RStringF realmValStr = strP.StringF(cookie.StrF());
												aCookie.Copy(realmValStr.DesC());
											}
										}			                
			            }
			        }
			        // Advance the iterator
			        ++it;
			    }
			}
			break;
			
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
			if (iLocating != EFalse)
				{
				iResponse.Append(dataChunk);
				}
			
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
// CLocalizame::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CLocalizame::MHFRunError(TInt aError,
							  RHTTPTransaction /*aTransaction*/,
							  const THTTPEvent& /*aEvent*/)
	{
	// Just notify about the error and return KErrNone.
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CLocalizame::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CLocalizame::GetNextDataPart(TPtrC8& aDataPart)
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
// CLocalizame::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers)
// ----------------------------------------------------------------------------
void CLocalizame::ReleaseData()
	{
	// It's safe to delete iPostData now.
	delete iPostData;
	iPostData = NULL;
	}

// ----------------------------------------------------------------------------
// CLocalizame::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data
// supplier that it should return to the first part of the data.
// In practise an error has occured while sending data, and framework needs to
// resend data.
// ----------------------------------------------------------------------------
TInt CLocalizame::Reset()
	{
	// Nothing needed since iPostData still exists and contains all the data.
	// (If a file is used and read in small parts we should seek to beginning
	// of file and provide the first chunk again in GetNextDataPart() )
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CLocalizame::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound (it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content
// length in header).
// ----------------------------------------------------------------------------
TInt CLocalizame::OverallDataSize()
	{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
	}

// ----------------------------------------------------------------------------
// CLocalizame::GetCredentialsL()
//
// Inherited from MHTTPAuthenticationCallback
// Called by framework when we requested authenticated page and framework
// needs to know username and password.
// ----------------------------------------------------------------------------
TBool CLocalizame::GetCredentialsL(const TUriC8& aURI,
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
// CLocalizame::SetupConnectionL()
//
// The method set the internet access point and connection setups.
// ----------------------------------------------------------------------------	
void CLocalizame::SetupConnectionL()
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
