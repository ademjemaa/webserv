#include "Connections.hpp"
#include "webserv.hpp"

int	main(int ac, char **av)
{
	Connections connections;

	if (ac != 2)
		return error_msg( ERR_ARG );
	try {
		parse(connections.servers, av[1]);
	}
	catch (std::exception &e) {
		return error_msg( e.what() );
	}
	if (connections.init() == 1)
		return (1);
	connections.loop();
	return 0;
}