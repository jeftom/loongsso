#ifndef	_MODULE_H
#define	_MODULE_H

//loongSSO ��½ module.c
int loong_sso_login(loong_conn *conn);

//loongSSO �˳� module.c
int loong_sso_logout(loong_conn *conn);

//������֤�� module.c
int loong_sso_validate(loong_conn *conn);

//loongSSO ע�� module.c
int loong_sso_register(loong_conn *conn);

//loongSSO ��֤
int loong_sso_check(loong_conn *conn);

//ɾ���û���Ϣ
int loong_sso_delete(loong_conn *conn);

//�û����� �û���Ϣ
int loong_sso_update(loong_conn *conn);

#endif

