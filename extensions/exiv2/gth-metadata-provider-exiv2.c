/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  GThumb
 *
 *  Copyright (C) 2009 Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include <string.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gthumb.h>
#include "exiv2-utils.h"
#include "gth-metadata-provider-exiv2.h"


#define GTH_METADATA_PROVIDER_EXIV2_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTH_TYPE_METADATA_PROVIDER_EXIV2, GthMetadataProviderExiv2Private))


struct _GthMetadataProviderExiv2Private {
	int dummy;
};


static GthMetadataProviderClass *parent_class = NULL;


static void
gth_metadata_provider_exiv2_read (GthMetadataProvider *self,
				  GthFileData         *file_data,
				  const char          *attributes)
{
	GFileAttributeMatcher *matcher;

	matcher = g_file_attribute_matcher_new (attributes);

	if (g_file_attribute_matcher_matches (matcher, "Exif::*,Iptc::*,Xmp::*")) {
		char        *uri;
		char        *uri_wo_ext;
		char        *sidecar_uri;
		GthFileData *sidecar_file_data;

		/* this function is executed in a secondary thread, so calling
		 * slow sync functions, such as obtain_local_file, is not a
		 * problem. */

		exiv2_read_metadata (file_data->file, file_data->info);

		/* sidecar data */

		uri = g_file_get_uri (file_data->file);
		uri_wo_ext = _g_uri_remove_extension (uri);
		sidecar_uri = g_strconcat (uri_wo_ext, ".xmp", NULL);
		sidecar_file_data = gth_file_data_new_for_uri (sidecar_uri, NULL);
		if (g_file_query_exists (sidecar_file_data->file, NULL)) {
			gth_file_data_update_info (sidecar_file_data, "time::*");
			if (g_file_query_exists (sidecar_file_data->file, NULL))
				exiv2_read_sidecar (sidecar_file_data->file, file_data->info);
		}

		g_object_unref (sidecar_file_data);
		g_free (sidecar_uri);
		g_free (uri_wo_ext);
		g_free (uri);
	}

	g_file_attribute_matcher_unref (matcher);
}


static void
gth_metadata_provider_exiv2_write (GthMetadataProvider *self,
				   GthFileData         *file_data,
				   const char          *attributes)
{
}


static void
gth_metadata_provider_exiv2_finalize (GObject *object)
{
	/*GthMetadataProviderExiv2 *comment = GTH_METADATA_PROVIDER_EXIV2 (object);*/

	G_OBJECT_CLASS (parent_class)->finalize (object);
}


static GObject *
gth_metadata_provider_constructor (GType                  type,
				   guint                  n_construct_properties,
				   GObjectConstructParam *construct_properties)
{
	GthMetadataProviderClass *klass;
	GObjectClass             *parent_class;
	GObject                  *obj;
	GthMetadataProvider      *self;

	klass = GTH_METADATA_PROVIDER_CLASS (g_type_class_peek (GTH_TYPE_METADATA_PROVIDER));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = GTH_METADATA_PROVIDER (obj);

	g_object_set (self, "readable-attributes", "Exif::*,Xmp::*,Iptc::*", NULL);

	return obj;
}


static void
gth_metadata_provider_exiv2_class_init (GthMetadataProviderExiv2Class *klass)
{
	parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GthMetadataProviderExiv2Private));

	G_OBJECT_CLASS (klass)->finalize = gth_metadata_provider_exiv2_finalize;
	G_OBJECT_CLASS (klass)->constructor = gth_metadata_provider_constructor;

	GTH_METADATA_PROVIDER_CLASS (klass)->read = gth_metadata_provider_exiv2_read;
	GTH_METADATA_PROVIDER_CLASS (klass)->write = gth_metadata_provider_exiv2_write;
}


static void
gth_metadata_provider_exiv2_init (GthMetadataProviderExiv2 *catalogs)
{
}


GType
gth_metadata_provider_exiv2_get_type (void)
{
	static GType type = 0;

	if (! type) {
		GTypeInfo type_info = {
			sizeof (GthMetadataProviderExiv2Class),
			NULL,
			NULL,
			(GClassInitFunc) gth_metadata_provider_exiv2_class_init,
			NULL,
			NULL,
			sizeof (GthMetadataProviderExiv2),
			0,
			(GInstanceInitFunc) gth_metadata_provider_exiv2_init
		};

		type = g_type_register_static (GTH_TYPE_METADATA_PROVIDER,
					       "GthMetadataProviderExiv2",
					       &type_info,
					       0);
	}

	return type;
}