/*
 * Claws Mail -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 2016 Ricardo Mones and the Claws Mail Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#include "claws-features.h"
#endif

#include <glib.h>
#include <curl/curl.h>
#include <pthread.h>

#include <common/claws.h>
#include <prefs_common.h>

#include "libravatar.h"
#include "libravatar_prefs.h"
#include "libravatar_missing.h"
#include "libravatar_image.h"

static size_t write_image_data_cb(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	debug_print("received %zu bytes from avatar server\n", written);

	return written;
}

static GdkPixbuf *image_pixbuf_from_filename(const gchar *filename)
{
	GdkPixbuf *picture = NULL;
	GError *error = NULL;
	gint w, h;

	gdk_pixbuf_get_file_info(filename, &w, &h);

	if (w != AVATAR_SIZE || h != AVATAR_SIZE)
		/* server can provide a different size from the requested in URL */
		picture = gdk_pixbuf_new_from_file_at_scale(
				filename, AVATAR_SIZE, AVATAR_SIZE, TRUE, &error);
	else	/* exact size */
		picture = gdk_pixbuf_new_from_file(filename, &error);

	if (error != NULL) {
		g_warning("failed to load image '%s': %s", filename, error->message);
		g_error_free(error);
	} else {
		if (!picture)
			g_warning("failed to load image '%s': no error returned!", filename);
	}

	return picture;
}

static GdkPixbuf *pixbuf_from_url(const gchar *url, const gchar *md5, const gchar *filename) {
	GdkPixbuf *image = NULL;
	FILE *file;
	CURL *curl;
	long filesize;

	file = fopen(filename, "wb");
	if (file == NULL) {
		g_warning("could not open '%s' for writing", filename);
		return NULL;
	}
	curl = curl_easy_init();
	if (curl == NULL) {
		g_warning("could not initialize curl to get image from URL");
		fclose(file);
		return NULL;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_image_data_cb);
	/* make sure timeout is less than general IO timeout */
	curl_easy_setopt(curl, CURLOPT_TIMEOUT,
			(libravatarprefs.timeout == 0
				|| libravatarprefs.timeout
					> prefs_common_get_prefs()->io_timeout_secs)
			? prefs_common_get_prefs()->io_timeout_secs
			: libravatarprefs.timeout);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

	if (libravatarprefs.allow_redirects) {
		long maxredirs = (libravatarprefs.default_mode == DEF_MODE_URL)? 3L
			: ((libravatarprefs.default_mode == DEF_MODE_MM)? 2L: 1L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, maxredirs);
	}
	curl_easy_setopt(curl, CURLOPT_FILE, file);
	debug_print("retrieving URL to file: %s -> %s\n", url, filename);
	curl_easy_perform(curl);
	filesize = ftell(file);
	fclose(file);
	if (filesize < MIN_PNG_SIZE)
		debug_print("not enough data for an avatar image: %ld bytes\n", filesize);
	else
		image = image_pixbuf_from_filename(filename);

	if (!libravatarprefs.cache_icons || filesize == 0) {
		if (g_unlink(filename) < 0)
			g_warning("failed to delete cache file '%s'", filename);
	}

	if (filesize == 0)
		missing_add_md5(libravatarmisses, md5);

	curl_easy_cleanup(curl);

	return image;
}

static void *get_image_thread(void *arg) {
	AvatarImageFetch *ctx = (AvatarImageFetch *)arg;

	/* get image */
	ctx->pixbuf = pixbuf_from_url(ctx->url, ctx->md5, ctx->filename);
	/* done here */
	ctx->ready = TRUE;

	return arg;
}

GdkPixbuf *libravatar_image_fetch(AvatarImageFetch *ctx)
{
#ifdef USE_PTHREAD
	pthread_t pt;
#endif

	g_return_val_if_fail(ctx != NULL, NULL);

#ifdef USE_PTHREAD
	if (pthread_create(&pt, PTHREAD_CREATE_JOINABLE, get_image_thread, (void *)ctx) != 0) {
		debug_print("synchronous image fetching (couldn't create thread)\n");
		get_image_thread(ctx);
	} else {
		debug_print("waiting for thread completion\n");
		/*
		while (!ctx->ready ) {
			claws_do_idle();
		}
		*/
		pthread_join(pt, NULL);
		debug_print("thread completed\n");
	}
#else
	debug_print("synchronous image fetching (pthreads unavailable)\n");
	get_image_thread(ctx);
#endif
	if (ctx->pixbuf == NULL) {
		g_warning("could not get image");
	}
	return ctx->pixbuf;
}
