#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include "Request.hpp"
# include "Cgi.hpp"

class Response
{
private:
	
	Request	&							_req;
	std::string							_method;
	std::string 						_path;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	int									_ret_code;
	std::map<std::string, void (Response::*)(void)> _methods;


public:
	Response(Request &req);
	Response(const Response &c);
	Response& operator=(const Response &c);
	~Response();

	void    										init_response(void);
	void											print_Response(void);
	void											get_method(void);
	void											post_method(void);
	void											delete_method(void);
	void											generate_error_page(int);
	void    										generate_raw_response(void);
	void											generate_index(void);
	std::map<std::string, void (Response::*)(void)>	methods_map(void);

	Request &							get_request(void) const {return _req;};
	std::map<std::string, std::string>	get_headers(void) const {return _headers;};
	std::string							get_methodd(void) const {return _method;};
	std::string							get_path(void) const {return _path;};
	std::string							get_body(void) const {return _body;};
	int									get_ret_code(void) const {return _ret_code;};

	std::string							_raw_response;

};

#endif