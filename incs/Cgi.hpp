#ifndef __CGI_HPP__
# define __CGI_HPP__

# include <string>
# include <map>
# include <string>
# include <string.h>
# include "Request.hpp"
# include <sys/types.h>
# include <sys/wait.h>
# include <fstream>

class Cgi {

	public:
		Cgi(Request &request);
		~Cgi(void);
		Cgi(const Cgi &);
		Cgi &   operator=(const Cgi &);

		std::string execute(Request &request);

	private:
		char    	**_map_to_table_char(std::map<std::string, std::string> map);
		void    	_init_envs(Request &request);
		std::string	_read_file(std::string path);
		std::string	_get_query_string(std::string uri);

		char								**_envs;
};

#endif