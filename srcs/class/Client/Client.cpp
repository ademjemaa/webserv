#include "Client.hpp"

Client::Client()
{
}

Client::Client(int fd, Server *server) : _fd(fd), _server(server), _ready_request(false)
{	
	_bytes_request = 0;
}

Client::Client(const Client &c) : _fd(c._fd), _server(c._server)
{
	*this = c;
}

Client& Client::operator=(const Client &c)
{

		_fd = c._fd;
		_server = c._server;
		_request = c._request;
		_ready_request = c._ready_request;
		_received_request = c._received_request;
		_bytes_request = c._bytes_request;

	return *this;
}

Client::~Client()
{
}

int		Client::get_fd(void) const
{
	return _fd;
}

void	Client::wait_response(void)
{
	_server->respond(_fd, _request);
}

/*

CAS 1 -> Non Chunked
	Si content_lenght -> 
	- Lire jusqua content len = bytes receive 
	
	( doit keep le bytes reveive )

	je parse


CAS 2 ->  Chunked   Transfere encoding .. lecture until = 0\r\n\r\n

	TANT QUE
		charger le buffer jusqu'a end.

	je parse

CAS 3 -> Pas de Body
	- tout d'un coup

	je parse


RECV = -1 
error, on doit gere ce cas propremement



*/

void 	Client::receive_request(void)
{
	u_long len_content;
	char buffer[2024];
	int ret;

	while (1)
	{
		bzero(buffer, 2024);
		ret = recv(_fd, buffer, 2024, 0);
		if (ret == -1)
			exit(6);
		buffer[ret] = 0;
		_received_request += buffer;
		_bytes_request += ret;
		if (ret == 0)
			break ;

		if (_received_request.find("\r\n\r\n") != std::string::npos) // HEADER FINI
		{
			if (_received_request.find("Content-Length") != std::string::npos)
			{
				len_content = atoi(_received_request.substr(_received_request.find("Content-Length:") + 16).substr(0, _received_request.substr(_received_request.find("Content-Length:") + 16).find("\n")).c_str());

				if ((u_long)_bytes_request - _received_request.find("\r\n\r\n") >= len_content)
				{
					break ;
				}
			}
			else if (_received_request.find("Transfer-Encoding") != std::string::npos)
			{
				if (_received_request.find("0\r\n\r\n") != std::string::npos)
				{
					break ;
				}

			}
			else
				break ;
		}
		else
			break ;
	}


	// ICI ON AFFICHE QUE 200 CHAR + ...

	 if (_received_request.size() > 200)
	 	std::cout << _received_request.substr(0, 197) + "..." << std::endl;
	 else
	 	MSG(DFL, _received_request);

	_request.parse(_received_request, _server->get_config(), len_content);
	_ready_request = true;
}

bool		Client::request_is_ready(void)
{
	return _ready_request;
}