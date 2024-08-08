/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minakim <minakim@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:23:00 by sanghupa          #+#    #+#             */
/*   Updated: 2024/08/08 22:00:37 by minakim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "Context.hpp"

////////////////////////////////////////////////////////////////////////////////

StaticFileHandler::StaticFileHandler()
	: _handledPath("")
{
}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other)
{
	_handledPath = other._handledPath;
}

StaticFileHandler& StaticFileHandler::operator=(const StaticFileHandler& other)
{
	if (this != &other)
	{
		_handledPath = other._handledPath;
	}
	return (*this);
}

StaticFileHandler::~StaticFileHandler()
{}

////////////////////////////////////////////////////////////////////////////////
/// Public methods: handleRequest
////////////////////////////////////////////////////////////////////////////////

/// @brief handleRequest, public method to handle the request
/// This method is called by the RequestHandler to handle the request.
/// It checks if the requested URI is a directory or a file and calls the
/// appropriate method to handle the request.
/// @param request HttpRequest object as reference
/// @param location Location object as reference
HttpResponse StaticFileHandler::handleRequest(const Context& context)
{
	this->_initMimeTypes(context);
	if (context.getRequest().getUri() == "/")
		return (_handleRoot(context));
	_setHandledPath(_buildPathWithUri(context));
	if (isDir(_handledPath))
	{
		// if (/* && location.location.isListDir() */)
		// 	return (_handleDirListing(request, location));
		return (_handleDirRequest(context));
	}
	if (isFile(_handledPath))
		return (_handleFileRequest(context));
	return (_handleNotFound(context));
}


////////////////////////////////////////////////////////////////////////////////
/// Private methods
////////////////////////////////////////////////////////////////////////////////

/// @brief _handleDirRequest, private method to handle the directory request
/// This method is called when the directory listing is not `true`,
/// but the `uri` in the request is a directory.
/// the method modifies the request to append the default file name.
/// @warning this method depends on `_handleFileRequest` method
HttpResponse StaticFileHandler::_handleDirRequest(const Context& context)
{
	HttpRequest	modifiedRequest = context.getRequest();
	Context&	moditiedContext = const_cast<Context&>(context);
	
	_setHandledPath(_buildAbsolutePathWithIndex(context));
	std::cout << "TEST |  absolute path that with index is built: " << _handledPath << std::endl;
	if (!isFile(_handledPath))
		return (_handleNotFound(context));
	modifiedRequest.setUri(getFullPath());
	moditiedContext.setRequest(modifiedRequest);
	return (_handleFileRequest(moditiedContext));
}

std::string StaticFileHandler::_buildAbsolutePathWithIndex(const Context& context) const
{
	std::string index		= INDEX_HTML;
	std::string readedIndex = context.getLocation().getIndex();
	if (!readedIndex.empty())
		index = readedIndex;
	return (_handledPath + index);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief _handleDirListing, private method to handle the directory listing
/// This method is called when the directory listing is `true`,
/// and the `uri` in the request is a directory.
/// @warning not implemented yet
HttpResponse StaticFileHandler::_handleDirListing(const Context& context)
{
	return (_createDirListingResponse(context));
}

// TODO: update this method to return a response object with the directory listing
/// @brief Create a `response` object for a directory listing.
/// @return `HttpResponse`
HttpResponse StaticFileHandler::_createDirListingResponse(const Context& context) const
{
	HttpResponse		resp(context);
	resp.setBody(genDirListingHtml(getFullPath()));

	if (resp.getBody().empty() || resp.getBodyLength() <= 0)
		return (HttpResponse::internalServerError_500(context));
	resp.setDefaultHeaders();
	return (resp);
}

/// @brief Generates an HTML page with a directory listing.
/// @param path 
/// @return Create a list of directories and files in HTML and export it to std::stringstream.
std::string	genDirListingHtml(const std::string& path)
{
	std::stringstream	html;
	std::string			body;

	html.clear();
    html << "<html><head><title>Directory Listing</title></head><body>";
    html << "<h2>Directory Listing for " << path << "</h2><ul>";
    
    // TODO: Implement directory listing

    html << "</ul></body></html>";
	body = html.str();
	return (body);
}

////////////////////////////////////////////////////////////////////////////////
HttpResponse StaticFileHandler::_handleFileRequest(const Context& context)
{
	return (_createResponseForFile(context));
}

HttpResponse StaticFileHandler::_createResponseForFile(const Context& context) const
{
	HttpResponse resp(context, _handledPath);
	resp.setHeader("Content-Type", resolveMimeType(_handledPath));
	return (resp);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief _handleRoot, private method to handle the root request
/// This method is called when the requested URI is the root directory.
/// It builds the full path of the default file and checks if the file exists.
HttpResponse StaticFileHandler::_handleRoot(const Context& context)
{
	_setHandledPath(_buildAbsolutePathWithRoot(context));
	if (!isFile(_handledPath))
		return (_handleNotFound(context));
	return (_createResponseForFile(context));
}

////////////////////////////////////////////////////////////////////////////////
std::string StaticFileHandler::_buildAbsolutePathWithRoot(const Context& context) const
{
	std::string	locationRoot	= context.getLocation().getRootPath();
	std::string serverRoot		= context.getServer().root;
	std::string	defaultFile		= context.getLocation().getIndex();

	if (locationRoot.empty())
		throw std::runtime_error("Root path is empty");
	if (defaultFile.empty()) // if can not read from cong, give default value: hard code
		defaultFile = INDEX_HTML;
	std::string fullPath = "." + serverRoot + locationRoot + "/" + defaultFile;
	std::cout << "TEST | absolute path that with index is built: " << _handledPath << std::endl;
	return (fullPath);
}

std::string StaticFileHandler::_buildPathWithUri(const Context& context) const
{
	std::string	locationRoot	= context.getLocation().getRootPath();
	std::string serverRoot		= context.getServer().root;

	if (locationRoot.empty() || serverRoot.empty())
		throw std::runtime_error("Root path is empty");
	std::string fullPath = "." + serverRoot + locationRoot + context.getRequest().getUri();

	std::cout << "TEST | first path that is built: " << _handledPath << std::endl;

	return(fullPath);
}

HttpResponse StaticFileHandler::_handleNotFound(const Context& context)
{
	return (HttpResponse::notFound_404(context));
}


////////////////////////////////////////////////////////////////////////////////
/// Setters
////////////////////////////////////////////////////////////////////////////////

void	StaticFileHandler::_setHandledPath(const std::string& fullPath)
{
	_handledPath = fullPath;
}

////////////////////////////////////////////////////////////////////////////////
/// Getters
////////////////////////////////////////////////////////////////////////////////

std::string	StaticFileHandler::getFullPath() const
{
	return (_handledPath);
}

////////////////////////////////////////////////////////////////////////////////
/// Mime types
////////////////////////////////////////////////////////////////////////////////

// FIXME: logic changed, need to updat: "context" parameter
/// @brief Initializes the MIME types map from the configuration.
/// If the configuration does not provide MIME types, default values are used.
void	StaticFileHandler::_initMimeTypes(const Context& context)
{
	(void)context;
	// ServerConfig& serverConfig = const_cast<ServerConfig&>(context.getServer());
	// if (serverConfig.getMimeTypes().size() > 0)
	// 	_staticMimeTypes = serverConfig.getMimeTypes();
	// else
	// {
		_mimeTypes.insert(std::make_pair("html", "text/html"));
		_mimeTypes.insert(std::make_pair("css", "text/css"));
		_mimeTypes.insert(std::make_pair("js", "application/javascript"));
		_mimeTypes.insert(std::make_pair("png", "image/png"));
		_mimeTypes.insert(std::make_pair("jpg", "image/jpeg"));
		_mimeTypes.insert(std::make_pair("gif", "image/gif"));
		_mimeTypes.insert(std::make_pair("txt", "text/plain"));
	// }
} 

/// @brief Returns the MIME type of a given file based on its file extension.
/// @example resolveMimeType("index.html") ext = "html", and returns "text/html"
/// @param path The path of the file.
/// @return The MIME type of the file.
std::string StaticFileHandler::resolveMimeType(const std::string path) const
{
	std::string::size_type dotPos = path.find_last_of(".");
	if (dotPos == std::string::npos)
		return ("text/plain");
	std::string ext = path.substr(dotPos + 1);
	std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
	if (it != _mimeTypes.end())
		return (it->second);
	return ("application/octet-stream");
}