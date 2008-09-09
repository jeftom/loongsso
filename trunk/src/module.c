#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <gd.h>
#include <gdfontg.h>
#include <gdfonts.h>
#include <gdfontl.h>
#include <loong.h>


//������֤��
int loong_sso_validate(loong_conn *conn)
{
	uint64_t   key;
	gdImagePtr img;
	char header[700], value[5];
	
	time_t lifetime;
	unsigned char *data;
	struct iovec vectors[2];
	char timebuf[100], cookie_buf[100];

	int watermark, background, font_color, randcolor, i, size, header_len, rc;
	
	srand((unsigned)time(0));

	memset(header, 0, sizeof(header));
	
	rc = 0;
	while(!rc)
	{
		//������ʧ��,��ѭ������������ݡ�ֱ������1(true)
		memset(value, 0, sizeof(value));

		key = ident_key();
		ident_value(value);

		lifetime = time((time_t*)0);
		rc = hash_add(codepool, key, value, lifetime);
	}
	
	lifetime += 60;

	img        = gdImageCreate(80, 25);  
	background = gdImageColorAllocate(img, 114 , 114 , 114); 
	watermark  = gdImageColorAllocate(img, 157 , 157 , 157); 
	font_color = gdImageColorAllocate(img , 254 , 248 , 248);

	gdImageFill(img, 0, 0, background);    
	
	for(i=0; i<200; i++)
	{ 
		// ����������� 
		randcolor = gdImageColorAllocate(img, rand() % 255, rand() % 255, rand() % 255);
		gdImageSetPixel(img, rand() % 80 , rand() % 25 , randcolor); 
	}
	
	gdImageString(img, gdFontLarge, 5, 5, WATER_MARK, watermark);
	
	gdImageStringFT(img, 0, font_color, SIGN_FONT_PATH, 15, 0.0, 10, 20, value);
	
	data = (unsigned char *)gdImagePngPtr(img, &size);

	strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %H:%M:%S GMT", localtime(&conn->now));
	strftime(cookie_buf, sizeof(cookie_buf), "%a, %d-%b-%Y %H:%M:%S GMT", localtime(&lifetime));

	header_len = snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: loongSSO(RC1.0)\r\nContent-Length: %u\r\nConnection: close\r\nContent-Type: image/png\r\nP3P: CP=\"CURa ADMa DEVa PSAo PSDo OUR BUS UNI PUR INT DEM STA PRE COM NAV OTC NOI DSP COR\"\r\nSet-Cookie: loongSSO=%llu; expires=%s; domain=%s\r\n\r\n", timebuf, size, key, cookie_buf, conf.domain);
	
	vectors[0].iov_base = header;
    vectors[0].iov_len  = header_len;
    vectors[1].iov_base = data;
    vectors[1].iov_len  = size;

	writev(conn->sfd, vectors, 2);

	gdFree(data);

	return 0;
}

//loongSSO ע��
int loong_sso_register(loong_conn *conn)
{
	char *email;
	char *password;
	const char *public_code;
	const char *private_code;
	unsigned char *username;
	
	uint64_t id, key;
	int json_len, query_len, rc;
	char json[200], query[400], code[34];
	
	struct loong_passwd info;

	email        = (char *)tcmapget2(conn->recs, "email");
	private_code = hash_get(codepool, conn->code);
	public_code  = tcmapget2(conn->recs, "code");
	username     = (char *)tcmapget2(conn->recs, "username");
	password     = (char *)tcmapget2(conn->recs, "password");

	if(username == NULL || !is_username(username))
	{
		//�û���֤ʧ��
		send_response(conn, HTTP_RESPONSE_USERNAME_NO, NULL);
		return 0;
	}
	else if(!is_user_exists(username))
	{
		//�û����Ѵ���
		send_response(conn, HTTP_RESPONSE_USERNAME_EXISTS, NULL);
		return 0;
	}
	else if(password == NULL || !is_password(password))
	{
		//������֤ʧ��
		send_response(conn, HTTP_RESPONSE_PASSWORD_NO, NULL);
		return 0;
	}
	else if(email == NULL || !is_mail(email))
	{
		//mail��֤ʧ��
		send_response(conn, HTTP_RESPONSE_EMAIL_NO, NULL);
		return 0;
	}
	else if(!is_mail_exists(email))
	{
		//mail�Ѵ���
		send_response(conn, HTTP_RESPONSE_EMAIL_EXISTS, NULL);
		return 0;
	}
	else if( (private_code == NULL) || (public_code == NULL) || (strcasecmp(private_code, public_code) != 0) )
	{
		//��֤ʧ��
		send_response(conn, HTTP_RESPONSE_VALIDATE_NO, NULL);
		return 0;
	}
	
	//ע��ɹ�
	hash_remove(codepool, conn->code);
	
	memset(code, 0, sizeof(code));
	memset(json, 0, sizeof(json));
	memset(query, 0, sizeof(query));
	
	
	MD5String(password, code);

	id        = ident_key();
	json_len  = snprintf(json, sizeof(json), "id:%llu|username:%s|mail:%s|ip:%u|time:%u|", id, username, email, ip2long(conn->ip), conn->now);
	query_len = snprintf(query, sizeof(query), "INSERT INTO `member` (`uid`, `username`, `password`, `email`, `ip`, `reg_time`, `c_status`)VALUES('%llu', '%s', '%s', '%s', %u, %u, 1);", id, username, code, email, ip2long(conn->ip), conn->now);
	

	info.id           = id;
	info.loong_status = 1;
	memcpy(info.pass, code, sizeof(info.pass));

	if(!tchdbput(loong_user, username, strlen(username), (char *)&(info), sizeof(struct loong_passwd)))
	{
		rc = tchdbecode(loong_user);
		printf("loong_user error: %s\r\n", tchdberrmsg(rc));
	}
	if(!tchdbput(loong_mail, email, strlen(email), (char *)&(info), sizeof(struct loong_passwd)))
	{
		rc = tchdbecode(loong_mail);
		printf("loong_mail error: %s\r\n", tchdberrmsg(rc));
	}
	if(!tchdbput(loong_info, (char *)&(id), sizeof(uint64_t), json, json_len))
	{
		rc = tchdbecode(loong_info);
		printf("loong_info error: %s\r\n", tchdberrmsg(rc));
	}
	
	rc = mysql_real_query(dbh, query, query_len);
	if(rc)
	{
		printf("Error making query: %s\n", mysql_error(dbh));
	}

	send_response(conn, HTTP_RESPONSE_REGISTER_OK, json);
	return 1;
}

//�û���½
// /?module=login&type=mail&mail=xxxx&password=xxxx
// /?module=login&type=user&username=xxxx&password=xxxx
int loong_sso_login(loong_conn *conn)
{
	int rc;
	char code[34];
	struct loong_passwd *info;
	char *val, *password, *json;
	const char *type, *mail, *username;
	
	memset(code, 0, sizeof(code));
	type     = tcmapget2(conn->recs, "type");
	mail     = tcmapget2(conn->recs, "mail");
	password = (char *)tcmapget2(conn->recs, "password");
	username = tcmapget2(conn->recs, "username");


	if(type == NULL)
	{
		//δ֪����
		send_response(conn, HTTP_RESPONSE_UNKNOWN_TYPE, NULL);
		return 0;
	}
	else if(strcasecmp(type, "mail") == 0)
	{
		if((mail == NULL) || !(val = tchdbget2(loong_mail, mail)))
		{
			//mail������
			send_response(conn, HTTP_RESPONSE_EMAIL_NO, NULL);
			return 0;
		}
	}
	else if(strcasecmp(type, "user") == 0)
	{
		if((username == NULL) || !(val = tchdbget2(loong_user, username)))
		{
			//�û���������
			send_response(conn, HTTP_RESPONSE_USERNAME_NO, NULL);
			return 0;
		}
	}
	else
	{
		//δ֪����
		send_response(conn, HTTP_RESPONSE_UNKNOWN_TYPE, NULL);
		return 0;
	}
	
	MD5String(password, code);
	info = (struct loong_passwd *)val;
	if( (password == NULL) || (strcasecmp(info->pass, code) != 0) )
	{
		//���벻ƥ��
		send_response(conn, HTTP_RESPONSE_PASSWORD_NO, NULL);
		free(val);
		return 0;
	}

	if(!(json = tchdbget(loong_info, (char *)&(info->id), sizeof(uint64_t), &rc)))
	{
		//printf("dpget: %s\n", dperrmsg(dpecode));
		//�û���������
		send_response(conn, HTTP_RESPONSE_USERNAME_NO, NULL);
		return 0;
	}
	
	//��½�ɹ�
	send_response(conn, HTTP_RESPONSE_LOGIN_OK, json);
	free(val);
	free(json);
	return 1;
}

//�û���֤
// /?module=check&type=mail&data=xxxx
// /?module=check&type=user&data=xxxx
int loong_sso_check(loong_conn *conn)
{
	char *type, *data;

	type = (char *)tcmapget2(conn->recs, "type");
	data = (char *)tcmapget2(conn->recs, "data");
	
	if(type == NULL)
	{
		send_response(conn, HTTP_RESPONSE_UNKNOWN_TYPE, NULL);
	}
	else if(strcasecmp(type, "mail") == 0)
	{
		if(data == NULL || !is_mail_exists(data))
		{
			send_response(conn, HTTP_RESPONSE_EMAIL_OK, NULL);
		}
		else
		{
			send_response(conn, HTTP_RESPONSE_EMAIL_NO, NULL);
		}
	}
	else if(strcasecmp(type, "user") == 0)
	{
		if(data == NULL || !is_user_exists(data))
		{
			send_response(conn, HTTP_RESPONSE_USERNAME_OK, NULL);
		}
		else
		{
			send_response(conn, HTTP_RESPONSE_USERNAME_NO, NULL);
		}
	}
	else
	{
		send_response(conn, HTTP_RESPONSE_UNKNOWN_TYPE, NULL);
	}

	return 1;
}


//�û����� �û���Ϣ
// /?module=update
int loong_sso_update(loong_conn *conn)
{
	send_response(conn, HTTP_RESPONSE_LOGOUT_OK, NULL);
	return 1;
}

//ɾ���û���Ϣ
// /?module=delete
int loong_sso_delete(loong_conn *conn)
{
	char code[35];
	char str[100];
	char query[128];
	unsigned int ind;
	my_ulonglong rows;
	int  i, query_len, rc;
	struct loong_site *recs;
	const char *mode, *uid, *sign;
	
	uid  = tcmapget2(conn->recs, "uid");
	mode = tcmapget2(conn->recs, "mode");
	sign = tcmapget2(conn->recs, "sign");

	if(uid == NULL || mode == NULL || sign == NULL)
	{
		send_response(conn, HTTP_RESPONSE_DELETE_NO, NULL);
		return 0;
	}
	
	recs = conf.site;
	for(i=0; i<conf.num; i++)
	{
		if(strcasecmp(mode, recs[i].id) == 0)
		{
			memset(&str, 0, sizeof(str));
			memset(&code, 0, sizeof(code));
			
			snprintf(str, sizeof(str), "%s|%s", recs[i].update_key, uid);
			MD5String(str, code);

			if(strcasecmp(sign, code) == 0)
			{
				ind       = strhash(uid);
				query_len = snprintf(query, sizeof(query), "DELETE FROM member_%d WHERE uid = '%s'", (ind % TABLE_CHUNK), uid);
				rc = mysql_real_query(dbh, query, query_len);
				if(rc)
				{
					printf("Error making query: %s\n", mysql_error(dbh));
					send_response(conn, HTTP_RESPONSE_SIGN_NO, NULL);
					return 1;
				}

				rows = mysql_affected_rows(conn);
				
				if(rows == 0)
				{
					//ɾ���û���Ϣ
					tchdbout(loong_info, (char *)&(id), sizeof(uint64_t));
					tchdbout(loong_info, (char *)&(id), sizeof(uint64_t));
					tchdbout(loong_info, (char *)&(id), sizeof(uint64_t));
				}

				send_response(conn, HTTP_RESPONSE_DELETE_OK, NULL);
				return 1;
			}
			else
			{
				send_response(conn, HTTP_RESPONSE_SIGN_NO, NULL);
				return 1;
			}
		}
	}

	send_response(conn, HTTP_RESPONSE_USERNAME_NOT_EXISTS, NULL);
	return 1;
}


//�û��˳�
// /?module=logout
int loong_sso_logout(loong_conn *conn)
{
	send_response(conn, HTTP_RESPONSE_LOGOUT_OK, NULL);
	return 1;
}

TCMAP *fetch_user_info(char *uid)
{
	TCMAP *data;
	MYSQL_RES *res; 
	unsigned int  ind;
	unsigned long rows; 
	int  query_len, rc;
	char query[256] = {0};
	
	ind       = strhash(uid);
	query_len = snprintf(query, sizeof(query), "SELECT `username`, `password`, `email`, `ip`, `sex`, `reg_time`, `c_status` FROM member_%d WHERE `uid` = '%s'", (ind % TABLE_CHUNK), uid);
	rc        = mysql_real_query(dbh, query, query_len);
	if(rc) return NULL;

	res       = mysql_store_result(dbh); 
	if(res == NULL) return NULL;

	rows      = mysql_num_rows(res); 
	if(num_rows == 0) return NULL;

	mysql_free_result(res); 
}