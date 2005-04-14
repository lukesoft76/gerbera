/*  upnp_cm_actions.cc - this file is part of MediaTomb.
                                                                                
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

#include "upnp_cm.h"
#include "storage.h"

using namespace zmm;
using namespace mxml;

void ConnectionManagerService::upnp_action_GetCurrentConnectionIDs(Ref<ActionRequest> request)
{
    printf("upnp_action_GetCurrentConnectionIDs: start\n");

    Ref<Element> response;
    response = UpnpXML_CreateResponse(request->getActionName(), serviceType);
    response->appendTextChild("ConnectionID", "0");

    request->setResponse(response); 
    request->setErrorCode(UPNP_E_SUCCESS);    
   
    printf("upnp_action_GetCurrentConnectionIDs: end\n");
}

void ConnectionManagerService::upnp_action_GetCurrentConnectionInfo(Ref<ActionRequest> request)
{
    printf("upnp_action_GetCurrentConnectionInfo: start\n");

    request->setErrorCode(UPNP_E_NOT_EXIST);

    printf("upnp_action_GetCurrentConnectionInfo: end\n");
}

void ConnectionManagerService::upnp_action_GetProtocolInfo(Ref<ActionRequest> request)
{
    printf("upnp_GetProtocolInfo: start\n");

    Ref<Element> response;
    response = UpnpXML_CreateResponse(request->getActionName(), serviceType);

    Ref<Array<StringBase> > mimeTypes = Storage::getInstance()->getMimeTypes();
    String CSV = mime_types_to_CSV(mimeTypes);

    response->appendTextChild("Source", CSV);
    response->appendTextChild("Sink", "");

    request->setResponse(response);
    request->setErrorCode(UPNP_E_SUCCESS);
        
    
    printf("upnp_GetProtocolInfo: end\n");
}

void ConnectionManagerService::process_action_request(Ref<ActionRequest> request)
{
    printf("ConnectionManagerService::process_action_request: start\n");

    if (request->getActionName() == "GetCurrentConnectionIDs")
    {
        upnp_action_GetCurrentConnectionIDs(request);
    }
    else if (request->getActionName() == "GetCurrentConnectionInfo")
    {
        upnp_action_GetCurrentConnectionInfo(request);
    }
    else if (request->getActionName() == "GetProtocolInfo")
    {
        upnp_action_GetProtocolInfo(request);
    }
    else
    {
        // invalid or unsupported action
        printf("process_action_request: unrecognized action %s\n", 
                request->getActionName().c_str());
        request->setErrorCode(UPNP_E_INVALID_ACTION);
        throw UpnpException(UPNP_E_INVALID_ACTION, "unrecognized action");
    }
    

    
    printf("ConnectionManagerService::process_action_request: end\n");

}


