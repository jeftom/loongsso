#ifndef	_UTIL_H
#define	_UTIL_H

int is_mail(char *str);

int is_password(char *str);

int is_user_exists(char *str);

int is_mail_exists(char *str);

int is_username(unsigned char *str);


char *long2ip(unsigned int v);

unsigned int ip2long(char *s);

int daemon(int nochdir, int noclose);

unsigned int strhash(const char *str);

//������֤��� �����ID
uint64_t ident_key();

//������֤��� �����ֵ
void ident_value(unsigned char *result);

int MD5String(char *str,char *hex_output);

int delete_user_info(TCMAP *data);

int update_user_info(TCMAP *data);

TCMAP *fetch_user_info(const char *uid);

//���ͽ��
void send_response(loong_conn *conn, http_response_t cmd, char *json);

unsigned long urlencode(unsigned char   *csource, unsigned char   *cbuffer, unsigned long   lbuffersize);

#endif

