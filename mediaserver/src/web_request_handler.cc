/*  web_request_handler.cc - this file is part of MediaTomb.
                                                                                
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

#include "config_manager.h"
#include "mem_io_handler.h"
#include "web_request_handler.h"
#include "web/pages.h"
#include "session_manager.h"

using namespace zmm;
using namespace mxml;

#define DEFAULT_PAGE_BUFFER_SIZE 4096

WebRequestHandler::WebRequestHandler() : RequestHandler()
{
    
}

String WebRequestHandler::param(String name)
{
    return params->get(name);
}

void WebRequestHandler::check_request()
{
    // we have a minimum set of parameters that are "must have"

    // check if the session parameter was supplied and if we have
    // a session with that id
    String sid = param("sid");
    if (sid == nil)
    {
        throw Exception(String("no session id given"));
    }
    if (SessionManager::getInstance()->getSession(sid) == nil)
    {
        throw Exception(String("invalid session id"));
    }

    // check the driver parameter, can't live without it
    // there can only be two active drivers at the same time
    String driver = param("driver");
    if ((driver != "1") && (driver != "2"))
    {
        if (driver == nil) driver = "nil";
        throw Exception(String("Invalid driver selected: ") + driver);
    }
}

String WebRequestHandler::renderXMLHeader(String xsl_link)
{
    if (xsl_link == nil)
    {
        return String("<?xml version=\"1.0\" encoding=\"")+
            DEFAULT_INTERNAL_CHARSET +"\"?>\n";
    }
    else
    {
        return String("<?xml version=\"1.0\" encoding=\"")+ DEFAULT_INTERNAL_CHARSET +"\"?>\n" +
                      "<?xml-stylesheet type=\"text/xsl\" href=\"" + xsl_link +
                      "\"?>\n";
    }
}

void WebRequestHandler::get_info(IN const char *filename, OUT struct File_Info *info)
{
    info->file_length = -1; // length is unknown
    info->last_modified = time(NULL);
    info->is_directory = 0; 
    info->is_readable = 1;
    String content_type = String(MIME_TYPE_XML) + "; charset=" + DEFAULT_INTERNAL_CHARSET;
    info->content_type = ixmlCloneDOMString(content_type.c_str());
}

Ref<IOHandler> WebRequestHandler::open(Ref<Dictionary> params, IN enum UpnpOpenFileMode mode)
{
    this->params = params;

    // creating the output buffer
    out = Ref<StringBuffer>(new StringBuffer(DEFAULT_PAGE_BUFFER_SIZE));

    // processing page, creating output
    try
    {
        process();
    }
    catch (Exception e)
    {
        e.printStackTrace();
        out->clear();
        Ref<Dictionary> par(new Dictionary());
        par->put("message", e.getMessage());
        *out << subrequest("error", par);
    }

    // returning output
    Ref<MemIOHandler> io_handler(new MemIOHandler(out->toString()));
    io_handler->open(mode);
    return RefCast(io_handler, IOHandler);
}

Ref<IOHandler> WebRequestHandler::open(IN const char *filename,
                                       IN enum UpnpOpenFileMode mode)
{
    this->filename = String((char *)filename);
    this->mode = mode;

    String path, parameters;
    split_url(filename, path, parameters);

    Ref<Dictionary> params = Ref<Dictionary>(new Dictionary());
    params->decode(parameters);
    return open(params, mode);
}

String WebRequestHandler::subrequest(String req_type, Ref<Dictionary> params)
{
    Ref<WebRequestHandler> subhandler(create_web_request_handler(req_type));
    Ref<IOHandler> io_handler = subhandler->open(params, mode);
    
    Ref<StringBuffer> buffer(new StringBuffer(DEFAULT_PAGE_BUFFER_SIZE));
    char *buf = (char *)malloc(DEFAULT_PAGE_BUFFER_SIZE * sizeof(char));
    while (true)
    {
        int bytesRead = io_handler->read(buf, DEFAULT_PAGE_BUFFER_SIZE);
        if (bytesRead <= 0)
            break;
        *buffer << String(buf, bytesRead);
    }
    free(buf);
    return buffer->toString();
}


// this method should be overridden
void WebRequestHandler::process()
{

}


