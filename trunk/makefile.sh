CC= gcc
CFLAGS= -O3 -fno-guess-branch-probability -DHAVE_SYS_EPOLL_H -DCHINA_USERNAME
INCLUDES= -I.
LIBS= -L.

OBJS= bubble_sort.o insert_sort.o main.o select_sort.o utility.o shell_sort.o





# configure�ˤ�ä�Makefile.in�����ԄӵĤ����ɤ���ޤ�����
# Un*x Makefile for GNU tar program.
# Copyright (C) 1991 Free Software Foundation, Inc.

# ���Υץ����ϥե�`���եȥ������Ǥ���GNU GPL(General
# Public License)�ξ���˻��Ť��ʤ����ɤ����䲼?��������
# �������äƽY���Ǥ���...
...
...

SHELL = /bin/sh

#### �����ƥॳ��ե������`����󲿷֤��_ʼ ####

srcdir = .

# gcc��ʹ�����Ϥ�gcc�˸�����fixincludes������ץȤ�g��
# ���뤫��gcc��-traditional���ץ�����Ĥ��ƌg�Ф��뤫
# ���٤��Ǥ����������ʤ���ioctl���`�뤬�����_�˥���ѥ�
# �뤵��Ƥ��ޤ������ƥब����ޤ���
CC = gcc -O
YACC = bison -y
INSTALL = /usr/local/bin/install -c
INSTALLDATA = /usr/local/bin/install -c -m 644

# ���x�˼Ӥ�����ΤϴΤΤ�Τˤʤ�ޤ���
# -DSTDC_HEADERS        ANSI C�إå��ȥ饤�֥�꤬������ϡ�
# -DPOSIX               POSIX�إå��ȥ饤�֥�꤬������ϡ�
# -DBSD42               sys/dir.h(-DPOSOX��ʹ��ʤ���)��
#                       sys/file.h��`struct stat'��st_blocks
#                       ��������ϡ�
# -DUSG                 System V/ANSI C�����Фȥ����v��?
#                       �إå���sys/sysmacros.h ��fcntl.h ��
#                       getcwd�����ꡢvalloc�ʤ��ǡ�ndir.h
#                       (-DDIRENT��ʹ��ʤ�����)��������ϡ�
# -DNO_MEMORY_H         USG��STDC_HEADERS����memory.h�򥤥�
#                       ����`�ɤ��ʤ����ϡ�
# -DDIRENT              USG��ndir.h�δ�����dirent.h�ˤ����
#                       �ϡ�
# -DSIGTYPE=int         �����ʥ�ϥ�ɥ餬void�ǤϤʤ�int��
#                       �����ϡ�
# -DNO_MTIO             sys/mtio.h(magtape ioctls)���ʤ����ϡ�
# -DNO_REMOTE           ���`�ȥ����롢rexec���ʤ����ϡ�
# -DUSE_REXEC           �ե��`��rsh��remsh�Τ����˥��`�ȥ�
#                       �`�׌g�ФΤ����rexec��ʹ����
# -DVPRINTF_MISSING     vprintf�v�����ʤ�(��_doprint�Ϥ���)���ϡ�
# -DDOPRNT_MISSING      _doprnt�v�����ʤ����ϡ�-DVPRINTF_MISSING
#                       �ⶨ�x�����Ҫ������ޤ���
# -DFTIME_MISSING       ftime�����ƥॳ�`�뤬�ʤ����ϡ�
# -DSTRSTR_MISSING      strstr�v�����ʤ����ϡ�
# -DVALLOC_MISSING      valloc�v�����ʤ����ϡ�
# -DMKDIR_MISSING       mkdir��rmdir�����ƥॳ�`�뤬�ʤ����ϡ�
# -DRENAME_MISSING      rename�����ƥॳ�`�뤬�ʤ����ϡ�
# -DFTRUNCATE_MISSING   ftruncate�����ƥॳ�`�뤬�ʤ����ϡ�
# -DV7                  Version 7 UNIX�h��(�L��ԇ�Y�Ϥ���
#                       ���ޤ���)��
# -DEMUL_OPEN3          �������ĥЩ`������open���ʤ��ơ�
#                       ���ڤ��륷���ƥॳ�`��Ǥ���򥨥�
#                       ���`�Ȥ����������ϡ�
# -DNO_OPEN3            �������ĥЩ`������open���ʤ��ơ�
#                       ���ߥ��`�Ȱ�open�δ�����tar -k
#                       ���ץ�����o���ˤ����������ϡ�
# -DXENIX               sys/inode.h�����ꡢ�����94�򥤥�
#                       ����`�ɤ����Ҫ��������ϡ�
#

DEFS =  -DSIGTYPE=int -DDIRENT -DSTRSTR_MISSING \
        -DVPRINTF_MISSING -DBSD42
# NO_REMOTE���x���ƿդäݤˤ��ʤ��ä����Ϥˡ������
# rtapelib.o�˥��åȤ��ޤ���
RTAPELIB = rtapelib.o
LIBS =
DEF_AR_FILE = /dev/rmt8
DEFBLOCKING = 20

CDEBUG = -g
CFLAGS = $(CDEBUG) -I. -I$(srcdir) $(DEFS) \
        -DDEF_AR_FILE=\"$(DEF_AR_FILE)\" \
        -DDEFBLOCKING=$(DEFBLOCKING)
LDFLAGS = -g

prefix = /usr/local
# ���󥹥ȩ`�뤹��ץ����θ����ˤĤ���ץ�
# �ե��å����ǡ�ͨ���Ͽդäݤ�`g'�ˤʤ�ޤ���
binprefix =

# tar�򥤥󥹥ȩ`�뤹��ǥ��쥯�ȥꡣ
bindir = $(prefix)/bin

# Info�ե�����򥤥󥹥ȩ`�뤹��ǥ��쥯�ȥꡣ
infodir = $(prefix)/info

#### �����ƥॳ��ե������`����󲿷֤νK�� ####

SRC1 =  tar.c create.c extract.c buffer.c \
        getoldopt.c update.c gnu.c mangle.c
SRC2 =  version.c list.c names.c diffarch.c \
        port.c wildmat.c getopt.c
SRC3 =  getopt1.c regex.c getdate.y
SRCS =  $(SRC1) $(SRC2) $(SRC3)
OBJ1 =  tar.o create.o extract.o buffer.o \
        getoldopt.o update.o gnu.o mangle.o
OBJ2 =  version.o list.o names.o diffarch.o \
        port.o wildmat.o getopt.o
OBJ3 =  getopt1.o regex.o getdate.o $(RTAPELIB)
OBJS =  $(OBJ1) $(OBJ2) $(OBJ3)
AUX =   README COPYING ChangeLog Makefile.in  \
        makefile.pc configure configure.in \
        tar.texinfo tar.info* texinfo.tex \
        tar.h port.h open3.h getopt.h regex.h \
        rmt.h rmt.c rtapelib.c alloca.c \
        msd_dir.h msd_dir.c tcexparg.c \
        level-0 level-1 backup-specs testpad.c

all:    tar rmt tar.info

tar:    $(OBJS)
        $(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

rmt:    rmt.c
        $(CC) $(CFLAGS) $(LDFLAGS) -o $@ rmt.c

tar.info: tar.texinfo
        makeinfo tar.texinfo

install: all
        $(INSTALL) tar $(bindir)/$(binprefix)tar
        -test ! -f rmt || $(INSTALL) rmt /etc/rmt
        $(INSTALLDATA) $(srcdir)/tar.info* $(infodir)

$(OBJS): tar.h port.h testpad.h
regex.o buffer.o tar.o: regex.h
# getdate.y�ˤ�(���?�p�٤���)8�Ĥθ��Ϥ�����ޤ���
# (getdate.y has 8 shift/reduce conflicts.)

testpad.h: testpad
        ./testpad

testpad: testpad.o
        $(CC) -o $@ testpad.o

TAGS:   $(SRCS)
        etags $(SRCS)

clean:
        rm -f *.o tar rmt testpad testpad.h core

distclean: clean
        rm -f TAGS Makefile config.status

realclean: distclean
        rm -f tar.info*

shar: $(SRCS) $(AUX)
        shar $(SRCS) $(AUX) | compress \
          > tar-`sed -e '/version_string/!d' \
                     -e 's/[^0-9.]*\([0-9.]*\).*/\1/' \
                     -e q
                     version.c`.shar.Z

dist: $(SRCS) $(AUX)
        echo tar-`sed \
             -e '/version_string/!d' \
             -e 's/[^0-9.]*\([0-9.]*\).*/\1/' \
             -e q
             version.c` > .fname
        -rm -rf `cat .fname`
        mkdir `cat .fname`
        ln $(SRCS) $(AUX) `cat .fname`
        -rm -rf `cat .fname` .fname
        tar chZf `cat .fname`.tar.Z `cat .fname`

tar.zoo: $(SRCS) $(AUX)
        -rm -rf tmp.dir
        -mkdir tmp.dir
        -rm tar.zoo
        for X in $(SRCS) $(AUX) ; do \
            echo $$X ; \
            sed 's/$$/^M/' $$X \
            > tmp.dir/$$X ; done
        cd tmp.dir ; zoo aM ../tar.zoo *
        -rm -rf tmp.dir
