#ifndef SOCKETS_HPP
# define SOCKETS_HPP

# include "webserv.hpp"

# include "Request.hpp"
# include "Client.hpp"
# include "Server.hpp"

class Server;
class Client;

class Connections
{
	private:
		fd_set	active_set;
		fd_set	ready_rset;
		int		ready_fd;
		int		max_fd;
		

	public:
		Connections();
		Connections(const Connections &c);
		Connections& operator=(const Connections &c);
		~Connections();

		int init();
		int add_client(Server &server);
		int check_clients();
		void	loop();

		std::list<int> fd_list;
		std::vector<Client> clients;
		std::vector<Server> servers;

		void	ft_clear_clients( void );
		void	ft_clear_servers( void );
};

#endif
