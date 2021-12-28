#include "Connections.hpp"
int nb_serv = 0;

Connections::Connections() {
}

Connections::Connections(const Connections &c) {
	(void)c;
}

Connections& Connections::operator=(const Connections &c) {
	(void)c;

	return *this;
}

Connections::~Connections() {
}

int Connections::init() {
	struct sockaddr_in addr;
	int		optval;

	memset(&addr, 0, sizeof(addr));
	FD_ZERO(&active_set);
	optval = 1;
	addr.sin_family = AF_INET;
	for (std::vector<Server>::iterator server = servers.begin();
			server != servers.end() && servers.size() > 0; server++)
	{
		int		fd;

		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
			break ;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval,
			sizeof(optval)) == -1)
		{
			close(fd);
			break ;
		}
		addr.sin_addr.s_addr = inet_addr((server)->get_host().c_str());
		addr.sin_port = htons((server)->get_port());
		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		{
			INFO(BLUE, "Error Binding PORT:", RED, (server)->get_port());
			servers.erase(server);
			close (fd);
			break ;
		}
		INFO(BLUE, "OPEN PORT", GREEN, (server)->get_port());
		if (listen(fd, SOMAXCONN) < 0)
		{
			servers.erase(server);
			close (fd);
			break ;
		}
		(server)->set_fd(fd);
		FD_SET(fd, &active_set);
		fd_list.push_back(fd);
		max_fd = fd;
		nb_serv++;
	}
	if (nb_serv == 0)
		return 1;
	else
	{
		max_fd = *std::max_element(	fd_list.begin(), fd_list.end());
		return 0;
	}
}

int Connections::add_client(Server &server) {
	Client new_client;
	int		fd;

	ready_fd--;
	if (servers.size() + clients.size() < FD_SETSIZE)
	{
		if ((fd = accept(server.get_fd(), 0, 0)) == -1)
			return -1;
		MSG(GREEN, " ------ Connection Accepted ----- ");
		FD_SET(fd, &active_set);
		fd_list.push_back(fd);
		max_fd = *std::max_element(fd_list.begin(), fd_list.end());
		Client new_client(fd, &server);
		clients.push_back(new_client);
	}
	else
		return (error_msg("Max connections reached"));
	return 0;
}

int Connections::check_clients() {
	int fd;

	for (std::vector<Client>::iterator client = clients.begin(); 
	client != clients.end() && ready_fd != 0 ; client++)
	{
		fd = (client)->get_fd();
		if (FD_ISSET(fd, &ready_rset))
		{
			ready_fd--;
			(client)->receive_request();
			if ((client)->request_is_ready())
			{	
				(client)->wait_response();
				close(fd);
				FD_CLR(fd, &active_set);
				fd_list.remove(fd);
				max_fd = *std::max_element(fd_list.begin(), fd_list.end());
				client = clients.erase(client);
			}
		}
	}
	return 0;
}


void	Connections::ft_clear_clients( void )
{
	for (std::vector<Client>::iterator cli_it = clients.begin();
			cli_it != clients.end() && ready_fd != 0 ; cli_it++)
		(*cli_it).~Client();
	clients.clear();
}

void	Connections::ft_clear_servers( void )
{
	for (std::vector<Server>::iterator serv_it = servers.begin();
		serv_it != servers.end() && ready_fd != 0 ; serv_it++)
		(*serv_it).~Server();
}

void	Connections::loop(void) 
{
	MSG(YELLOW, " ----- Waiting for connection -----");
	while (1)
	{
		ready_rset = active_set;
		ready_fd = select(max_fd + 1, &ready_rset, 0, 0, 0);
		for (std::vector<Server>::iterator server = servers.begin();
			server != servers.end(); server++)
		{
			if (FD_ISSET((server)->get_fd(), &ready_rset))
				add_client(*server);
		}
		check_clients();
	}
	ft_clear_clients();
}
