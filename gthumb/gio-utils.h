/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  GThumb
 *
 *  Copyright (C) 2008 Free Software Foundation, Inc.
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

#ifndef _GIO_UTILS_H
#define _GIO_UTILS_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* callback types */

typedef enum { /*< skip >*/
	DIR_OP_CONTINUE,
	DIR_OP_SKIP,
	DIR_OP_STOP
} DirOp;

typedef DirOp (*StartDirCallback)    (GFile       *directory,
				      GFileInfo   *info,
				      GError     **error,
				      gpointer     user_data);
typedef void (*ForEachChildCallback) (GFile       *file,
				      GFileInfo   *info,
				      gpointer     user_data);
typedef void (*ForEachDoneCallback)  (GError      *error,
				      gpointer     data);
typedef void (*ListReadyCallback)    (GList       *files,
				      GList       *dirs,
				      GError      *error,
				      gpointer     user_data);
typedef void (*CopyProgressCallback) (goffset      current_file,
                                      goffset      total_files,
                                      GFile       *source,
                                      GFile       *destination,
                                      goffset      current_num_bytes,
                                      goffset      total_num_bytes,
                                      gpointer     user_data);
typedef void (*CopyDoneCallback)     (GError      *error,
				      gpointer     user_data);
typedef void (*BufferReadyCallback)  (void        *buffer,
				      gsize        count,
				      GError      *error,
				      gpointer     user_data);
typedef void (*InfoReadyCallback)    (GList       *files,
				      GError      *error,
				      gpointer     user_data);

/* asynchronous recursive list functions */

void   g_directory_foreach_child     (GFile                 *directory,
				      gboolean               recursive,
				      gboolean               follow_links,
				      const char            *attributes,
				      GCancellable          *cancellable,
				      StartDirCallback       start_dir_func,
				      ForEachChildCallback   for_each_file_func,
				      ForEachDoneCallback    done_func,
				      gpointer               user_data);
void   g_directory_list_async        (GFile                 *directory,
				      const char            *base_dir,
				      gboolean               recursive,
				      gboolean               follow_links,
				      gboolean               no_backup_files,
				      gboolean               no_dot_files,
				      const char            *include_files,
				      const char            *exclude_files,
				      const char            *exclude_folders,
				      gboolean               ignorecase,
				      GCancellable          *cancellable,
				      ListReadyCallback      done_func,
				      gpointer               done_data);
void   g_list_items_async            (GList                 *items,
				      const char            *base_dir,
				      GCancellable          *cancellable,
				      ListReadyCallback      done_func,
				      gpointer               done_data);
void   g_query_info_async            (GList                 *files,
				      const char            *attributes,
				      GCancellable          *cancellable,
				      InfoReadyCallback      ready_func,
				      gpointer               user_data);

/* asynchronous copy functions */

void   _g_dummy_file_op_async        (CopyDoneCallback       callback,
				      gpointer               user_data);
void   g_copy_files_async            (GList                 *sources,
				      GList                 *destinations,
				      GFileCopyFlags         flags,
				      int                    io_priority,
				      GCancellable          *cancellable,
				      CopyProgressCallback   progress_callback,
				      gpointer               progress_callback_data,
				      CopyDoneCallback       callback,
				      gpointer               user_data);
void   _g_copy_file_async            (GFile                 *source,
				      GFile                 *destination,
				      GFileCopyFlags         flags,
				      int                    io_priority,
				      GCancellable          *cancellable,
				      CopyProgressCallback   progress_callback,
				      gpointer               progress_callback_data,
				      CopyDoneCallback       callback,
				      gpointer               user_data);
void   g_directory_copy_async        (GFile                 *source,
				      GFile                 *destination,
				      GFileCopyFlags         flags,
				      int                    io_priority,
				      GCancellable          *cancellable,
				      CopyProgressCallback   progress_callback,
				      gpointer               progress_callback_data,
				      CopyDoneCallback       callback,
				      gpointer               user_data);
gboolean _g_delete_files             (GList                 *file_list,
				      gboolean               include_metadata,
				      GError               **error);
gboolean g_load_file_in_buffer       (GFile                 *file,
				      void                 **buffer,
				      gsize                 *size,
				      GError               **error);
void     g_load_file_async           (GFile                 *file,
				      int                    io_priority,
				      GCancellable          *cancellable,
				      BufferReadyCallback    callback,
				      gpointer               user_data);
gboolean g_write_file                (GFile                 *file,
				      gboolean               make_backup,
			              GFileCreateFlags       flags,
				      void                  *buffer,
				      gsize                  count,
				      GCancellable          *cancellable,
				      GError               **error);
void     g_write_file_async          (GFile                 *file,
				      void                  *buffer,
		    		      gsize                  count,
				      int                    io_priority,
				      GCancellable          *cancellable,
				      BufferReadyCallback    callback,
				      gpointer               user_data);
GFile * _g_file_create_unique        (GFile                 *parent,
				      const char            *display_name,
				      const char            *suffix,
				      GError               **error);
GFile * _g_directory_create_unique   (GFile                 *parent,
				      const char            *display_name,
				      const char            *suffix,
				      GError               **error);
GFileType
	_g_file_get_standard_type    (GFile      *file);

/* convenience macros */

/**
 * g_directory_list_all_async:
 * @directory:
 * @base_dir:
 * @recursive:
 * @cancellable:
 * @done_func:
 * @done_data:
 *
 */
#define g_directory_list_all_async(directory, base_dir, recursive, cancellable, done_func, done_data) \
    g_directory_list_async ((directory), (base_dir), (recursive), TRUE, FALSE, FALSE, NULL, NULL, NULL, FALSE, (cancellable), (done_func), (done_data))

gboolean _g_directory_make (GFile    *file,
			    guint32   unix_mode,
			    GError  **error);

G_END_DECLS

#endif /* _GIO_UTILS_H */