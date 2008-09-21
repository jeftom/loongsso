#ifndef	_UTIL_H
#define	_UTIL_H


//������֤��� �����ID
uint64_t ident_key();

int make_socket();

char *long2ip(unsigned int v);

unsigned int ip2long(char *s);

bool is_mail(const char *str);

bool is_password(const char *str);

bool is_mail_exists(const char *str);

int daemon(int nochdir, int noclose);

unsigned int strhash(const char *str);

int set_nonblocking(int sock);

//�Ƿ�ʱ
bool is_timeout(time_t t1, int minute);

//ֻ����Ӣ����ĸ�����֡����ĵ��û���
bool is_ch_username(unsigned char *str);

//ֻ����Ӣ����ĸ�����ֵ��û���
bool is_alpha_username(const char *str);

//������֤��� �����ֵ
void ident_value(unsigned char *result);

int MD5String(char *str,char *hex_output);

http_response_t delete_user_info(TCMAP *data);

http_response_t update_user_info(TCMAP *data);

TCMAP *fetch_user_info(const char *uid);

//���ͽ��
void send_response(loong_conn *conn, http_response_t cmd, char *json);

unsigned long urlencode(unsigned char   *csource, unsigned char   *cbuffer, unsigned long   lbuffersize);

#endif

