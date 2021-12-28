#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <unistd.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <fcntl.h>
# include <time.h>
# include <dirent.h>


# include <cstring>
# include <iostream>
# include <string>
# include <algorithm>
# include <vector>
# include <list>
# include <map>
# include <sstream>
# include <fstream>

# ifndef __APPLE__

#  define	TIME	st_mtim
/* ********************************************* */
/*	     COLOR FOR BETTER TEXT INFORMATION:		 */
/*	- ATTENTION: FOR MAC OS						 */
/* ********************************************* */
#  define DFL	 "\e[0;39m"
#  define RED	 "\e[0;31m"
#  define GREEN	 "\e[0;32m"
#  define YELLOW "\e[0;33m"
#  define BLUE	 "\e[0;34m"
# else
#  define TIME	st_mtimespec
/* ********************************************* */
/*	     COLOR FOR BETTER TEXT INFORMATION:		 */
/*	- ATTENTION: FOR LINUX						 */
/* ********************************************* */
#  define DFL	 "\033[39m"
#  define RED	 "\033[31m"
#  define GREEN	 "\033[32m"
#  define YELLOW "\033[33m"
#  define BLUE	 "\033[34m"
# endif

/* ********************************************* */
/*	    	DEFINE FOR BETTER CODE:				 */
/* ********************************************* */
# define ERR_ARG "No config file found."
/* ********************************************* */
# define SPACE std::cout << std::endl;
# define LINE	" ---------------------------------- " << std::endl
# define MSG(color, msg) std::cout << color << msg << DFL << std::endl
# define INFO(color, type, color2, value) std::cout << color << type << ": " << color2 << value << std::endl << DFL
# define TITLE(color, title)	std::cout << LINE << color << title << std::endl << DFL << LINE
/* ********************************************* */


/* Location struct */
struct	s_location {
    std::map<int, std::string>  error_pages;
    std::vector<std::string>	methods;
    std::vector<std::string>	cgi_extension;
    std::vector<std::string>	index;

    std::string					path;
    std::string					root;
    std::string					cgi_path;
    std::string					upload_path;
    std::string					auth_basic_user_file;
    std::string					auth_basic;

    bool						upload_eanable;
    bool						autoindex;

    unsigned int				client_max_body_size;
};

struct  s_config {
    std::map<int, std::string>					error_pages;
    std::vector<std::string>					names;
    std::vector<s_location>						locations;
    std::string									host;
    std::string									root;
    unsigned int								port;
    unsigned int								client_max_body_size;
    int											fd;
};

typedef struct  s_chunk
{
    int         length;
	int			status;

}               t_chunk;


std::string		cut_line(std::string &str, bool cond, int back);
std::string		MIME_types(std::string	str);
std::string		Last_modified(std::string path);
std::string	    status_code(int code);
std::string     read_html(std::string path, std::string &body);
std::string     to_string(int n);
std::string		get_current_time(void);
std::string	    default_error_page(int error_code);
std::string	    to_string(int n);
size_t	        count_char_in_string(std::string & s, char c);
int				file_status(std::string path);
int				error_msg(const char *msg);
void	        remove_extra_space(std::string & str, size_t pos);

#endif
