#ifndef	_PARSE_CONF_H
#define	_PARSE_CONF_H

#define SITE_CHUNK   10


struct loong_site
{
	char   id[30];
	char   key[30];
	char   update_key[30];

//------Ҫ�ص���url����
	char   del[128];
	char   login[128];
	char   logout[128];
	char   update[128];
};	

struct loong_config
{
	u_short server_id;
	u_short server_port;

	//mysql ��ز���
	size_t port;       //mysql�Ķ˿�
	size_t chunk;      //�ֱ�ĸ���
	char   host[30];   //mysql�ĵ�ַ
	char   user[30];   //mysql���û�����
	char   pass[30];   //mysql������
	char   dbname[30]; //mysql�����ݿ�����
	char   domain[30]; //loong SSO domain

	//SSO Ӧ�õĿͻ���

	size_t num;                 //�ͻ��˵ĸ���
	size_t used;                //���˶���
	struct loong_site *site;
};	

struct loong_config conf;


// public ��ʼ���������ļ�
int parse_conf(char *filename);

// public ע�������ļ����ڴ�
void parse_conf_destroy();
#endif

