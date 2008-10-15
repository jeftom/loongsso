#ifndef __HASHMAP_H__
#define __HASHMAP_H__


typedef struct hashmap hashmap;

struct record 
{
    char    path[128];       //�ļ�·��
    void    *content;        //�ļ�����
	time_t  file_time;       //�ļ�������޸�ʱ��
	time_t  visit_time;      //��󱻷��͵�ʱ��
	unsigned int length;     //�ļ����ݵĴ�С
	unsigned int visit;      //�����͵Ĵ���
	unsigned int hash;
};

struct hashmap 
{
	uint64_t length;
    struct record *records;
    unsigned int records_count;
    unsigned int size_index;
};

void hashmap_destroy(hashmap *h);

uint64_t hashmap_length(hashmap *h);

hashmap *hashmap_new(unsigned int capacity);

int hashmap_remove(hashmap *h, const char *path);

const struct record *hashmap_get(hashmap *h, const char *path);

int hashmap_add(hashmap *h, char *path, void *content, unsigned int length, time_t file_time);

#endif
