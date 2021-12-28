#include "webserv.hpp"

std::string	cut_line(std::string &str, bool cond, int back)
{
	std::string ret;

	ret = str.substr(0, str.find('\n') + back);
	if (cond == true)
		str.erase(0, str.find('\n') + 1);
	return (ret);
}


std::string	MIME_types(std::string	str)
{
	std::string ret;
	std::map<std::string, std::string> mimes;

	if (str.length() > 0 && (str.find('.') != std::string::npos))
		ret = str.substr(str.find('.'));
	else
		ret = ".html";
	mimes[".aac"] = "audio/aac";
	mimes[".abw"] = "application/x-abiword";
	mimes[".arc"] = "application/x-freearc";
	mimes[".avi"] = "video/x-msvideo";
	mimes[".bin"] = "application/octet-stream";
	mimes[".bmp"] = "image/bmp";
	mimes[".bz"] = "application/x-bzip";
	mimes[".csh"] = "application/x-csh";
	mimes[".css"] = "text/css";
	mimes[".csv"] = "text/csv";
	mimes[".doc"] = "application/msword";
	mimes[".gz"] = "application/gzip";
	mimes[".ico"] = "image/vnd.microsoft.icon";
	mimes[".jpeg"] = "image/jpeg";
	mimes[".jpg"] = "image/jpeg";
	mimes[".html"] = "text/html";
	mimes[".htm"] = "text/html";
	mimes[".pdf"] = "application/pdf";
	mimes[".php"] = "application/x-httpd-php";
	mimes[".sh"] = "application/x-sh";
	mimes[".tar"] = "application/x-tar";
	mimes[".txt"] = "text/plain";
	mimes[".zip"] = "application/zip";

	return (mimes[ret]);
}

std::string	Last_modified(std::string path)
{
	struct stat	ret;
	char		buffer[1024] = "";
	struct tm * timeinfo;

	if (stat(path.c_str(), &ret) == 0)
	{
		ret.TIME.tv_sec = ret.TIME.tv_sec - 3600;
		timeinfo = localtime(&ret.TIME.tv_sec);
		strftime(buffer, 1024, "%a, %d %b %Y %T GMT", timeinfo);
		std::string	time_string(buffer);
		return (time_string);
	}
	return ("");
}

std::string	status_code(int code)
{
	switch (code)
	{
	case 200:
		return ("200 OK");
		break;
	case 201:
		return ("201 Created");
		break;
	case 204:
		return ("204 No Content");
		break;
	case 400:
		return ("400 Bad Request");
		break;
	case 403:
		return ("403 Forbidden");
		break;
	case 404:
		return ("404 Not Found");
		break;
	case 405:
		return ("405 Method Not Allowed");
		break;
	case 413:
		return ("413 Payload Too Large");
		break;
	case 500:
		return ("500 Internal Server Error");
		break;
	case 501:
		return ("501 Not Implemented");
		break;
	}
	return "";
}

std::string		read_html(std::string path, std::string &body)
{
	std::ifstream ifs;
	std::stringstream ss;
	std::stringstream ss1;

	ifs.open(path.c_str());
	ss << ifs.rdbuf();
	body += ss.str();
	return (ss.str());
}

std::string		get_current_time(void)
{
	struct timeval tmp;
	struct tm * timeinfo;
	char		buffer[1024] = "";
	

	gettimeofday(&tmp, NULL);
	tmp.tv_sec = tmp.tv_sec - 3600;
	timeinfo = localtime(&tmp.tv_sec);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", timeinfo);
	std::string	ret(buffer);
	return (ret);
}

int				error_msg(const char *msg)
{
	std::cerr << RED << "Error: " << msg << DFL << std::endl;
	return EXIT_FAILURE;
}

int				file_status(std::string		path)
{
    struct stat             s;

    if (stat(path.c_str(), &s) == 0 )
    {
        if (s.st_mode & S_IFREG)
            return (200);
        else
            return (403);
    }
	return (404);
}

std::string	to_string(int n) {
	std::stringstream	ss;
	std::string			s;

	ss << n;
	s = ss.str();
	return (s);
}

std::string	default_error_page(int error_code)
{
	std::string tmp;

	tmp += "<!DOCTYPE html>\n<html>\n<title>";
	tmp += status_code(error_code);
	tmp += "</title>\n<body>\n<div>\n<H1>";
	tmp += status_code(error_code);
	tmp += "</H1>\n</div>\n</body>\n</html>\n";

	return (tmp);
}