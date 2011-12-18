/* Copyright Abandoned 1996, 1999, 2001 MySQL AB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Version numbers for protocol & mysqld */

#ifdef _CUSTOMCONFIG_
#include <custom_conf.h>
#else
#define PROTOCOL_VERSION		10
#define MYSQL_SERVER_VERSION		"3.23.40"
#define MYSQL_SERVER_SUFFIX		""
#define FRM_VER				6
#define MYSQL_VERSION_ID		32333
#define MYSQL_PORT			3306
#define MYSQL_UNIX_ADDR			"\\socket\\MySQL"

/* mysqld compile time options */
#ifndef MYSQL_CHARSET
#define MYSQL_CHARSET			"latin1"
#endif
#endif
