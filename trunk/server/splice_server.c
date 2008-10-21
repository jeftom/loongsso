#define _GNU_SOURCE
#include <stdio.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/resource.h>
#include "server.h"
#include "sock_io.h"

#define SPLICE_SIZE	(64*1024)

#define min(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x < _y ? _x : _y; })

#define max(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x > _y ? _x : _y; })


const char *mimetype(const char *filename)
{
	static const char *(assocNames[][2]) =
	{
		{ "c",       "text/plain"                         },
		{ "js",      "text/javascript"                    },
		{ "gz",      "application/x-gzip"                 },
		{ "ps",      "application/postscript"             },
		{ "pdf",     "application/pdf"                    },
		{ "dvi",     "application/x-dvi"                  },
		{ "tgz",     "application/x-tgz"                  },
		{ "tar",     "application/x-tar"                  },
		{ "zip",     "application/zip"                    },
		{ "mp3",     "audio/mpeg"                         },
		{ "m3u",     "audio/x-mpegurl"                    },
		{ "wma",     "audio/x-ms-wma"                     },
		{ "wax",     "audio/x-ms-wax"                     },
		{ "ogg",     "application/ogg"                    },
		{ "wav",     "audio/x-wav"                        },
		{ "gif",     "image/gif"                          },
		{ "jar",     "application/x-java-archive"         },
		{ "jpg",     "image/jpeg"                         },
		{ "jpeg",    "image/jpeg"                         },
		{ "png",     "image/png"                          },
		{ "xbm",     "image/x-xbitmap"                    },
		{ "xpm",     "image/x-xbitmap"                    },
		{ "xwd",     "image/x-xwindowdump"                },
		{ "css",     "text/css"                           },
		{ "asc",     "text/plain"                         },
		{ "cpp",     "text/plain"                         },
		{ "htm",     "text/html"                          },
		{ "html",    "text/html"                          },
		{ "log",     "text/plain"                         },
		{ "txt",     "text/plain"                         },
		{ "conf",    "text/plain"                         },
		{ "dtd",     "text/xml"                           },
		{ "xml",     "text/xml"                           },
		{ "mov",     "video/quicktime"                    },
		{ "mpg",     "video/mpeg"                         },
		{ "mpeg",    "video/mpeg"                         },
		{ "qt",      "video/quicktime"                    },
		{ "avi",     "video/x-msvideo"                    },
		{ "asf",     "video/x-ms-asf"                     },
		{ "asx",     "video/x-ms-asf"                     },
		{ "wmv",     "video/x-ms-wmv"                     },
		{ "bz2",     "application/x-bzip"                 },
		{ "tbz",     "application/x-bzip-compressed-tar"  },
		{ "sig",     "application/pgp-signature"          },
		{ "spl",     "application/futuresplash"           },
		{ "swf",     "application/x-shockwave-flash"      },
		{ "pac",     "application/x-ns-proxy-autoconfig"  },
		{ "class",   "application/octet-stream"           },
		{ "tar.gz",  "application/x-tgz"                  },
		{ "torrent", "application/x-bittorrent"           },
		{ "tar.bz2", "application/x-bzip-compressed-tar"  },
		{ "",        "application/octet-stream"           }
	};

	const char *((*anp)[2]);
	char *suffix;

	suffix = strrchr(filename, '.');
	if (suffix != NULL) 
	{
		suffix++;
		for (anp=assocNames; strlen((*anp)[0])>0; anp++)
			if (!strcmp((*anp)[0], suffix)) break;
	}

	return (*anp)[1];
}

void parse_query(char *request, char *uri, int url_len, char *query, int query_len)
{
	int i, pos, flags, len, num;
	
	num   = 0;
	pos   = 0;
	flags = 0;
	len   = strlen(request);
	for(i=0; i<len; i++)
	{
		if(request[i] == '?')
		{
			flags = 1;
		}
		else if(flags && pos < query_len)
		{
			query[num++] = request[i];
		}
		else if(!flags && pos < url_len)
		{
			uri[pos++] = request[i];
		}
		else 
		{
			break;
		}
	}

	request[pos] = '\0';
}


int request_parse(char *req_ptr, size_t req_len, int fd) 
{
	int len, pos;
	struct conn_t *conn;
	char *line_start, *line_end, *ptr, *req_end, *uri;
	
	ptr     = req_ptr;
	req_end = req_ptr + req_len;
	conn    = &server.conn[fd];

	memset(&conn->req.uri,       0, sizeof(conn->req.uri));
	memset(&conn->req.filepath,  0, sizeof(conn->req.filepath));
	memset(&conn->req.query_ptr, 0, sizeof(conn->req.query_ptr));
	memset(&conn->req.if_modified_since, 0, sizeof(conn->req.if_modified_since));

	for (line_end = ptr;ptr < req_end;ptr++) 
	{
		if (ptr >= line_end) 
		{
			//At the end of a line, find if we have another
			line_end = ptr;
			
			//Find start of next line
			while (ptr < req_end && (*ptr == '\n' || *ptr == '\r')) 
			{
				//If null line found return 1
				if (*ptr == '\n' && (*(ptr - 1) == '\n' || *(ptr - 2) == '\n'))
					return 1;
				ptr++;
			}
			
			line_start = ptr;
			
			//Find end of line
			while (ptr < req_end && *ptr != '\n' && *ptr != '\r') ptr++;
			
			line_end = ptr - 1;
			ptr      = line_start;
		}
		
		switch (*ptr) 
		{
			case ':':
				ptr += 2;
				switch (*line_start) 
				{
					case 'I':
						if ((line_start + 3) < req_end && !strncmp(line_start+1, "f-Modified-Since", 16)) 
						{
							len = line_end - ptr;
							pos = (len+1) <= sizeof(conn->req.if_modified_since) ? (len+1) : sizeof(conn->req.if_modified_since); 
							memcpy(conn->req.if_modified_since, ptr, pos);
						}
						break;
					case 'C':
						if ((line_start + 9) < req_end && !strncmp(line_start+1, "onnection", 9)) 
						{
							if ((ptr + 10) < req_end && !strncmp(ptr, "keep-alive", 10))
								conn->req.keep_alive = 1;
							else if ((ptr + 5) < req_end && !strncmp(ptr, "close", 5))
								conn->req.keep_alive = 0;
						}
				}
				ptr = line_end;
				break;
			case ' ':
				switch (*line_start) 
				{
					case 'G':
						if ((line_start + 3) < req_end && !strncmp(line_start+1, "ET", 2))
							conn->req.http_method = HTTP_METHOD_GET;
						break;
					case 'P':
						if ((line_start + 4) < req_end && !strncmp(line_start+1, "OST", 3))
							conn->req.http_method = HTTP_METHOD_POST;
						break;
					case 'H':
						if ((line_start + 4) < req_end && !strncmp(line_start+1, "EAD", 3))
							conn->req.http_method = HTTP_METHOD_POST;
				}
				
				line_start = ptr;
				do ptr++; while (ptr < line_end && *ptr != ' ');
				
				if (*ptr == ' ' && (ptr + 8) == line_end && !strncmp(ptr+1, "HTTP/1.", 7)) 
				{
					if (*(ptr+8) == '0')
						conn->req.http_version = HTTP_VERSION_1_0;
					else if (*(ptr+8) == '1')
						conn->req.http_version = HTTP_VERSION_1_1;
					
					len = ptr - line_start;
					pos = len <= sizeof(conn->req.filepath) ? len : sizeof(conn->req.filepath); 
					memcpy(conn->req.filepath, line_start+1, pos-1);
					conn->req.filepath[pos-1] = '\0';
					
					uri  = strrchr(conn->req.filepath, '/');
					if(uri)
					{
						uri++;
						parse_query(uri, conn->req.uri, sizeof(conn->req.uri), conn->req.query_ptr, sizeof(conn->req.query_ptr));
					}
				}

				ptr = line_end;
				break;
		}
	}
	
	return 1;
}

static int client_splice_loop(int out_fd, int fd, int *pfd)
{
	loff_t off;
	struct stat sb;
	int  header_len;
	char header[400];
	char timebuf[100];
	char lastime[100];
	struct conn_t *conn;
	unsigned int length;
	uint64_t size;
	int len, slen;
	
	conn  = &server.conn[out_fd];
	memset(&header, 0, sizeof(header));
	memset(&timebuf, 0, sizeof(timebuf));
	memset(&lastime, 0, sizeof(lastime));
	
	off    = 0;
	length = hashmap_get(memdisk, conn->req.filepath);
	if(length)
	{
		size = length;
	}
	else
	{
		if (fstat(fd, &sb) < 0)
		{
			printf("fstat %s\n", strerror(errno));
			return errno;
		}
		size = sb.st_size;
		hashmap_add(memdisk, conn->req.filepath, sb.st_size);
	}
	
	strftime(timebuf, sizeof(timebuf), RFC_TIME, localtime(&conn->now));
	strftime(lastime, sizeof(lastime), RFC_TIME, localtime(&sb.st_mtime));
	
	header_len = snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nServer: Memhttpd/Beta1.0\r\nDate: %s\r\nContent-Type: %s\r\nContent-Length: %llu\r\nLast-Modified: %s\r\nConnection: keep-alive\r\nAccept-Ranges: bytes\r\n\r\n", timebuf, mimetype(conn->req.filepath), size, lastime);
	
	write(conn->fd, header, header_len);

	
	do {
		int ret = splice(fd, &off, pfd[1], NULL, 1400, SPLICE_F_NONBLOCK | SPLICE_F_MORE | SPLICE_F_MOVE);

		if (ret < 0)
		{
			if(errno == EAGAIN)
			{
				fprintf(stderr, "EAGAIN: IN=%d, OUT=%d\n", fd, pfd[1]);
				usleep(1000);
				continue;
			}

			printf("splice-in %s\n", strerror(errno));
			return errno;
		}
		size -= ret;
		while (ret > 0) 
		{
			int written = splice(pfd[0], NULL, out_fd, NULL, ret, SPLICE_F_NONBLOCK | SPLICE_F_MORE | SPLICE_F_MOVE);

			if (written < 0)
			{
				if(errno == EAGAIN)
				{
					fprintf(stderr, "EAGAIN: OUT=%d, OUT=%d\n", pfd[0], out_fd);
					usleep(1000);
					continue;
				}


				printf("splice-out %s\n", strerror(errno));
				return errno;
			}

			ret -= written;
		}
	} while (size);

	
	return 0;
}

static int client_splice(int out_fd, int file_fd)
{
	int *pfd, ret;
	
	pfd = (int *)malloc(sizeof(int)*2);

	if (pipe(pfd) < 0)
	{
		printf("pipe %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		//return errno;
	}
	
	ret = client_splice_loop(out_fd, file_fd, pfd);
	close(pfd[0]);
	close(pfd[1]);
	safe_free(pfd);

	return ret;
}



void http_request_error(struct conn_t *conn, char *cmd)
{
	ssize_t bytes;
	char body[400];
	char header[400];
	char timebuf[100];
	struct iovec vectors[2];
	int  header_len, body_len;

	bytes = 0;
	memset(&body, 0, sizeof(body));
	memset(&header, 0, sizeof(header));
	memset(&timebuf, 0, sizeof(timebuf));
	
	strftime(timebuf, sizeof(timebuf), RFC_TIME, localtime(&conn->now));

	if(strcmp(cmd, "404 Not Found") == 0)
	{
		body_len   = snprintf(body, sizeof(body), "<html>\r\n<head>\r\n<title>404 Not Found</title>\r\n</head>\r\n<body>\r\n<h1>Not Found</h1><p>The requested URL %s was not found on this server.</p><hr><address>Memhttpd/Beta1.0</address>\r\n</body>\r\n</html>", conn->req.filepath);
		header_len = snprintf(header, sizeof(header), RFC_404, cmd, timebuf, body_len);

		vectors[0].iov_base = header;
		vectors[0].iov_len  = header_len;
		vectors[1].iov_base = body;
		vectors[1].iov_len  = body_len;

		bytes = writev(conn->fd, vectors, 2);
	}
	else
	{
		header_len = snprintf(header, sizeof(header), RFC_304, cmd, timebuf);
		bytes = send(conn->fd, header, header_len, 0);
	}
	sock_close(conn->fd);
}

void http_request_accept(int fd)
{
	struct sockaddr_in addr;
	int socklen = sizeof(struct sockaddr);

	int client_fd = accept(fd, (struct sockaddr*)&addr, (socklen_t *)&socklen);
	
	if(client_fd == -1)
	{
		printf("http_request_accept %s! Exit.\n", strerror(errno));
		return;
	}
	
	fd_open(client_fd);
	sock_set_linger(client_fd);

	sock_epoll_add(client_fd, EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP);
}

void http_request_write(int fd)
{
	int rc, fp;
	char filename[200];
	struct conn_t *conn;
	const struct record *recs;
	
	conn  = &server.conn[fd];

	memset(&filename, 0, sizeof(filename));
	snprintf(filename, sizeof(filename), "%s%s", server.root, conn->req.filepath);
	
	fp = open(filename, O_RDONLY);
	if(fp == -1)
	{
		http_request_error(conn, "404 Not Found");
		return ;
	}

	client_splice(conn->fd, fp);
	close(fp);
	sock_close(conn->fd);
}

void http_request_read(int fd)
{
	ssize_t bytes;
	fd_set readfds;
	struct timeval tv;
	char buffer[1024];
	struct conn_t *conn;
	
	bytes = 0;
	conn  = &server.conn[fd];
	memset(&buffer, 0, sizeof(buffer));

	tv.tv_sec  = 10;
	tv.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(conn->fd, &readfds);
	
	select(conn->fd+1, &readfds, NULL, NULL, &tv);
	
	if (!FD_ISSET(conn->fd, &readfds)) 
	{
		// timeout
		return ; 
	}

	bytes = recv(conn->fd, buffer, sizeof(buffer), 0);
	if(bytes <= 0)
	{
		sock_close(conn->fd);
		return ;
	}
	
	request_parse(buffer, bytes, conn->fd);

	
//	printf("uri = %s\r\n", conn->req.uri);
//	printf("query = %s\r\n", conn->req.query_ptr);
//	printf("filepath = %s\r\n", conn->req.filepath);
//	printf("if_modified_since = %s\r\n", conn->req.if_modified_since);
	
	sock_epoll_mod(fd, EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP);
}


static void server_down(int signum)
{
	sock_epoll_free();
	hashmap_destroy(memdisk);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
//	daemon(1, 1);
	struct rlimit rt;
	struct sigaction sa;

	sa.sa_handler = SIG_IGN;//�趨���ܵ�ָ���źź�Ķ���Ϊ����
	sa.sa_flags   = 0;
	
	//��ʼ���źż�Ϊ�� ����SIGPIPE�ź�
	if (sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, 0) == -1) 
	{ 
		perror("failed to ignore SIGPIPE; sigaction");
		exit(EXIT_FAILURE);
	}

	signal(SIGTERM, server_down);
	signal(SIGINT,  server_down);
	signal(SIGQUIT, server_down);
	signal(SIGSEGV, server_down);
	signal(SIGALRM, server_down);

	rt.rlim_max = rt.rlim_cur = MAX_FD;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1) 
	{
		perror("setrlimit");
		exit(EXIT_FAILURE);
	}

	sock_init();
	memdisk = hashmap_new(100);
	sock_epoll_wait(-1);
	return 0;
}

/*

HTTP/1.1 200 OK
Server: qiye/RC1.2
Date: Wed, 15 Oct 2008 09:26:37 GMT
Content-Type: application/x-javascript
Content-Length: 7750
Last-Modified: Mon, 15 Sep 2008 08:42:04 GMT
Connection: keep-alive
Accept-Ranges: bytes


HTTP/1.1 404 Not Found
Server: qiye/RC1.2
Date: Thu, 16 Oct 2008 02:26:34 GMT
Content-Type: text/html
Content-Length: 527
Connection: keep-alive

*/

