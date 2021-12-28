#include "Request.hpp"

Request::Request()  : _status(START)
{
	_chunk.status = Request::UNCHUNKED;
}

Request::Request(const Request &req)
{
	*this = req;
}

Request& Request::operator=(const Request &c)
{
	if (this != &c)
	{
		_raw_request = c._raw_request;
		_conf = c._conf;
		_status = c._status;
		_method = c._method;
		_version = c._version;
		_uri = c._uri;
		_headers = c._headers;
		_body = c._body;
		_lan = c._lan;
		_ret_code = c._ret_code;
		_chunk = c._chunk;
	}
	return *this;
}

Request::~Request()
{
}

void		Request::check_parsing(void)
{
	_ret_code = 200;
	// the minimum required information is METHOD, RESOURCE, PROTOCOL VERSION and at least the Host header
	if (_method.length() == 0 || (_method != "GET" && _method != "POST"
		&& _method != "HEAD" && _method != "PUT"
		&& _method != "CONNECT" && _method != "TRACE"
		&& _method != "OPTIONS" && _method != "DELETE"))
		_ret_code = 400;

	else if (_method != "GET" && _method != "POST"
			&& _method != "DELETE")
		_ret_code = 501;

	if (_uri.length() == 0 || _uri[0] != '/')
		_ret_code = 400;

	if (_version.length() == 0 || (_version != "HTTP/1.1\r" && _version != "HTTP/1.1"))
		_ret_code = 400;

	if (_headers.find("Host") == _headers.end())
		_ret_code = 400;

	std::vector<std::string>::iterator it = find(_conf.methods.begin(), _conf.methods.end(), _method);
	if (it == _conf.methods.end())
		_ret_code = 405;
}

void	Request::parse_body(u_long len_content)
{
	if (_headers.find("Content-Length") != _headers.end())
	{
		unsigned long length = atoi(_headers.find("Content-Length")->second.c_str());

		if (len_content == length)
		{
			_body += _raw_request;
			_ret_code = 200;

		}
		else
			_ret_code = 400;
		_status = Request::DONE;
	}
	else if (_headers.find("Transfer-Encoding") != _headers.end())
	{
		parse_chunked_body();
	}
	else if (_raw_request.size() != 0)
		_ret_code = 411;
}

void		Request::parse_chunked_body(void)
{
	if (_chunk.status == Request::UNCHUNKED)
			_chunk.status = Request::BEGIN;
		while (_raw_request.size() != 0 && (_chunk.status != Request::FINISHED))
		{
			switch (_chunk.status)
			{
				case Request::BEGIN :
				{
					std::stringstream strm;
					unsigned int count;

					strm.str("");
					strm << std::hex << cut_line(_raw_request, true, 0);
					strm >> count;

					if (count == 0)
					{
						_chunk.status = Request::FINISHED;
						_status = Request::DONE;
					}
					else
					{
						_chunk.status = Request::FOUND;
						_chunk.length = count;
					}
					break;
				}
				case Request::FOUND :
					if (_raw_request.size() >= (unsigned long)_chunk.length)
					{
						_body += _raw_request.substr(0, _chunk.length);
						_raw_request.erase(0, _chunk.length + 2);
						_chunk.status = Request::BEGIN;
					}
					else
					{
						_body += _raw_request;
						_chunk.length -= _raw_request.size();
						_raw_request.erase(_raw_request.begin(), _raw_request.end());
						_chunk.status = Request::FOUND;
					}
					break;
			}
		}
}

void		Request::parse_language(void)
{
	if (_headers.find("Accept-Language") != _headers.end())
	{
		std::string		current;

		current = _headers["Accept-Language"];

		if (current.find(',') == std::string::npos)
			_lan = "en-US";
		else
		{
			while (current.find(',') != std::string::npos)
			{
				_lan +=	current.substr(0, current.find(',') + 1);
				if (_lan.find(';') != std::string::npos)
					_lan.erase(_lan.find(';'), _lan.length());
				current.erase(0, current.find(',') + 1);
			}
			_lan += current;
			if (_lan.find(';') != std::string::npos)
					_lan.erase(_lan.find(';'), _lan.length());
		}
	}
}

void		Request::parse_config(s_config conf)
{
	std::string		match = "";
	s_location		tmp;

	for (unsigned long i = 0; i < conf.locations.size(); i++)
	{
		if (conf.locations[i].path == _uri)
		{
			match = conf.locations[i].path;
			tmp = conf.locations[i];
			break ;
		}
		if (_uri.find(conf.locations[i].path) != std::string::npos && _uri.find(conf.locations[i].path) == 0)
		{
			if (conf.locations[i].path.length() > match.length())
			{
				match = conf.locations[i].path;
				tmp = conf.locations[i];
			}
		}
	}
	_conf.error_pages = tmp.error_pages;
	_conf.path = tmp.path;
	_conf.error_pages.insert(conf.error_pages.begin(), conf.error_pages.end());
	_conf.names = conf.names;
	_conf.host = conf.host;
	_conf.root = (tmp.root != "") ? tmp.root : conf.root;
	
	if (tmp.methods.size() == 0)
	{
		_conf.methods.push_back("GET");
		_conf.methods.push_back("POST");
		_conf.methods.push_back("DELETE");
	}
	else
		_conf.methods = tmp.methods;
	_conf.cgi_extension = tmp.cgi_extension;
	_conf.index = tmp.index ;
	_conf.cgi_path = tmp.cgi_path;
	_conf.upload_path = tmp.upload_path;
	_conf.auth_basic_user_file = tmp.auth_basic_user_file;
	_conf.auth_basic = tmp.auth_basic;
	_conf.upload_eanable = tmp.upload_eanable;
	_conf.autoindex = tmp.autoindex;
	_conf.client_max_body_size = tmp.client_max_body_size;
	_conf.port = conf.port;
}

void		Request::parse(std::string str, s_config conf, u_long len_content)
{
	std::string		line;
	std::string		key;
	std::string		value;

	_raw_request = str;
	if (_status == Request::START)
	{
		line = cut_line(_raw_request, true, 0);
		_method = line.substr(0, line.find(" "));
		line.erase(0, _method.length() + 1);
		_uri = line.substr(0, line.find(" "));
		line.erase(0, _uri.length() + 1);
		_version = line.substr(0, line.find(" "));
		line = cut_line(_raw_request, true, 0);
		_status = Request::HEADERS;
		parse_config(conf);
		print_config();
	}
	while (_status == Request::HEADERS && line.find(":") != std::string::npos)
	{
		// ATTENTION POTENSIELLE SEGFAULT SUBSTR NULL ? 
		key = line.substr(0, line.find(":"));
		value = line.substr(line.find(":") + 2);
		_headers[key] = value;
		line = cut_line(_raw_request, true, 0);
	}
	_status = Request::BODY;
	parse_language();
	check_parsing();
	if (_method == "POST" && _status == Request::BODY && _ret_code < 400)
		parse_body(len_content);
	else
		_status = Request::DONE;
}

void	Request::print_request(void)
{
	TITLE(YELLOW, " -----         REQUEST        ----- ");
	INFO(BLUE, "method", YELLOW, _method);
	INFO(BLUE, "URI", YELLOW, _uri);
	INFO(BLUE, "version", YELLOW, _version);
	INFO(BLUE, "languages", YELLOW, _lan);

	/* HEADER */
	SPACE MSG(BLUE, " -----  HEADER  -----");
	std::map<std::string, std::string>::iterator it = _headers.begin();
	for (; it != _headers.end(); ++it)
		std::cout << it->first << " : " << it->second << std::endl;
		
	/* BODY */
	SPACE MSG(BLUE, " -----   BODY   -----");
	// MSG(YELLOW, _body); ?? il est print ou le BODY en vrai ? 
}

void	Request::print_config(void)
{
	TITLE(YELLOW, " -----     REQUEST CONFIG     ----- ");

	MSG(BLUE, "error pages:");
	for (std::map<int, std::string>::iterator it = _conf.error_pages.begin(); it != _conf.error_pages.end(); it++)
		std::cout << it->first << " " << it->second << std::endl;

	MSG(BLUE, "names:");
	for (std::vector<std::string>::iterator it = _conf.names.begin(); it != _conf.names.end(); it++)
		std::cout << YELLOW << *it << " ";

	SPACE INFO(BLUE, "host", YELLOW, _conf.host);
	INFO(BLUE, "root", YELLOW, _conf.root);

	MSG(BLUE, "methods:");
	for (std::vector<std::string>::iterator it = _conf.methods.begin(); it != _conf.methods.end(); it++)
		std::cout << *it << " ";

	SPACE MSG(BLUE, "cgi extension:");
	for (std::vector<std::string>::iterator it = _conf.cgi_extension.begin(); it != _conf.cgi_extension.end(); it++)
		std::cout << *it << " ";

	SPACE INFO(BLUE, "cgi path", YELLOW, _conf.cgi_path);

	MSG(BLUE, "index:");
	for (std::vector<std::string>::iterator it = _conf.index.begin(); it != _conf.index.end(); it++)
		std::cout << *it << ", ";

	INFO(BLUE, "upload_path", YELLOW, _conf.upload_path);
	INFO(BLUE, "auth_basic_user_file", YELLOW, _conf.auth_basic_user_file);
	INFO(BLUE, "auth_basic", YELLOW, _conf.auth_basic);
	INFO(BLUE, "upload_eanable", YELLOW, _conf.upload_eanable);
	INFO(BLUE, "auto index", YELLOW, _conf.autoindex);
	INFO(BLUE, "client body size", YELLOW, _conf.client_max_body_size);
	SPACE
} 