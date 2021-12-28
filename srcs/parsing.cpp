#include "Config.hpp"

size_t	count_char_in_string(std::string & s, char c) {
	size_t	count;

	count = 0;
	for (size_t	i = 0; (i = s.find(c, i)) != std::string::npos; i++)
		count++;
	return (count);
}

void	remove_extra_space(std::string & str, size_t pos) {
	while (std::isspace(str[pos]))
		str.erase(str.begin() + pos);
}

char	parse(std::vector<Server> &servers, char *path) {
	std::ifstream	file(path);
	std::string		spath(path);
	std::string		content;
	Config			config;
	int				line_count;

	if (".conf" != spath.substr(spath.size() - 5, spath.size()))
		throw Config::Error("Bad extension.", 0);
	if (!file.is_open()) {
		throw Config::Error("Unable to open config file.", 0);
	}
	line_count = 0;
	while (std::getline(file, content)) {
		
		remove_extra_space(content, 0);
		if (content.empty() || content[0] == '#') {
			line_count++;
			continue;
		}
		
		line_count++;
		if (!content.compare(0, 5, "server")) {
			throw Config::Error("I don't know.", line_count);
		}
		remove_extra_space(content, 6);
		
		if (!content.compare(0, 6, "server{")) {
			throw Config::Error("I don't know.", line_count);
		}
		
		Server	serv;
		config.parse(serv, file, line_count);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
			if ((*it).get_port() == serv.get_port()) {
				throw Config::Error("duplicate port.", line_count);
			}
			for (std::vector<std::string>::iterator name = serv.get_names().begin(); name != serv.get_names().end(); name++) {
				for (std::vector<std::string>::iterator old_name = (*it).get_names().begin(); old_name != (*it).get_names().end(); old_name++) {
					if (*old_name == *name) {
						throw Config::Error("Duplicate name.", line_count);
					}
				}
			}
		}
		servers.push_back(serv);
	}
	if (!servers.size())
		throw Config::Error("No server found.", 0);

	return (0);
}
