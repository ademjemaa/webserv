#include "Response.hpp"

Response::Response(Request &req) : _req(req), _path(req._uri), _ret_code(req._ret_code)
{
    if (req._body.length() > req._conf.client_max_body_size)
        _ret_code = 413;
    _headers["Content-Language: "] = (req._lan.length() > 0) ? req._lan : "fr,en-USen";
    if (req._conf.names.size() > 0)
        _headers["Server: "] = req._conf.names[0];
    else
        _headers["Server: "] = "luigi's mansion";
    _headers["Host: "] = req._conf.host;
    _raw_response = req._version + " ";
    _method = req._method;
    _methods = methods_map();
    init_response();
}

Response::Response(const Response &res) : _req(res._req), _path(res._path), _ret_code(res._ret_code)
{
    *this = res;
}

Response& Response::operator=(const Response &res)
{
    if (this != &res)
    {
        _raw_response = res._raw_response;
        _ret_code = res._ret_code;
        _path = res._path;
        _method = res._method;
        _headers = res._headers;
        _body = res._body;
    }
    return *this;
}

Response::~Response()
{
}

std::map<std::string, void (Response::*)(void)>    Response::methods_map(void)
{
    std::map<std::string, void (Response::*)(void)>    map;

    map["GET"] = &Response::get_method;
    map["POST"] = &Response::post_method;
    map["DELETE"] = &Response::delete_method;

    return (map);
}

void Response::print_Response(void)
{
    std::cout << _raw_response << std::endl;
}

void    Response::init_response(void)
{
    if (_ret_code < 400)
        ((this->*_methods[_method]))();
    if (_ret_code >= 400)
        generate_error_page(_ret_code);
    generate_raw_response();
}

void    Response::generate_index(void)
{
    DIR				*dp;
	struct dirent   *dirp;

    //https://www.delftstack.com/howto/c/opendir-in-c/
	dp = opendir((_req._conf.root + _req._uri).c_str());
    if (dp == NULL)
    {
        _ret_code = 500;
        return ;
    }
    _body = "<html>\n<body>\n";
    _body += "<h2>directory listing :</h2>\n";
	while ((dirp = readdir(dp)) != NULL)
	{
		_body += "<a href=\"" + _req._uri;
		if (_req._uri[_req._uri.length() - 1] != '/')
			_body += '/';
		_body += dirp->d_name;
		_body += "\">";
		_body += dirp->d_name;
		_body += "</a>";
        _body += "<br>\n";
	}
    _body += "</body>\n</html>\n";
    _headers["Content-type: "] = MIME_types(".html");
	closedir(dp);
}

void    Response::generate_raw_response(void)
{

    _raw_response = "HTTP/1.1 " + status_code(_ret_code) + "\n";
    if (_body.length() > 0)
    {
        std::ostringstream ss;

        ss << _body.length();
        _headers["Content-Length: "] = ss.str();
    }
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        _raw_response += it->first + it->second + "\n";
    }
    _raw_response += "Date: " + get_current_time() + "\n";
    _raw_response += "\n" + _body + "\n";
}

void    Response::get_method(void)
{
    struct stat info;

    if (_req._uri.find("?") != std::string::npos)
    {
        _ret_code = 200;
        Cgi cgi(_req);
        _body += cgi.execute(_req);
        _headers["Content-type: "] = MIME_types("");
        return ;
    }
    if (_req._conf.autoindex && _req._uri[_req._uri.length() - 1] == '/' && _req._conf.index.size() <= 1)
    {
        generate_index();
        _ret_code = 200;
        return ;
    }
    if (_req._uri == _req._conf.path)
    {
        
        for (std::vector<std::string>::iterator it = _req._conf.index.begin(); it != _req._conf.index.end(); it++)
        {
            if ((*it).length() > 0)
            {
                _req._uri += *it;
                break;
            }
        }
    }
    if (stat((_req._conf.root + _req._uri).c_str(), &info) == -1)
        _ret_code = 404;
    else
    {
        if (info.st_mode & S_IRUSR)
        {
            read_html(_req._conf.root + _req._uri, _body);
            if (_body.length() > 0)
            {
                _headers["Content-type: "] = MIME_types(_req._conf.root + _req._uri);
                _headers["Last-Modified: "] = Last_modified(_req._conf.root + _req._uri);
                
                _ret_code = 200;
            }
            else
                _ret_code = 204;
        }
        else
            _ret_code = 403;
    }
}

void    Response::post_method(void)
{
    std::ofstream fd;

    if (_req._conf.cgi_path != "")
    {
        _ret_code = 200;
        Cgi cgi(_req);
        _body += cgi.execute(_req);
        _headers["Content-type: "] = MIME_types("");
    }
    else
    {
        if (file_status(_req._conf.root + _req._uri) == 403)
            _ret_code = 403;
        else
        {
            fd.open((_req._conf.root + _req._uri).c_str(), std::ofstream::out);
            _ret_code = 201;
            fd << _req._body;
            fd.close();
        }
    }
}

void    Response::delete_method(void)
{
    struct stat info;

    if (stat((_req._conf.root + _req._uri).c_str(), &info) == -1)
        _ret_code = 404;
    else
   {
        if (info.st_mode & S_IRUSR)
        {
            remove((_req._conf.root + _req._uri).c_str());
            _ret_code = 204;
        }
        else
            _ret_code = 403;
    }
}

void    Response::generate_error_page(int error_code)
{
    if (error_code == 405)
    {
        std::string     methods;
        for (std::vector<std::string>::iterator it = _req._conf.methods.begin(); it != _req._conf.methods.end(); it++)
        {
            methods += *it;
            if ((it + 1) != _req._conf.methods.end())
                methods += ", ";
        }

        _headers["Allow: "] = methods;
        if (file_status(_req._conf.root + _req._conf.error_pages[error_code]) != 200)
            _body = default_error_page(405);
        else
            read_html(_req._conf.root + _req._conf.error_pages[405], _body);
        _headers["Content-type: "] = MIME_types(_req._conf.error_pages[405]);
    }
    else
    {
        if (file_status(_req._conf.root + _req._conf.error_pages[error_code]) != 200)
        {
            std::cout << "test file not found" << std::endl;
            _body = default_error_page(error_code);
        }
        else
            read_html(_req._conf.root + _req._conf.error_pages[error_code], _body);
        _headers["Content-type: "] = MIME_types(_req._conf.error_pages[error_code]);
    }
}