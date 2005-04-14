/*  action_request.cc - this file is part of MediaTomb.
                                                                                
    Copyright (C) 2005 Gena Batyan <bgeradz@deadlock.dhs.org>,
                       Sergey Bostandzhyan <jin@deadlock.dhs.org>
                                                                                
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
                                                                                
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                                
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "action_request.h"

using namespace zmm;
using namespace mxml;

ActionRequest::ActionRequest(Upnp_Action_Request *upnp_request) : Object()
{
    this->upnp_request = upnp_request;

    errCode = UPNP_E_SUCCESS;
    actionName = String(upnp_request->ActionName);
    UDN = String(upnp_request->DevUDN);
    serviceID = String(upnp_request->ServiceID);

    DOMString cxml = ixmlPrintDocument(upnp_request->ActionRequest);
    String xml(cxml);
    ixmlFreeDOMString(cxml);
   
    Ref<Parser> parser(new Parser());

    request = parser->parseString(xml);
}

String ActionRequest::getActionName()
{
    return actionName;
}
String ActionRequest::getUDN()
{
    return UDN;
}
String ActionRequest::getServiceID()
{
    return serviceID;
}
Ref<Element> ActionRequest::getRequest()
{
    return request;
}

void ActionRequest::setResponse(Ref<Element> response)
{
    this->response = response;
}
void ActionRequest::setErrorCode(int errCode)
{
    this->errCode = errCode;
}

void ActionRequest::update()
{
    if(response != nil)
    {
        String xml = response->print();
        int ret;

        //printf("ActionRequest::update(): \n%s\n\n", xml.c_str());
        
        ret = ixmlParseBufferEx(xml.c_str(), &upnp_request->ActionResult);
        if (ret != IXML_SUCCESS)
        {
            printf("ActionRequest::update(): could not convert to iXML\n");
            upnp_request->ErrCode = UPNP_E_ACTION_FAILED;    
        } 
        else
        {
            printf("ActionRequest::update(): converted to iXML, code %d\n", errCode);
            upnp_request->ErrCode = errCode;    
        }
    }
    else
    {
        // ok, here there can be two cases
        // either the function below already did set an error code,
        // then we keep it
        // if it did not do so - we set an error code of our own
        if (errCode == UPNP_E_SUCCESS) 
        {   
            upnp_request->ErrCode = UPNP_E_ACTION_FAILED;
        }
        
        printf("ActionRequest::update(): response is nil, code %d\n", errCode);
    }
}


