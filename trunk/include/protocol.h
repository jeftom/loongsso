#ifndef	_PROTOCOL_H
#define	_PROTOCOL_H

//0 ����ʧ�� 1 �����ɹ�  ����httpЭ��ͷ  protocol.c
int parse_http_header(char *header, size_t header_len, loong_conn *conn);

#endif

