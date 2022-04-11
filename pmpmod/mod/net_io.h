/*
Network File IO
Copyright (C) 2006 David Voswinkel

Homepage: http://optixx.org
E-mail:   david@optixx.org

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
#################################################################################
#$LastChangedDate: 2006-02-18 20:16:19 +0100 (Sa, 18 Feb 2006) $
#$LastChangedRevision: 6 $
#$LastChangedBy: david $
#$HeadURL: http://127.0.0.1/svn/repos/pmp_mod/trunk/pmpmod/mod/net_io.h $
#$Id: net_io.h 6 2006-02-18 19:16:19Z david $
#################################################################################
*/


#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <sys/select.h>
#include <errno.h>
#include <curl/curl.h>

#define NET_IO_MAX_DIR_BUFFER   8192
#define NET_IO_DEBUG		1

#ifndef net_io_h
#define net_io_h

enum protocols  { FILE_IO = 0,NET_IO = 1};

typedef struct netfd {
    CURL	*curl;
    CURLcode	res;
    int		fd;	
    double	speed;
} netfd;

struct __netfd {
    int init; 
    int	fd;
    int	seek_offset;
    int err;
    int read_offset;
    void * read_buffer;
    int  read_speed;
    int  dir_offset;
    void * dir_buffer;
    int  stat_offset;
    void *stat_buffer;
    int  fstat_offset;
    void *fstat_buffer;
    SceUID sema_open;
    SceUID sema_read;
    SceUID sema_lseek;
    SceUID sema_close;
    SceUID sema_listdir;
    SceUID sema_stat;
    SceUID sema_fstat;
} __netfd;

enum protocols  net_io_isnet(char *filename);

int net_io_connect(int config);
int net_io_init();
void net_io_shutdown();
netfd *net_io_open(const char *filename);
int net_io_close( netfd *fd);
int net_io_read( netfd *fd,void *read_buffer, int size);
int net_io_lseek( netfd *fd,int offset, int whence);
int net_io_listdir(char * path, void *dir_buffer,int max_buffer_size);
int net_io_fstat(netfd *fd, char *fstat_buffer);
int net_io_stat(char *filename, char *stat_buffer);
int net_io_stat_get_size(char * stat_buffer);
int net_io_stat_get_mtime(char *stat_buffer, ScePspDateTime *mtime);
int net_io_read_get_speed();




#endif
