#include "Server.hpp"

Server::Server(void) {
	_port = 8080;
	_host = "127.0.0.1";
	_root = "";
	_client_max_body_size = 536870912;
}

Server::Server(const Server &c) {
	*this = c;
}

Server& Server::operator=(const Server &c) {
	if (this != &c)
	{
		_error_pages = c._error_pages;
		_names = c._names;
		_locations = c._locations;
		_host = c._host;
		_root = c._root;
		_port = c._port;
		_client_max_body_size = c._client_max_body_size;
		_fd = c._fd;
	}
	return *this;
}

Server::~Server(void) 
{
}

void Server::respond(int fd, Request &request) 
{
	Response	res(request);
	std::string  resp;

	if (res._raw_response.size() > 200)
	 	std::cout << res._raw_response.substr(0, 197) + "..." << std::endl;
	else
		MSG(DFL, res._raw_response);

	send(fd, res._raw_response.c_str(), res._raw_response.length(), 0);
}

/* Get */
std::vector<s_location>						&Server::get_locations() {return _locations;}
std::vector<std::string>					&Server::get_names() {return _names;}
std::map<int, std::string>					Server::get_error_pages() const {return _error_pages;}
std::string									Server::get_host() const {return _host;}
std::string									Server::get_root() const {return _root;}
unsigned int								Server::get_port() const {return _port;}
int											Server::get_fd() const {return _fd;}
unsigned int								Server::get_client_max_body_size() const {return _client_max_body_size;}

s_config									Server::get_config() const
{
	s_config	tmp;

	tmp.client_max_body_size = _client_max_body_size;
	tmp.error_pages = _error_pages;
	tmp.fd = _fd;
	tmp.host = _host;
	tmp.locations = _locations;
	tmp.names = _names;
	tmp.port = _port;
	tmp.root = _root;
	return (tmp);
}

/* Set */
char	Server::set_error_pages(int id, std::string path) {
	_error_pages[id] = path;
	return (0);	
}

char	Server::set_names(std::string name) {
	_names.push_back(name);
	return (0);	
}

char	Server::set_locations(s_location &location) {
	_locations.push_back(location);
	return (0);	
}

char	Server::set_host(std::string host) {
	_host = host;
	return (0);	
}

char	Server::set_root(std::string root) {
	_root = root;
	return (0);	
}

char	Server::set_port(unsigned int port) {
	_port = port;
	return (0);
}

char	Server::set_fd(int fd) {
	_fd = fd;
	return (0);
}

char	Server::set_client_max_body_size(unsigned int client_max_body_size)
{
	_client_max_body_size = client_max_body_size;
	return (0);	
}

/* Usuful func */
void	Server::print(void) {
	TITLE(BLUE, " ------ SERVER INFORMATIONS -------");

	/* NAMES */
	INFO(BLUE, "server_names", "", "");
	for (std::vector<std::string>::iterator it = _names.begin(); it !=_names.end(); it++) {
		std::cout << YELLOW << *it;
		if (it != _names.end() - 1)
			std::cout << ", ";
		else
			std::cout << std::endl;
	}

	/* ERROR PAGES */
	INFO(BLUE, "error_pages", "", "");
	for (std::map<int, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++) {
		std::cout << "[" << it->first << "] --> " << it->second << std::endl;
	}
	INFO(BLUE, "host", YELLOW, _host);
	INFO(BLUE, "port", YELLOW, _port);
	INFO(BLUE, "root", YELLOW, (_root.empty() ? "null" : _root));
	INFO(BLUE, "client_max_body_size", YELLOW, _client_max_body_size);

	/* LOCATION(S) */
	SPACE TITLE(BLUE, " -----------  LOCATIONS -----------");
	for (std::vector<struct s_location>::iterator it = _locations.begin(); it != _locations.end(); it++) {
		std::cout  << std::boolalpha << std::endl;
		INFO(BLUE, "path", YELLOW, it->path);
		INFO(BLUE, "root", YELLOW, (it->root.empty() ? "null" : it->root));
		INFO(BLUE, "cgi_path", YELLOW, (it->cgi_path.empty() ? "null" : it->cgi_path));
		INFO(BLUE, "upload_path", YELLOW, (it->upload_path.empty() ? "null" : it->upload_path));
		INFO(BLUE, "auth_basic_user_file", YELLOW, (it->auth_basic_user_file.empty() ? "null" : it->auth_basic_user_file));
		INFO(BLUE, "auth_basic", YELLOW, (it->auth_basic.empty() ? "null" : it->auth_basic));
		INFO(BLUE, "client_max_body_size", YELLOW, it->client_max_body_size);
		INFO(BLUE, "upload_eanable", YELLOW, it->upload_eanable);
		INFO(BLUE, "autoindex", YELLOW, it->autoindex);

		/* METHODES */
		INFO(BLUE, "methods", "", "");
		for (std::vector<std::string>::iterator m = it->methods.begin(); m != it->methods.end(); m++) {
			std::cout << *m << ", ";
		}	SPACE

		/* CGI EXTENSION */
		INFO(BLUE, "cgi_extension", "", "");
		for (std::vector<std::string>::iterator m = it->cgi_extension.begin(); m != it->cgi_extension.end(); m++) {
			std::cout << *m << ", ";
		}	SPACE

		/* INDEX PAGES */
		INFO(BLUE, "index", "", "");
		for (std::vector<std::string>::iterator m = it->index.begin(); m != it->index.end(); m++) {
			std::cout << *m << ", ";
		}	SPACE

		/* ERROR PAGES */
		INFO(BLUE, "error_pages", "", "");
		for (std::map<int, std::string>::iterator m = it->error_pages.begin(); m != it->error_pages.end(); m++) {
			std::cout << "[" << m->first << "]" << m->second << std::endl;
		}	SPACE std::cout << LINE;
	}	SPACE
}