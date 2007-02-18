/*
 * Sylpheed -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 2004-2007 Hiroyuki Yamamoto & the Claws Mail team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

typedef struct GPGConfig GPGConfig;
typedef struct GPGAccountConfig GPGAccountConfig;

typedef enum {
	SIGN_KEY_DEFAULT,
	SIGN_KEY_BY_FROM,
	SIGN_KEY_CUSTOM
} SignKeyType;

#include "prefs_account.h"

struct GPGConfig
{
	gboolean	 auto_check_signatures;
	gboolean	 use_gpg_agent;
	gboolean	 store_passphrase;
	gint		 store_passphrase_timeout;
	gboolean	 passphrase_grab;
	gboolean	 gpg_warning;
	gboolean	 gpg_ask_create_key;
};

struct GPGAccountConfig
{
	SignKeyType	 sign_key;
	gchar 		*sign_key_id;
};

void prefs_gpg_init(void);
void prefs_gpg_done(void);
void prefs_gpg_save_config(void);
struct GPGConfig *prefs_gpg_get_config(void);
struct GPGAccountConfig *prefs_gpg_account_get_config(PrefsAccount *account);
void prefs_gpg_account_set_config(PrefsAccount *account, GPGAccountConfig *config);
void prefs_gpg_account_free_config(GPGAccountConfig *config);
