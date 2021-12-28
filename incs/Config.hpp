#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

#include "Server.hpp"
#include "webserv.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <arpa/inet.h>

class Config {

	public:
		/* Constructor & Destructor */
		Config(void);
		~Config(void);
		Config(Config &);
		Config &	operator=(Config &);

		/* Main func */
		char		parse(Server &server, std::ifstream & file, int &line_count);

		class Error : public std::exception {
			private:
				std::string	_msg;

			public:
				Error(std::string error_msg, int line) {
					_msg = "Line " + to_string(line) + ": " + error_msg;
				};
				~Error() throw() {};

 				virtual const char *what() const throw() {
					return (_msg.c_str());
        		};
        };

	private:
		Server	*server;

		/* Set (server config) */
		char		set_listen(std::string &content, int line_count);
		char		set_error_pages(std::string &content, int line_count);
		char		set_server_names(std::string &content, int line_count);
		char		set_client_max_body_size(std::string &content, int line_count);
		char		set_root(std::string &content, int line_count);
		char		set_location(std::ifstream & file, std::string &content, int & line_count);

		/* Set (location config) */
		s_location	get_default_location(void);
		char		set_location_error_pages(std::string &content, std::map<int, std::string> &error_pages, int line_count);
		char		set_location_root(std::string &content, std::string &root, int line_count);
		char		set_location_client_max_body_size(std::string &content, unsigned int &client_max_body_size, int line_count);
		char		set_autoindex(std::string &content, bool & autoindex, int line_count);
		char		set_cgi_extension(std::string &content, std::vector<std::string> &cgi_extension, int line_count);
		char		set_method(std::string &content, std::vector<std::string> &method, int line_count);
		char		set_cgi_path(std::string &content, std::string &cgi_path, int line_count);
		char		set_auth_basic(std::string &content, std::string &auth_basic, int line_count);
		char		set_auth_basic_user_file(std::string &content, std::string &auth_basic_user_file, int line_count);
		char		set_index(std::string &content, std::vector<std::string> &index, int line_count);

		/* Usuful func / Delete this! */
		char	check_ip(void) const;
};

#endif
