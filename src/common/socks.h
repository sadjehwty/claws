/*
 * Claws Mail -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2010 Hiroyuki Yamamoto
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOCKS_H__
#define __SOCKS_H__

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>

#include "socket.h"

typedef struct _SocksInfo SocksInfo;

typedef enum {
	SOCKS_SOCKS4,
	SOCKS_SOCKS5
} SocksType;

struct _SocksInfo
{
	SocksType type;
	gchar *proxy_host;
	gushort proxy_port;
	gchar *proxy_name;
	gchar *proxy_pass;
};

SocksInfo *socks_info_new(SocksType type, const gchar *proxy_host,
			  gushort proxy_port, const gchar *proxy_name,
			  const gchar *proxy_pass);
void socks_info_free(SocksInfo *socks_info);

gint socks_connect(SockInfo *sock, const gchar *hostname, gushort port,
		   SocksInfo *socks_info);

gint socks4_connect(SockInfo *sock, const gchar *hostname, gushort port);
gint socks5_connect(SockInfo *sock, const gchar *hostname, gushort port,
		    const gchar *proxy_name, const gchar *proxy_pass);

#endif /* __SOCKS_H__ */
