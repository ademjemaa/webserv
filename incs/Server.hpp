#ifndef SERVER_H
# define SERVER_H

# include "Connections.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server {

	public:
		
		/* Constructor & Destructor */
		Server();
		Server(const Server &c);
		Server& operator=(const Server &c);
		~Server();

		/* I don't know */
		void	respond(int fd, Request &request);

		/* Get */
		s_config									get_config() const;
		std::map<int, std::string>					get_error_pages() const;
		std::vector<std::string>					&get_names();
		std::vector<s_location>						&get_locations();
		std::string									get_host() const;
		std::string									get_root() const;
		unsigned int								get_port() const;
		int											get_fd() const;
		unsigned int								get_client_max_body_size() const;

		/* Set */
		char	set_error_pages(int, std::string);
		char	set_names(std::string);
		char	set_locations(s_location &);
		char	set_host(std::string);
		char	set_root(std::string);
		char	set_port(unsigned int);
		char	set_client_max_body_size(unsigned int);
		char	set_fd(int);

		/* Usuful func */
		void	print(void);

	private:
		/* Server config */
		
		std::map<int, std::string>					_error_pages;
		std::vector<std::string>					_names;
		std::vector<s_location>						_locations;
		std::string									_host;
		std::string									_root;
		unsigned int								_port;
		unsigned int								_client_max_body_size;
		int											_fd;
};

char	parse(std::vector<Server> &servers, char *path);



#endif
