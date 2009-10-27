/*
 * ============================================================================
 *  Name     : CSMS20 of SMS20.cpp
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
#include <SMS20Example.rsg>

#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <CommDbConnPref.h>

#include "SMS20.h"

// Used user agent for requests
_LIT8(KUserAgent, "SimpleClient 1.0");

// headers
_LIT8(KAccept, "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*");
_LIT8(KContentType, "application/vnd.wv.csp.xml");
_LIT8(KContentTypeLog, "application/x-www-form-urlencoded");
_LIT8(KHost, "sms20.movistar.es");
_LIT8(KHostLog, "impw.movistar.es");
_LIT8(KEncoding, "identity");

_LIT8(KExpect, "100-continue");

// null strings
_LIT(KNull, "");
_LIT8(KNull8, "");

_LIT8(KPetRestridted, "Restringido");
_LIT8(KUrl, "/");

// xmls
// connect
_LIT8(KLogoutReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><Logout-Request /></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KCapabilityReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ClientCapability-Request><ClientID><URL>WV:InstantMessenger-1.0.2309.16485@COLIBRIA.PC-CLIENT</URL></ClientID><CapabilityList><ClientType>COMPUTER</ClientType><InitialDeliveryMethod>P</InitialDeliveryMethod><AcceptedContentType>text/plain</AcceptedContentType><AcceptedContentType>text/html</AcceptedContentType><AcceptedContentType>image/png</AcceptedContentType><AcceptedContentType>image/jpeg</AcceptedContentType><AcceptedContentType>image/gif</AcceptedContentType><AcceptedContentType>audio/x-wav</AcceptedContentType><AcceptedContentType>image/jpg</AcceptedContentType><AcceptedTransferEncoding>BASE64</AcceptedTransferEncoding><AcceptedContentLength>256000</AcceptedContentLength><MultiTrans>1</MultiTrans><ParserSize>300000</ParserSize><SupportedCIRMethod>STCP</SupportedCIRMethod><ColibriaExtensions>T</ColibriaExtensions></CapabilityList></ClientCapability-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KServiceReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><Service-Request><ClientID><URL>WV:InstantMessenger-1.0.2309.16485@COLIBRIA.PC-CLIENT</URL></ClientID><Functions><WVCSPFeat><FundamentalFeat /><PresenceFeat /><IMFeat /><GroupFeat /></WVCSPFeat></Functions><AllFunctionsRequest>T</AllFunctionsRequest></Service-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KUpdatePresenceReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><UpdatePresence-Request><PresenceSubList xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"><OnlineStatus><Qualifier>T</Qualifier></OnlineStatus><ClientInfo><Qualifier>T</Qualifier><ClientType>COMPUTER</ClientType><ClientTypeDetail xmlns=\"http://imps.colibria.com/PA-ext-1.2\">PC</ClientTypeDetail><ClientProducer>Colibria As</ClientProducer><Model>TELEFONICA Messenger</Model><ClientVersion>1.0.2309.16485</ClientVersion></ClientInfo><CommCap><Qualifier>T</Qualifier><CommC><Cap>IM</Cap><Status>OPEN</Status></CommC></CommCap><UserAvailability><Qualifier>T</Qualifier><PresenceValue>AVAILABLE</PresenceValue></UserAvailability></PresenceSubList></UpdatePresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KGetListReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><GetList-Request /></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KGetPresenceReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><GetPresence-Request><User><UserID>wv:%S@movistar.es</UserID></User></GetPresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KListManageReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ListManage-Request><ContactList>wv:%S/~pep1.0_privatelist@movistar.es</ContactList><ReceiveList>T</ReceiveList></ListManage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KCreateListReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><CreateList-Request><ContactList>wv:%S/~PEP1.0_subscriptions@movistar.es</ContactList>%S</CreateList-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KSubscribePresenceReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><SubscribePresence-Request><ContactList>wv:%S/~PEP1.0_subscriptions@movistar.es</ContactList><PresenceSubList xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"><OnlineStatus /><ClientInfo /><FreeTextLocation /><CommCap /><UserAvailability /><StatusText /><StatusMood /><Alias /><StatusContent /><ContactInfo /></PresenceSubList><AutoSubscribe>T</AutoSubscribe></SubscribePresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KUpdatePresenceReStr2,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><UpdatePresence-Request><PresenceSubList xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"><Alias><Qualifier>T</Qualifier><PresenceValue>%S</PresenceValue></Alias></PresenceSubList></UpdatePresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");

_LIT8(KNullNick8, "<NickList />");

// polling
_LIT8(KPollingReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID /></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><Polling-Request /></TransactionContent></Transaction></Session></WV-CSP-Message>");

// add contact
_LIT8(KSearchReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><Search-Request><SearchPairList><SearchElement>USER_MOBILE_NUMBER</SearchElement><SearchString>%S</SearchString></SearchPairList><SearchLimit>50</SearchLimit></Search-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KGetPresenceReStrC,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><GetPresence-Request><User><UserID>wv:%S@movistar.es</UserID></User><PresenceSubList xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"><OnlineStatus /><ClientInfo /><GeoLocation /><FreeTextLocation /><CommCap /><UserAvailability /><StatusText /><StatusMood /><Alias /><StatusContent /><ContactInfo /></PresenceSubList></GetPresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KListManageReStrC,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ListManage-Request><ContactList>wv:%S/~PEP1.0_subscriptions@movistar.es</ContactList><AddNickList><NickName><Name>%S</Name><UserID>wv:%S@movistar.es</UserID></NickName></AddNickList><ReceiveList>T</ReceiveList></ListManage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KListManagePLReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ListManage-Request><ContactList>wv:%S/~PEP1.0_privatelist@movistar.es</ContactList><AddNickList><NickName><Name>%S</Name><UserID>wv:%S@movistar.es</UserID></NickName></AddNickList><ReceiveList>T</ReceiveList></ListManage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");

// autorice
_LIT8(KGetPresenceReStrA,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><GetPresence-Request><User><UserID>%S</UserID></User><PresenceSubList xmlns=\"http://www.openmobilealliance.org/DTD/WV-PA1.2\"><OnlineStatus /><ClientInfo /><GeoLocation /><FreeTextLocation /><CommCap /><UserAvailability /><StatusText /><StatusMood /><Alias /><StatusContent /><ContactInfo /></PresenceSubList></GetPresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KStatusReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Response</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><Status><Result><Code>200</Code></Result></Status></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KPresenceAuthReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><PresenceAuth-User><UserID>%S</UserID><Acceptance>T</Acceptance></PresenceAuth-User></TransactionContent></Transaction></Session></WV-CSP-Message>");

// del contact
_LIT8(KListManageReStrD,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ListManage-Request><ContactList>wv:%S/~PEP1.0_subscriptions@movistar.es</ContactList><RemoveNickList><UserID>%S</UserID></RemoveNickList><ReceiveList>T</ReceiveList></ListManage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KListManagePReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><ListManage-Request><ContactList>wv:%S/~PEP1.0_privatelist@movistar.es</ContactList><RemoveNickList><UserID>%S</UserID></RemoveNickList><ReceiveList>T</ReceiveList></ListManage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KnsubscribePresenceReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><UnsubscribePresence-Request><User><UserID>%S</UserID></User></UnsubscribePresence-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KDeleteAttributeListReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><DeleteAttributeList-Request><UserID>%S</UserID><DefaultList>F</DefaultList></DeleteAttributeList-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");
_LIT8(KCancelAuthReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><CancelAuth-Request><UserID>%S</UserID></CancelAuth-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");

// send message
_LIT8(KSendMessageReStr,"<?xml version=\"1.0\" encoding=\"utf-8\"?><WV-CSP-Message xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.openmobilealliance.org/DTD/WV-CSP1.2\"><Session><SessionDescriptor><SessionType>Inband</SessionType><SessionID>%S</SessionID></SessionDescriptor><Transaction><TransactionDescriptor><TransactionMode>Request</TransactionMode><TransactionID>%d</TransactionID></TransactionDescriptor><TransactionContent xmlns=\"http://www.openmobilealliance.org/DTD/WV-TRC1.2\"><SendMessage-Request><DeliveryReport>F</DeliveryReport><MessageInfo><ContentType>text/html</ContentType><ContentSize>148</ContentSize><Recipient><User><UserID>%S</UserID></User></Recipient><Sender><User><UserID>%S@movistar.es</UserID></User></Sender></MessageInfo><ContentData>&lt;span style=\"color:#000000;font-family:\"Microsoft Sans Serif\";font-style:normal;font-weight:normal;font-size:12px;\"&gt;%S&lt;/span&gt;</ContentData></SendMessage-Request></TransactionContent></Transaction></Session></WV-CSP-Message>");

// buffer sizes
const TInt KStatustextBufferSize = 32;
const TInt KInfotextBufferSize = 64;
const TInt KURIBufferSize = 128;
const TInt KDefaultBufferSize = 256;


// ----------------------------------------------------------------------------
// CContactoSMS20::CContactoSMS20()
//
// Creates instance of CContactoSMS20.
// ----------------------------------------------------------------------------
CContactoSMS20::CContactoSMS20()
{
	m_bPresente = EFalse;
}

// ----------------------------------------------------------------------------
// CContactoSMS20::CContactoSMS20()
//
// Creates instance of CContactoSMS20.
// ----------------------------------------------------------------------------
CContactoSMS20::CContactoSMS20(const TDesC8& csUserID, const TDesC8& csAlias, TBool bPresente)
{
	m_csUserID = csUserID;
	m_csAlias = csAlias;
	m_bPresente = bPresente;
}

// ----------------------------------------------------------------------------
// CSMS20::NewL()
//
// Creates instance of CSMS20.
// ----------------------------------------------------------------------------
CSMS20* CSMS20::NewL()
	{
    CSMS20* self = CSMS20::NewLC();
    CleanupStack::Pop(self);
    return self;
	}


// ----------------------------------------------------------------------------
// CSMS20::NewLC()
//
// Creates instance of CSMS20.
// ----------------------------------------------------------------------------
CSMS20* CSMS20::NewLC()
	{
    CSMS20* self = new (ELeave) CSMS20();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


// ----------------------------------------------------------------------------
// CSMS20::CSMS20()
//
// First phase constructor.
// ----------------------------------------------------------------------------
CSMS20::CSMS20()
:	iPostData(NULL),
	iRunning(EFalse)
	{
		iSessionId = HBufC8::NewL(0);
		iSessionId->Des().Copy(KNull8);
		iTransId = 0;
		iAlias = (KNull8);
		iTlf = (KNull8);
	}


// ----------------------------------------------------------------------------
// CSMS20::~CSMS20()
//
// Destructor.
// ----------------------------------------------------------------------------
CSMS20::~CSMS20()
	{	
	iSession.Close();

    // and finally close handles
    iConnection.Close();
    iSocketServ.Close();

	delete iPostData;
	}


// ----------------------------------------------------------------------------
// CSMS20::ConstructL()
//
// Second phase construction.
// ----------------------------------------------------------------------------
void CSMS20::ConstructL()
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
	iResponse = HBufC8::NewL(0);
	}


// ----------------------------------------------------------------------------
// CSMS20::SetHeaderL()
//
// Used to set header value to HTTP request
// ----------------------------------------------------------------------------
void CSMS20::SetHeaderL(RHTTPHeaders aHeaders,
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
// CSMS20::Login()
//
// Login into SMS20 platform.
// ----------------------------------------------------------------------------
HBufC8* CSMS20::Login(const TDesC8& aLogin,
								 const TDesC8& aPwd)
	{
	SetupConnectionL();

	iTlf.Copy(aLogin);
	
	// mount the body	
	TBuf8<KDefaultBufferSize> text;	
	
	// Login
	text=(_L8( "TM_ACTION=AUTHENTICATE&TM_LOGIN="));
	text+= (aLogin);
	
	// password
	text+=(_L8("&TM_PASSWORD="));
	text+= (aPwd);

	// ending
	text+=(_L8("&SessionCookie=ColibriaIMPS_367918656&ClientID=WV:InstantMessenger-1.0.2309.16485@COLIBRIA.PC-CLIENT"));
		
	// URL
	TBuf8<KDefaultBufferSize> aUrl;	
	
	// Login
	aUrl= (_L8("/tmelogin/tmelogin.jsp"));
	
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
	SetHeaderL(hdr, HTTP::EContentType, KContentTypeLog);
	SetHeaderL(hdr, HTTP::EHost, KHostLog);
	
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

	// clean response
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	TPtr8 rwResponse2(iResponse->Des());	
	rwResponse2.Copy(KNull8);	
	
	// wait for http response	
	iSchedulerWait.Start();
	
	// get sesion ID
	delete iSessionId;
	iSessionId = FindValue(iResponse->Des(), _L8("SessionID"));
	
	// get transaction ID
	TBuf<256> Temp;
	Temp.Copy(FindValue(iResponse->Des(), _L8("TransactionID"))->Des());
	TLex iLex(Temp);
	iLex.Val(iTransId);
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	return iSessionId;
	}


// ----------------------------------------------------------------------------
// CSMS20::Connect()
//
//  connect to SMS20 platform.
// ----------------------------------------------------------------------------
RPointerArray<CContactoSMS20> CSMS20::Connect(const TDesC8& aAlias)
	{
	// mount the body
	HBufC8*	iPost;
	HBufC8*	aNickL;
	
	RPointerArray<CContactoSMS20> iContactList;
	
	// Iniciamos acceso a sms20.movistar.es
	// Enviamos <ClientCapability-Request>
	iPost = HBufC8::NewL(KCapabilityReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KCapabilityReStr, iSessionId, ++iTransId);	
	PostHTTP(iPost->Des());	
	
	// Enviamos <Service-Request>
	delete iPost;
	iPost = HBufC8::NewL(KServiceReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KServiceReStr, iSessionId, ++iTransId);	
	PostHTTP(iPost->Des());
	
	// Enviamos <UpdatePresence-Request> para avisar de que estamos conectados con un messenger
	delete iPost;
	iPost = HBufC8::NewL(KUpdatePresenceReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KUpdatePresenceReStr, iSessionId, ++iTransId);	
	PostHTTP(iPost->Des());
	
	// Enviamos <GetList-Request>
	delete iPost;
	iPost = HBufC8::NewL(KGetListReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KGetListReStr, iSessionId, ++iTransId);	
	PostHTTP(iPost->Des());
	
	// Enviamos <GetPresence-Request>
	delete iPost;
	iPost = HBufC8::NewL(KGetPresenceReStr().Length() + iSessionId->Length() + iTlf.Length() + 6);	
	iPost->Des().Format(KGetPresenceReStr, iSessionId, ++iTransId, &iTlf);	
	PostHTTP(iPost->Des());
	
	// Enviamos <ListManage-Request>
	delete iPost;
	iPost = HBufC8::NewL(KListManageReStr().Length() + iSessionId->Length() + iTlf.Length() + 6);	
	iPost->Des().Format(KListManageReStr, iSessionId, ++iTransId, &iTlf);
	PostHTTP(iPost->Des());
	
	// sacamos lista de contactos
	aNickL = FindValue(iResponse->Des(), _L8("NickList"));
	delete iResponse;
	
	if (aNickL->Des() != KNull8)
		{
		iResponse = HBufC8::NewL(aNickL->Length());
		iResponse->Des().Copy(aNickL->Des());
			
		// no es vacia, añadimos tags
		aNickL = aNickL->ReAlloc(aNickL->Length()+21);
		aNickL->Des().Insert(0,_L8("<NickList>"));
		aNickL->Des().Append(_L8("</NickList>"));
		
		// guardamos en local
		iContactList = GetContactList();
		}
	else
		{
		// lista de contactos	vacia
		delete aNickL;
		aNickL = HBufC8::NewL(KNullNick8().Length());
		aNickL->Des().Copy(KNullNick8);
		}
	
	// Enviamos <CreateList-Request>	
	delete iPost;
	iPost = HBufC8::NewL(KCreateListReStr().Length() + iSessionId->Length() + iTlf.Length() + aNickL->Length() + 6);
	iPost->Des().Format(KCreateListReStr, iSessionId, ++iTransId, &iTlf, aNickL);
	PostHTTP(iPost->Des());
	
	// Enviamos <SubscribePresence-Request>
	delete iPost;
	iPost = HBufC8::NewL(KSubscribePresenceReStr().Length() + iSessionId->Length() + iTlf.Length() + 6);	
	iPost->Des().Format(KSubscribePresenceReStr, iSessionId, ++iTransId, &iTlf);	
	PostHTTP(iPost->Des());	
	
	// Enviamos <UpdatePresence-Request> para enviar nuestro nick
	// Sólo la primera vez y cuando queramos cambiar de nick
	if (aAlias != (KNullNick8))	
		{
		iAlias.Copy(aAlias);
		delete iPost;
		iPost = HBufC8::NewL(KUpdatePresenceReStr2().Length() + iSessionId->Length() + iAlias.Length() + 6);	
		iPost->Des().Format(KUpdatePresenceReStr2, iSessionId, ++iTransId, &iAlias);	
		PostHTTP(iPost->Des());
		}
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	return iContactList;
	}
	

// ----------------------------------------------------------------------------
// CSMS20::Polling()
//
// Wait from a SMS20 platform response.
// ----------------------------------------------------------------------------
HBufC8* CSMS20::Polling()
	{
	// mount the body
	HBufC8*	iPost;	
	HBufC8*	iResult;
	
	// Iniciamos acceso a sms20.movistar.es
	// Enviamos <ClientCapability-Request>
	iPost = HBufC8::NewL(KPollingReStr().Length() + iSessionId->Length());
	iPost->Des().Format(KPollingReStr, iSessionId);	
	PostHTTP(iPost->Des());
	
	// respuesta limpia
	delete iPost;
	iResult = HBufC8::NewL(0);
	iResult->Des().Copy(KNull8);
	
	// miramos si es un mensaje	
	iPost = FindValue(iResponse->Des(), _L8("NewMessage"));
	if (iPost->Des() != (KNull8))
		{
		iResult = GetMsg(iPost->Des());
		}
	
	// miramos si es una notificacion de presencia
	iPost = FindValue(iResponse->Des(), _L8("PresenceNotification-Request"));
	if (iPost->Des() != (KNull8))
		{
		iResult = HBufC8::NewL(iPost->Length());
		iResult->Des().Copy(iPost->Des());			
		}
	
	// miramos si es una solicitud de permiso
	iPost = FindValue(iResponse->Des(), _L8("PresenceAuth-Request"));
	if (iPost->Des() != (KNull8))				
		{
		iResult = GetUser(iPost->Des());
		}
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	return iResult;
	}


// ----------------------------------------------------------------------------
// CSMS20::AddContact()
//
// add a contact to the contact list in SMS20 platform.
// ----------------------------------------------------------------------------
HBufC8* CSMS20::AddContact(const TDesC8& aContact)
	{
	// mount the body
	HBufC8*	iPost;
	HBufC8*	iNick;

	// Enviamos <Search-Request> (en teoría para obtener el userId)
	iPost = HBufC8::NewL(KSearchReStr().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KSearchReStr, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());

	// Enviamos <GetPresence-Request> para obtener el estado del contacto inmediatamente
	//	(sin esto habría que esperar a que el contacto cambie de estado)
	delete iPost;
	iPost = HBufC8::NewL(KGetPresenceReStrC().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KGetPresenceReStrC, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());

	iNick = GetAlias(iResponse->Des(), aContact);

	// Enviamos <ListManage-Request>
	delete iPost;
	iPost = HBufC8::NewL(KListManageReStrC().Length() + iSessionId->Length() + iTlf.Length() + iNick->Length() + aContact.Length() + 6);
	iPost->Des().Format(KListManageReStrC, iSessionId, ++iTransId, &iTlf, iNick, &aContact);
	PostHTTP(iPost->Des());

	// Enviamos <ListManage-Request> esta vez para la PrivateList
	delete iPost;
	iPost = HBufC8::NewL(KListManagePLReStr().Length() + iSessionId->Length() + iTlf.Length() + iNick->Length() + aContact.Length() + 6);
	iPost->Des().Format(KListManagePLReStr, iSessionId, ++iTransId, &iTlf, iNick, &aContact);
	PostHTTP(iPost->Des());
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	return iNick;
	}

	
// ----------------------------------------------------------------------------
// CSMS20::AuthorizeContact()
//
// autorice a contact in SMS20 platform..
// ----------------------------------------------------------------------------
void CSMS20::AuthorizeContact(const TDesC8& aContact)
	{
	// mount the body
	HBufC8*	iPost;

	// Enviamos <GetPresence-Request>
	iPost = HBufC8::NewL(KGetPresenceReStrA().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KGetPresenceReStrA, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());
	

	// Enviamos <Status> para hacer el ack de la petición
	delete iPost;
	iPost = HBufC8::NewL(KStatusReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KStatusReStr, iSessionId, ++iTransId);	
	PostHTTP(iPost->Des());


	// Enviamos <PresenceAuth-User>
	delete iPost;
	iPost = HBufC8::NewL(KPresenceAuthReStr().Length() + iSessionId->Length() + aContact.Length() + 6);	
	iPost->Des().Format(KPresenceAuthReStr, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());		
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	}
	

// ----------------------------------------------------------------------------
// CSMS20::DeleteContact()
//
// delete a from the contact list in SMS20 platform.
// ----------------------------------------------------------------------------
void CSMS20::DeleteContact(const TDesC8& aContact)
	{
	// mount the body
	HBufC8*	iPost;

	// Enviamos <ListManage-Request> para eliminar contacto de la lista de suscripciones
	iPost = HBufC8::NewL(KListManageReStrD().Length() + iSessionId->Length() + iTlf.Length() + aContact.Length() + 6);
	iPost->Des().Format(KListManageReStrD, iSessionId, ++iTransId, &iTlf, &aContact);	
	PostHTTP(iPost->Des());
	
	// Enviamos <ListManage-Request> para eliminar contacto de la lista privada
	delete iPost;
	iPost = HBufC8::NewL(KListManagePReStr().Length() + iSessionId->Length() + iTlf.Length() + aContact.Length() + 6);
	iPost->Des().Format(KListManagePReStr, iSessionId, ++iTransId, &iTlf, &aContact);
	PostHTTP(iPost->Des());
	
	// Enviamos <UnsubscribePresence-Request>
	delete iPost;
	iPost = HBufC8::NewL(KnsubscribePresenceReStr().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KnsubscribePresenceReStr, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());
	
	// Enviamos <DeleteAttributeList-Request>
	delete iPost;
	iPost = HBufC8::NewL(KDeleteAttributeListReStr().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KDeleteAttributeListReStr, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());
	
	// Enviamos <CancelAuth-Request>
	delete iPost;
	iPost = HBufC8::NewL(KCancelAuthReStr().Length() + iSessionId->Length() + aContact.Length() + 6);
	iPost->Des().Format(KCancelAuthReStr, iSessionId, ++iTransId, &aContact);	
	PostHTTP(iPost->Des());
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	}


// ----------------------------------------------------------------------------
// CSMS20::SendMessage()
//
// send a message to a contact
// ----------------------------------------------------------------------------
void CSMS20::SendMessage(const TDesC8& aDest, const TDesC8& aMsg)
	{
	// mount the body
	HBufC8*	iPost;
	
	// Enviamos <SendMessage-Request>
	iPost = HBufC8::NewL(KSendMessageReStr().Length() + iSessionId->Length() + iSessionId->Length() + aDest.Length() + iTlf.Length() + aMsg.Length());
	iPost->Des().Format(KSendMessageReStr, iSessionId, ++iTransId, &aDest, &iTlf, &aMsg);
	PostHTTP(iPost->Des());
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	}


// ----------------------------------------------------------------------------
// CSMS20::Logout()
//
// Logout from SMS20 platform.
// ----------------------------------------------------------------------------
void CSMS20::Logout()
	{
	HBufC8*	iPost;
	
	iPost = HBufC8::NewL(KLogoutReStr().Length() + iSessionId->Length() + 6);	
	iPost->Des().Format(KLogoutReStr, iSessionId, ++iTransId);
	
	// send request
	PostHTTP(iPost->Des());
	
	// clean sesion vars
	delete iSessionId;
	iSessionId = HBufC8::NewL(0);
	iSessionId->Des().Copy(KNull8);
	iTransId = 0;
	iAlias = (KNull8);
	iTlf = (KNull8);
	
	delete iResponse;
	iResponse = HBufC8::NewL(0);
		
	}
	
	
// ----------------------------------------------------------------------------
// CSMS20::CancelTransaction()
//
// Cancels currently running transaction and frees resources related to it.
// ----------------------------------------------------------------------------
void CSMS20::CancelTransaction()
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
// CSMS20::MHFRunL()
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
void CSMS20::MHFRunL(RHTTPTransaction aTransaction,
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

			// dinamic buffer			
			iResponse = iResponse->ReAlloc(iResponse->Length()+dataChunk.Length());
			TPtr8 rwResponse2(iResponse->Des());
			rwResponse2.Append(dataChunk);
			
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
// CSMS20::MHFRunError()
//
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
TInt CSMS20::MHFRunError(TInt aError,
							  RHTTPTransaction /*aTransaction*/,
							  const THTTPEvent& /*aEvent*/)
	{
	// Just notify about the error and return KErrNone.
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CSMS20::GetNextDataPart()
//
// Inherited from MHTTPDataSupplier
// Called by framework when next part of the body is needed. In this
// this provides data for HTTP post.
// ----------------------------------------------------------------------------
TBool CSMS20::GetNextDataPart(TPtrC8& aDataPart)
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
// CSMS20::ReleaseData()
//
// Inherited from MHTTPDataSupplier
// Called by framework. Allows us to release resources needed for previous
// chunk. (e.g. free buffers)
// ----------------------------------------------------------------------------
void CSMS20::ReleaseData()
	{
	// It's safe to delete iPostData now.
	delete iPostData;
	iPostData = NULL;
	}

// ----------------------------------------------------------------------------
// CSMS20::Reset()
//
// Inherited from MHTTPDataSupplier
// Called by framework to reset the data supplier. Indicates to the data
// supplier that it should return to the first part of the data.
// In practise an error has occured while sending data, and framework needs to
// resend data.
// ----------------------------------------------------------------------------
TInt CSMS20::Reset()
	{
	// Nothing needed since iPostData still exists and contains all the data.
	// (If a file is used and read in small parts we should seek to beginning
	// of file and provide the first chunk again in GetNextDataPart() )
	return KErrNone;
	}


// ----------------------------------------------------------------------------
// CSMS20::OverallDataSize()
//
// Inherited from MHTTPDataSupplier
// Called by framework. We should return the expected size of data to be sent.
// If it's not know we can return KErrNotFound (it's allowed and does not cause
// problems, since HTTP protocol allows multipart bodys without exact content
// length in header).
// ----------------------------------------------------------------------------
TInt CSMS20::OverallDataSize()
	{
	if(iPostData)
		return iPostData->Length();
	else
		return KErrNotFound ;
	}

// ----------------------------------------------------------------------------
// CSMS20::GetCredentialsL()
//
// Inherited from MHTTPAuthenticationCallback
// Called by framework when we requested authenticated page and framework
// needs to know username and password.
// ----------------------------------------------------------------------------
TBool CSMS20::GetCredentialsL(const TUriC8& aURI,
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
// CSMS20::SetupConnectionL()
//
// The method set the internet access point and connection setups.
// ----------------------------------------------------------------------------	
void CSMS20::SetupConnectionL()
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

// ----------------------------------------------------------------------------
// CSMS20::FindValue()
//
// Find a value in a xml between the tags
// ----------------------------------------------------------------------------
HBufC8*	CSMS20::FindValue(const TDesC8& csDatos, const TDesC8& csTag)
	{
	HBufC8* iResult;
	HBufC8* text;
	TInt aLen;
	
	iResult = HBufC8::NewL(0);
	TPtr8 rwResult(iResult->Des());	
	rwResult.Copy(KNull8);
	
	text = HBufC8::NewL(csTag.Length() + 3);
	TPtr8 rwtext(text->Des());	
	rwtext.Copy(_L8("<"));
	rwtext.Append(csTag);
	rwtext.Append(_L8(">"));
	
	aLen= rwtext.Length();

	TInt pos = csDatos.Find(rwtext);
	if (pos != KErrNotFound)
		{
		rwtext.Copy(_L8("</"));
		rwtext.Append(csTag);
		rwtext.Append(_L8(">"));
		
		TInt pos2 = csDatos.Find(rwtext);
		if (pos2 != KErrNotFound)
			{
			iResult = iResult->ReAlloc(pos2 - pos - aLen);
			TPtr8 rwResult2(iResult->Des());
			rwResult2.Copy(csDatos.Mid(pos + aLen, pos2 - pos - aLen));	
			}
		}

		return iResult;
	}
	
// ----------------------------------------------------------------------------
// CSMS20::PostHTTP()
//
// Send a post to SMS20 platform.
// ----------------------------------------------------------------------------
void CSMS20::PostHTTP(const TDesC8& aBody)
	{
	SetupConnectionL();

	// URL
	TBuf8<KDefaultBufferSize> aUrl;
	aUrl= (_L8("/"));
	
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
	SetHeaderL(hdr, HTTP::EContentType, KContentType);
	SetHeaderL(hdr, HTTP::EHost, KHost);
	SetHeaderL(hdr, HTTP::EExpect, KExpect);
	
	
	// Copy data to be posted into member variable; iPostData is used later in
	// methods inherited from MHTTPDataSupplier.
	delete iPostData;
	iPostData = aBody.AllocL();
	
	// Set this class as an data supplier. Inherited MHTTPDataSupplier methods
	// are called when framework needs to send body data.
	MHTTPDataSupplier* dataSupplier = this;
	iTransaction.Request().SetBody(*dataSupplier);
	
	// Submit the transaction. After this the framework will give transaction
	// events via MHFRunL and MHFRunError.
	iTransaction.SubmitL();
	
	iRunning = ETrue;
	
	// clean response
	delete iResponse;
	iResponse = HBufC8::NewL(0);
	
	TPtr8 rwResponse2(iResponse->Des());	
	rwResponse2.Copy(KNull8);	
	
	// wait for http response	
	iSchedulerWait.Start();
	
	// get transaction ID
	TBuf<256> Temp;
	Temp.Copy(FindValue(iResponse->Des(), _L8("TransactionID"))->Des());
	TLex iLex(Temp);
	iLex.Val(iTransId);
	}

// ----------------------------------------------------------------------------
// CSMS20::GetAlias()
//
// get the alias from the response
// ----------------------------------------------------------------------------	
HBufC8* CSMS20::GetAlias(const TDesC8& csDatos, const TDesC8& csLogin)
	{
	HBufC8* csPresenceValue;
	
	csPresenceValue = HBufC8::NewL(0);
	TPtr8 rwPV(csPresenceValue->Des());	
	rwPV.Copy(KNull8);
	
	HBufC8* csPresence = FindValue(csDatos, _L8("Presence"));
	if(csPresence->Des() != KNull8)
		{
		TBuf8<KDefaultBufferSize> csUserIdT = _L8("wv:");
		csUserIdT += (csLogin);
		csUserIdT += _L8("@movistar.es");
		
		HBufC8* csUserId = FindValue(csPresence->Des(),_L8("UserID"));
		if(csUserId->Des() == csUserIdT)
			{
			HBufC8* csAlias = FindValue(csPresence->Des(),_L8("Alias"));
			if(csAlias->Des() != KNull8)
				{
				csPresenceValue = FindValue(csAlias->Des(),_L8("PresenceValue"));
				}
			}
		}
		return csPresenceValue;
	}

// ----------------------------------------------------------------------------
// CSMS20::GetContactList()
//
//	parses a string to get the contact list
// ----------------------------------------------------------------------------	
RPointerArray<CContactoSMS20> CSMS20::GetContactList()
	{
	HBufC8* csContact;
	TInt 		pos;
	CContactoSMS20* contacto;	
	RPointerArray<CContactoSMS20> iContactList;
	
	// buscar lista
	csContact = FindValue(iResponse->Des(), _L8("NickName"));
	
	while (csContact->Des() != KNull8)
		{
		// sacamos parametros
		HBufC8* csName = FindValue(csContact->Des(),_L8("Name"));
		HBufC8* csUserID = FindValue(csContact->Des(),_L8("UserID"));
		
		// sacamos solo el numero de tlf
		csUserID->Des().Copy(csUserID->Des().Mid(3, 9));
		
		// creamos contacto
		contacto = new (ELeave) CContactoSMS20(csUserID->Des(), csName->Des());
		
		// añadimos a la lista
		iContactList.Append(contacto);
		
		// cortamos la parte que ya hemos tratado		
		pos = iResponse->Des().Find(_L8("</NickName>"));
		iResponse->Des().Copy(iResponse->Des().Right(iResponse->Length() - pos - 11));
		
		// buscamos siguiente
		csContact = FindValue(iResponse->Des(), _L8("NickName"));
		}
		
		return iContactList;
	}

// ----------------------------------------------------------------------------
// CSMS20::GetPresenceList()
//
// parses a string to get the presence list
// ----------------------------------------------------------------------------	
RPointerArray<CContactoSMS20> CSMS20::GetPresenceList(const TDesC8& csContacts)
	{
	HBufC8* iResponseT;
	HBufC8* csContact;
	CContactoSMS20* contacto;	
	RPointerArray<CContactoSMS20> iContactList;
	
	iResponseT = HBufC8::NewL(csContacts.Length());	
	iResponseT->Des().Copy(csContacts);
	
	// buscar lista
	csContact = FindValue(iResponseT->Des(), _L8("Presence"));	
	while (csContact->Des() != KNull8)
		{
		// sacamos parametros		
		HBufC8* csUserID = FindValue(csContact->Des(),_L8("UserID"));
		
		// sacamos solo el numero de tlf
		csUserID->Des().Copy(csUserID->Des().Mid(3, 9));
		
		HBufC8* csPresente = FindValue(csContact->Des(),_L8("UserAvailability"));
		csPresente = FindValue(csPresente->Des(),_L8("PresenceValue"));
		
		HBufC8* csAlias = FindValue(csContact->Des(),_L8("Alias"));
		csAlias = FindValue(csAlias->Des(),_L8("PresenceValue"));
		
		// creamos contacto
		contacto = new (ELeave) CContactoSMS20(csUserID->Des(), csAlias->Des(), (csPresente->Des() == _L8("AVAILABLE")));
		
		// añadimos a la lista
		iContactList.Append(contacto);

		// cortamos la parte que ya hemos tratado		
		TInt pos = iResponseT->Des().Find(_L8("</Presence>"));
		iResponseT->Des().Copy(iResponseT->Des().Right(csContacts.Length() - pos - 11));
		
		// buscamos siguiente
		csContact = FindValue(iResponseT->Des(), _L8("NickName"));		
		}
		
		return iContactList;
	}
	
// ----------------------------------------------------------------------------
// CSMS20::GetMsg()
//
// parses a string to get the message
// ----------------------------------------------------------------------------	
HBufC8* CSMS20::GetMsg(const TDesC8& csDatos)
	{
		HBufC8* csReturn;
		
		HBufC8* csUserID = FindValue(csDatos,_L8("Sender"));
		csUserID = FindValue(csUserID->Des(),_L8("UserID"));
		
		// sacamos solo el numero de tlf
		csUserID->Des().Copy(csUserID->Des().Mid(3, 9));
		
		HBufC8* csMsg = FindValue(csDatos,_L8("ContentData"));
		
		// limpiamos formato
		TInt pos = csMsg->Des().Find(_L8("\">"));
		if (pos != KErrNotFound)
		{
		csMsg->Des().Copy(csMsg->Des().Mid(pos + 2, csMsg->Length() - pos - 12));
		}
		
		csReturn = HBufC8::NewL(csUserID->Length() + csMsg->Length() + 3);
		
		csReturn->Des().Copy(csUserID->Des());		
		csReturn->Des().Append(_L8(" | "));
		csReturn->Des().Append(csMsg->Des());
		
		return csReturn;
	}
	
// ----------------------------------------------------------------------------
// CSMS20::GetUser()
//
// parses a string to get the contact
// ----------------------------------------------------------------------------	
HBufC8* CSMS20::GetUser(const TDesC8& csDatos)
	{
		HBufC8* csUserID = FindValue(csDatos,_L8("UserID"));
		
		// sacamos solo el numero de tlf
		csUserID->Des().Copy(csUserID->Des().Mid(3, 9));
		
		return csUserID;
	}
		
// end of file