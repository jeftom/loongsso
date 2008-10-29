#ifndef __HASHMAP_H__
#define __HASHMAP_H__


typedef struct hashmap hashmap;

struct record 
{
    char    path[100];       //�ļ�·��
	time_t  file_time;       //�ļ�������޸�ʱ��
	time_t  visit_time;      //��󱻷��͵�ʱ��
	unsigned int length;     //�ļ����ݵĴ�С
	unsigned int visit;      //�����͵Ĵ���
	unsigned int hash;
};

struct hashmap 
{
    struct record  *records;
	unsigned int   size_index;
    unsigned int   records_count;
};

void hashmap_destroy(hashmap *h);

hashmap *hashmap_new(unsigned int capacity);

int hashmap_remove(hashmap *h, const char *path);

const struct record *hashmap_get(hashmap *h, const char *path);

int hashmap_add(hashmap *h, char *path, unsigned int length, time_t file_time);

#endif
