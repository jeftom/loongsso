#!/bin/sh

#--------- ��Щ������Ҫ�Լ��޸Ŀ�ʼ

#GD���·��
GD_PATH="/usr/local/gd2"

#MySQL���·��
DB_PATH="/usr/local/mysql"

#loong SSO��װ���Ǹ�Ŀ¼
ROOT_PATH="/home/lijinxing/test"

#������·��
CACHE_PATH="/usr/local/tokyocabinet"

#�û����Ƿ��������������, 1Ϊ�������Ӣ����ĸ������ 2Ϊ�������Ӣ����ĸ�����֡����� (Ĭ��Ϊ1)
USER_TYPE=1

#���ݿ��û���ķֱ���Ŀ
TABLE_CHUNK=10

#--------- ��Щ������Ҫ�Լ��޸Ľ���


if [ $USER_TYPE -eq 1 ]; then
	condition="-DALPHA_USERNAME";
elif [ $USER_TYPE -eq 2 ]; then
	condition="-DCHINA_USERNAME";
else
	echo "Please select USERNAME TYLE '1 or 2'";
	exit 0;
fi


OS=$(uname)


CFLAGS="-O2 -fno-guess-branch-probability -DHAVE_SYS_EVENT_H";
SRCS="./src/estring.c ./src/loong_log.c ./src/module.c ./src/protocol.c ./src/util.c ./src/evio_kqueue.c ./src/hash.c ./src/md5.c ./src/parse_conf.c  ./src/sxml.c";
OBJS="estring.o  md5.o  sxml.o loong_log.o module.o protocol.o util.o evio_kqueue.o hash.o parse_conf.o";


LIBS="-L${CACHE_PATH}/lib -L${GD_PATH}/lib -L${DB_PATH}/lib/mysql"
INCLUDE="-I./ -I./include -I${CACHE_PATH}/include -I${GD_PATH}/include -I${DB_PATH}/include/mysql"
MYLIB="-lmysqlclient -lgd -ltokyocabinet -lrt"


loong_define_config="\
#ifndef __CONFIG_H_
\r\n#define __CONFIG_H_
\r\n
\r\n#define TABLE_CHUNK         ${TABLE_CHUNK} 
\r\n#define WATER_MARK          \"loong SSO\"
\r\n#define SIGN_FONT_PATH      \"${ROOT_PATH}/augie.ttf\"
\r\n#define LOONG_INFO_PATH     \"${ROOT_PATH}/cache/loong_info.db\"
\r\n#define LOONG_MAIL_PATH     \"${ROOT_PATH}/cache/loong_mail.db\"
\r\n#define LOONG_USER_PATH     \"${ROOT_PATH}/cache/loong_user.db\"
\r\n#define LOONG_CONFIG_PATH   \"${ROOT_PATH}/conf/loong_sso.xml\"
\r\n#define LOONG_LOG_PATH      \"${ROOT_PATH}/log/loong_sso.log\"
\r\n\r\n#endif
\r\n"

echo $loong_define_config > _config.h

gcc -c ${SRCS} ${INCLUDE} ${CFLAGS} ${condition}; 
gcc -o loong ./src/loong.c ${SRCS} ${INCLUDE} ${CFLAGS} ${condition} ${LIBS} ${MYLIB}; 

if [ ! -d $ROOT_PATH ]; 
then 
	mkdir $ROOT_PATH; 
fi

if [ ! -d "${ROOT_PATH}/cache/" ]; 
then 
	mkdir "${ROOT_PATH}/cache/"; 
fi

if [ ! -d "${ROOT_PATH}/sbin/" ]; 
then 
	mkdir "${ROOT_PATH}/sbin/"; 
fi

if [ ! -d "${ROOT_PATH}/log/" ]; 
then 
	mkdir "${ROOT_PATH}/log/"; 
fi

cp loong "${ROOT_PATH}/sbin/"; 
cp -r ./conf/ $ROOT_PATH;  
cp -r ./font/ $ROOT_PATH;  
echo "Installed in ${ROOT_PATH}"; 
rm -rf $OBJS _config.h loong;

