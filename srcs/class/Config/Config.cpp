#include "Config.hpp"

/* Constructor & Destructor */
Config::Config(void) {
	return ;
}

Config::Config(Config & src) {
	*this = src;
	return ;
}

Config &	Config::operator=(Config &src) {
	(void)src;
	return (*this);
}

Config::~Config(void) {
	return ;
}


char	Config::check_ip(void) const {
	struct sockaddr_in sa;
	return (!inet_pton(AF_INET, server->get_host().c_str(), &(sa.sin_addr)));
}

/* Set */
char	Config::set_listen(std::string &content, int line_count) {
	size_t			pos;
	unsigned int	expected_port;
	
	content.erase(0, 6);
	remove_extra_space(content, 0);
	if (content.substr(0, content.size() - 1).empty())
		throw Error("no content.", line_count);

	pos = content.find(":");
	if (pos != std::string::npos) {
		expected_port = std::atoi(content.substr(pos + 1, content.length()).c_str());
		if (expected_port <= 0 || expected_port > 65535)
			throw Error("The port used is not valid.", line_count);
		server->set_port(expected_port);
		server->set_host(content.substr(0, pos));
	}
	else
		server->set_host(content);

	if (server->get_host() == std::string("*"))
		server->set_host(std::string("0.0.0.0"));
	else if (server->get_host() == std::string("localhost"))
		server->set_host(std::string("127.0.0.1"));
	
	if (check_ip())
		throw Error("The ip address is invalid.", line_count);
	content.erase();
	return (0);
}

char	Config::set_error_pages(std::string &content, int line_count) {
	std::string	path;
	std::string	error_expected;
	size_t		error;
	size_t		pos;

	error = 0;
	content.erase(0, 10);
	remove_extra_space(content, 0);
	if (content.substr(0, content.size() - 1).empty())
		return (1);

	pos = content.find_first_of(" ");
	if (pos == std::string::npos)
		return (1);

	error_expected = content.substr(0, pos);
	if (error_expected.find_first_not_of("0123456789") != std::string::npos)
		throw Error("The error is invalid.", line_count);
	error = std::atoi(error_expected.c_str());

	content.erase(0, pos);
	remove_extra_space(content, 0);
	if (content.empty())
		throw Error("No error related path is used.", line_count);

	if (content[0] != '/' || content.substr(content.length() - 6, 5) != std::string(".html"))
		throw Error("The path must be absolute and must end in .html.", line_count);
	server->set_error_pages(error, content.substr(0, content.length() - 1));

	return (0);
}

char	Config::set_server_names(std::string &content, int line_count) {
	std::string name;
	size_t 		pos;

	content.erase(0, 11);
	remove_extra_space(content, 0);
	if (content.substr(0, content.size() - 1).empty())
		throw Error("You must specify a name.", line_count);

	pos = content.find_first_of(" ");
	while (pos != std::string::npos) {
		server->set_names(content.substr(0, pos));
		content.erase(0, pos);
		remove_extra_space(content, 0);
		pos = content.find_first_of(" ");
	}
	server->set_names(content.substr(0, content.length() - 1));
	content.erase();
	return (0);
}

char	Config::set_client_max_body_size(std::string &content, int line_count) {
	content.erase(0, 20);
	remove_extra_space(content, 0);
	if (content.substr(0, content.size() - 1).empty())
		throw Error("You must specify a client max body size.", line_count);
	if (content.find_first_not_of("0123456789MmGgKk;") != std::string::npos)
		throw Error("Bad format.", line_count);
	server->set_client_max_body_size(static_cast<size_t>(std::atol(content.c_str())));
	if (content[content.length() - 2] == 'G' || content[content.length() - 2] == 'g')
		server->set_client_max_body_size(server->get_client_max_body_size() * 1000000000);
	else if (content[content.length() - 2] == 'M' || content[content.length() - 2] == 'm')
		server->set_client_max_body_size(server->get_client_max_body_size() * 1000000);
	else if (content[content.length() - 2] == 'K' || content[content.length() - 2] == 'k')
		server->set_client_max_body_size(server->get_client_max_body_size() * 1000);
	content.erase();
	return (0);
}

char	Config::set_root(std::string &content, int line_count) {
	char	tmp[256];

	content.erase(0, 4);
	remove_extra_space(content, 0);
	if (content.substr(0, content.size() - 1).empty())
		throw Error("You must specified a path for the root.", line_count);

	if (!server->get_root().empty())
		throw Error("Root already specified.", line_count);
	
	if (content[0] != '/')
		throw Error("Only absolute path accepted.", line_count);
	getcwd(tmp, 256);
	server->set_root(tmp + content.substr(0, content.length() - 1));
	content.erase();
	return (0);
}

s_location	Config::get_default_location(void) {
	s_location	loc;

	loc.path = "/";
	loc.root = "";
	loc.autoindex = false;
	loc.upload_eanable = false;
	loc.cgi_path = "";
	loc.auth_basic = "";
	loc.auth_basic_user_file = "";
	loc.client_max_body_size = 536870912;

	return (loc);
}

/* PARSE LOCATION */
char	Config::set_location(std::ifstream &file, std::string &content, int &line_count) {
	s_location		location = get_default_location();
	std::string		path;
	size_t			pos;

	content.erase(0, 8);
	if (content.empty())
		throw Error("No path.", line_count);
	remove_extra_space(content, 0);
	if (content[0] != '/')
		throw Error("Only aboslute path accepted.", line_count);
	pos = content.find_first_of(" ");
	if (pos == std::string::npos)
		throw Error("Missing bracket.", line_count);
	path = content.substr(0, pos);
	for (std::vector<s_location>::iterator it = server->get_locations().begin(); it < server->get_locations().end(); it++) {
		if ((*it).path == path) {
			throw Error("Duplicate location path.", line_count);	
		}
	}
	location.path = path;
	content.erase(0, pos);
	remove_extra_space(content, 0);
	if (content.empty() || content[0] != '{')
		return (1);
	while (std::getline(file, content)) {
		remove_extra_space(content, 0);
		if (content.empty() || content[0] == '#') {
			line_count++;
			continue;
		}
		line_count++;

		if (content[content.length() - 1] != ';' && content[0] != '}')
			throw Error("semicolon required at the end of the line.", line_count);
		if (count_char_in_string(content, ';') > 1)
			throw Error("There is more than one semicolon at the end of the line.", line_count);
		
		std::string	str = content.substr(0, content.find_first_of(" \t"));
		if ("root" == str)
			set_location_root(content, location.root, line_count);
		else if ("methods" == str)
			set_method(content, location.methods, line_count);
		else if ("autoindex" == str)
			set_autoindex(content, location.autoindex, line_count);
		else if ("index" == str)
			set_index(content, location.index, line_count);
		else if ("cgi_extension" == str)
			set_cgi_extension(content, location.cgi_extension, line_count);
		else if ("cgi_path" == str)
			set_cgi_path(content, location.cgi_path, line_count);
		else if ("client_max_body_size" == str)
			set_location_client_max_body_size(content, location.client_max_body_size, line_count);
		else if ("auth_basic_user_file" == str)
			set_auth_basic_user_file(content, location.auth_basic_user_file, line_count);
		else if ("auth_basic" == str)
			set_auth_basic(content, location.auth_basic, line_count);
		else if ("error_page" == str)
			set_location_error_pages(content, location.error_pages, line_count);
		else if (content[0] == '}') {
			server->set_locations(location);
			return (0);
		}
		else
			throw Error("Unknow keyword.", line_count);
	}
	
	return (0);
}

/* PARSE */
char	Config::parse(Server &server, std::ifstream &file, int &line_count) {
	std::string	content;
	char		bracket;

	bracket = 0;
	this->server = &server;
	while (std::getline(file, content)) {
		remove_extra_space(content, 0);
		if (content.empty() || content[0] == '#') {
			line_count++;
			continue;
		}

		line_count++;
		if (content[content.length() - 1] != ';' && content[0] != '}' && content.compare(0, 8, "location")) {
			throw Error("semicolon required at the end of the line.", line_count);
		}
		if (count_char_in_string(content, ';') > 1) {
			throw Error("There is more than one semicolon at the end of the line.", line_count);
		}
		if ("listen" == content.substr(0, content.find_first_of(" \t")))
			set_listen(content, line_count);
		else if ("error_page" == content.substr(0, content.find_first_of(" \t")))
			set_error_pages(content, line_count);
		else if ("server_name" == content.substr(0, content.find_first_of(" \t")))
			set_server_names(content, line_count);
		else if ("client_max_body_size" == content.substr(0, content.find_first_of(" \t")))
			set_client_max_body_size(content, line_count);
		else if ("root" == content.substr(0, content.find_first_of(" \t")))
			set_root(content, line_count);
		else if ("location" == content.substr(0, content.find_first_of(" \t")))
			set_location(file, content, line_count);
		else if (content[0] == '}') {
			bracket = 1;
			break;
		}
		else
			throw Error("Unknow keyword.", line_count);
	}
	if (!bracket)
		throw Error("Missing bracket.", line_count);
	
	server.print();
	return (0);
}
