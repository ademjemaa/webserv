#include "Cgi.hpp"

Cgi::Cgi(Request &request) {
	_init_envs(request);
	execute(request);
	return ;
}

char    **Cgi::_map_to_table_char(std::map<std::string, std::string> map) {
	char        **envs_char;
	std::string s;
	size_t      j;

	j = 0;
	envs_char = new char*[map.size() + 1];
	for (std::map<std::string, std::string>::iterator i = map.begin(); i != map.end(); i++) {
		s = i->first + "=" + i->second;
		envs_char[j] = new char[s.size()];
		envs_char[j] = strdup(s.c_str());
		j++;
	}
	envs_char[j] = 0;
	return (envs_char);
}

void	Cgi::_init_envs(Request &request) {
	std::map<std::string, std::string>	envs;

	envs["REDIRECT_STATUS"] = "200";
	envs["SERVER_PROTOCOL"] = "HTTP/1.1";
	envs["PATH_INFO"] = request.get_conf().path;
	envs["CONTENT_LENGTH"] = request._headers["Content-Length"];
	envs["CONTENT_TYPE"] = request._headers["Content-Type"];
	envs["GATEWAY_INTERFACE"] = "CGI/1.1";
	envs["PATH_TRANSLATED"] = request._conf.root + request._uri;
	envs["QUERY_STRING"] = request._uri.substr(request._uri.find("?") + 1);
	envs["REMOTE_ADDR"] = "";
	envs["REMOTE_HOST"] = "";
	envs["REMOTE_IDENT"] = "";
	envs["REMOTE_USER"] = "";
	envs["REQUEST_METHOD"] = request.get_method();
	envs["SERVER_PORT"] = to_string(request.get_conf().port);
	envs["SERVER_SOFTWARE"] = "webserv/1.0";
	envs["SERVER_NAME"] = request._headers["Host"];
	std::map<std::string, std::string>	headers = request.get_headers();
	for (std::map<std::string, std::string>::iterator i = headers.begin(); i != headers.end(); i++) {
		std::string	tmp = i->first;
		size_t pos = tmp.find_first_of("-");
		while (pos != std::string::npos) {
			tmp.replace(pos, 1, "_");
			pos = tmp.find_first_of("-", pos);
		}
		for (size_t n = 0; n < tmp.size(); n++)
			tmp[n] = toupper(tmp[n]);
		envs["HTTP_" + tmp] = i->second;
	}
	_envs = _map_to_table_char(envs);
}

std::string	Cgi::_read_file(std::string path) {
	std::string		s;
	std::string		buff;
	std::ifstream	file(path.c_str());

	if (!file.is_open())
		return (s);
	while(std::	getline(file, buff)) {
		s += buff;
	}
	file.close();

	return (s);
}

std::string	Cgi::execute(Request &request) {
	pid_t   		pid;
	int				status;
	int				ret_fd;
	int				fds[2];
	char            **args;

	if (!(args = (char**)malloc(sizeof(**args) * 3)))
		return ("Status: 500\r\n\r\n");
    args[0] = strdup((request._conf.root + request._conf.cgi_path + request._conf.cgi_extension[0]).c_str());
    args[1] = strdup((request._conf.root + request._uri).c_str());
    args[2] = NULL;
	if (pipe(fds) == -1)
		return ("Status: 500\r\n\r\n");
	pid = fork();
	if (pid == -1) {
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid) {
		close(fds[1]);
		dup2(fds[0], 0);
	
		ret_fd = open("webserv_cgi", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (ret_fd < 0)
			exit(0);

		dup2(ret_fd, 1);
		dup2(ret_fd, 2);

		execve(args[0], args, _envs);
		
		close(ret_fd);
		close(fds[0]);

		exit(0);
	}
	free(args);
	close(fds[0]);
	write(fds[1], request.get_body().c_str(), request.get_body().length());
	close(fds[1]);
	wait(&status);
	// for (int i = 0; _envs[i]; i++)
	// 	delete _envs[i];
	// delete[] _envs;
	return (_read_file("webserv_cgi"));
}

Cgi::~Cgi(void) {
	return ;
}

Cgi::Cgi(const Cgi & src) {
	(void)src;

	return ;
}

Cgi &   Cgi::operator=(const Cgi & src) {
	(void)src;

	return (*this);
}