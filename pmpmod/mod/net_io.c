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
#$HeadURL: http://127.0.0.1/svn/repos/pmp_mod/trunk/pmpmod/mod/net_io.c $
#$Id: net_io.c 6 2006-02-18 19:16:19Z david $
#################################################################################
*/



#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <psputility.h>
#include <psputils.h>
#include <sys/select.h>
#include <errno.h>
#include <curl/curl.h>
#include <time.h>
#include "iniparser.h"
#include "net_io.h"

#define MODULE_NAME "net_io"
#define printf pspDebugScreenPrintf

static struct __netfd  __net_io_state;


char *net_io_uri;
int net_io_enabled;
int net_io_con;
int net_io_debug;
extern char psp_full_path[1024 + 1];


int net_io_connect(int config)
{
    int err;
    int stateLast = -1;
    err = sceNetApctlConnect(config);
    if (err != 0){
	printf(MODULE_NAME ": sceNetApctlConnect returns %08X\n", err);
	fprintf(stderr,MODULE_NAME ": sceNetApctlConnect returns %08X\n", err);
	return -1;
    }
    printf("Connecting to Access Point...\n");
    fprintf(stdout,"Connecting to Access Point...\n");
    while (1){
	int state;
	err = sceNetApctlGetState(&state);
	if (err != 0){
	    printf(MODULE_NAME ": sceNetApctlGetState returns $%x\n", err);
	    fprintf(stderr,MODULE_NAME ": sceNetApctlGetState returns $%x\n", err);
	    break;
	}
	if (state > stateLast){
	    printf("  connection state %d of 4\n", state);
	    fprintf(stderr,"  connection state %d of 4\n", state);
	    stateLast = state;
	}
	if (state == 4)
	    break;  // connected with static IP
	    sceKernelDelayThread(50*1000); // 50ms
	}
	return err;
}


size_t callback_open(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char* buf = (char*)ptr;
    __net_io_state.fd = atoi(buf);
    if (net_io_debug)    
	fprintf(stdout,"callback_open: %i\n",__net_io_state.fd);
    return nmemb;	
}

size_t callback_lseek(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char* buf = (char*)ptr;
    __net_io_state.seek_offset = atoi(buf);
    if (net_io_debug)
	fprintf(stdout,"callback_lseek: %i\n", __net_io_state.read_offset);
    return nmemb;	
}

size_t callback_close(void *ptr, size_t size, size_t nmemb, void *stream)
{
    char* buf = (char*)ptr;
    __net_io_state.err = atoi(buf);
    if (net_io_debug)
	fprintf(stdout,"callback_close: %i\n", __net_io_state.err);
    return nmemb;	
}

size_t callback_read(void *ptr, size_t size, size_t nmemb, void *stream)
{
    memcpy(__net_io_state.read_buffer + __net_io_state.read_offset ,ptr, size*nmemb);
    __net_io_state.read_offset += size*nmemb ;
    //fprintf(stdout,"callback_read: %i\n", __net_io_state.read_offset);
    return nmemb;	
}

size_t callback_listdir(void *ptr, size_t size, size_t nmemb, void *stream)
{
    memcpy(__net_io_state.dir_buffer + __net_io_state.dir_offset ,ptr, size*nmemb);
    __net_io_state.dir_offset += size*nmemb ;
    return nmemb;	
}

size_t callback_stat(void *ptr, size_t size, size_t nmemb, void *stream)
{
    memcpy(__net_io_state.stat_buffer + __net_io_state.stat_offset ,ptr, size*nmemb);
    __net_io_state.stat_offset += size*nmemb ;
    return nmemb;	
}

size_t callback_fstat(void *ptr, size_t size, size_t nmemb, void *stream)
{
    memcpy(__net_io_state.fstat_buffer + __net_io_state.fstat_offset ,ptr, size*nmemb);
    __net_io_state.fstat_offset += size*nmemb ;
    return nmemb;	
}




int net_io_init()
{
    int err;
    char config_file[1024];
    dictionary	*ini ;
      
    if (__net_io_state.init)
	return -1;

    strcpy( config_file, psp_full_path );
    strcpy( config_file + strlen(config_file), "/pmp.ini");

    ini = iniparser_load(config_file);
    
    net_io_con = iniparser_getint(ini, "net:con",0);
    net_io_enabled = iniparser_getint(ini, "net:enabled",0);
    net_io_uri = iniparser_getstr(ini, "net:uri");
    net_io_debug = iniparser_getint(ini,"net:debug",0); 
    
    fprintf(stdout,"net_io_uri=%s\n",net_io_uri);
    fprintf(stdout,"net_io_enabled=%i\n",net_io_enabled);
    fprintf(stdout,"net_io_con=%i\n",net_io_con);
    fprintf(stdout,"net_io_debug=%i\n",net_io_debug);
   
    if (!net_io_enabled)
      return -1;
    
    
    if((err = pspSdkInetInit())){
	printf(MODULE_NAME ": Error, could not initialise the network %08X\n", err);
	fprintf(stderr,MODULE_NAME ": Error, could not initialise the network %08X\n", err);
	return -1;
    }
    if (net_io_connect(net_io_con)){
	printf(MODULE_NAME ": Error, could not connect to AP\n");
	fprintf(stderr,MODULE_NAME ": Error, could not connect to AP\n");
	return -1;
    }
    __net_io_state.sema_open	= sceKernelCreateSema("net_io_open", 0, 1, 1, NULL);
    __net_io_state.sema_read	= sceKernelCreateSema("net_io_read", 0, 1, 1, NULL);
    __net_io_state.sema_lseek	= sceKernelCreateSema("net_io_lseek", 0, 1, 1, NULL);
    __net_io_state.sema_close	= sceKernelCreateSema("net_io_close", 0, 1, 1, NULL);
    __net_io_state.sema_listdir = sceKernelCreateSema("net_io_listdir", 0, 1, 1, NULL);
    __net_io_state.sema_stat	= sceKernelCreateSema("net_io_stat", 0, 1, 1, NULL);
    __net_io_state.sema_fstat   = sceKernelCreateSema("net_io_fstat", 0, 1, 1, NULL);
    __net_io_state.init = 1;
    return 0;
}

void net_io_shutdown()
{
    if (!__net_io_state.init)
	return;
    fprintf(stdout,"net_io_shutdown\n");
    if (!(__net_io_state.sema_open	< 0)) sceKernelDeleteSema(__net_io_state.sema_open);
    if (!(__net_io_state.sema_read	< 0)) sceKernelDeleteSema(__net_io_state.sema_read);
    if (!(__net_io_state.sema_lseek	< 0)) sceKernelDeleteSema(__net_io_state.sema_lseek);
    if (!(__net_io_state.sema_close	< 0)) sceKernelDeleteSema(__net_io_state.sema_close);
    if (!(__net_io_state.sema_listdir	< 0)) sceKernelDeleteSema(__net_io_state.sema_listdir);
    if (!(__net_io_state.sema_stat	< 0)) sceKernelDeleteSema(__net_io_state.sema_stat);
    if (!(__net_io_state.sema_fstat	< 0)) sceKernelDeleteSema(__net_io_state.sema_fstat);
}


netfd *net_io_open(const char *filename)
{
    netfd	*fd;
    char	url[512];
    char	*urldecoded_filename;
    int		ret;
    
    net_io_init();
    
    fd = malloc(sizeof(netfd));
    fd-> curl = curl_easy_init();
    
    if (strstr(filename,"net:/"))
	filename = filename + 5;
 
    urldecoded_filename = curl_escape(filename,0); 
    snprintf(url,511,"http://%s/open?filename=%s&flags=O_RDONLY",net_io_uri,urldecoded_filename);
    curl_free(urldecoded_filename);
    if (net_io_debug) 
	fprintf(stdout,"net_io_open: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(fd->curl, CURLOPT_MUTE, 1);
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_open);  
    (void) sceKernelWaitSema(__net_io_state.sema_open, 1, 0);
    __net_io_state.fd = -1;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.fd; 
    (void) sceKernelSignalSema(__net_io_state.sema_open,1);
    
    if (fd->res != CURLE_OK) {
	printf("net_io_open: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_open: %s\n", curl_easy_strerror(fd->res));
	free(fd);
	return NULL;
    }		    
    if (ret) {
	fd->fd = ret;
	return fd;
    }
    printf("net_io_open: no fd\n");
    fprintf(stderr,"net_io_open: no fd\n");
    free(fd);
    return NULL;
}

int net_io_close( netfd *fd)
{
    
    char url[512];
    int ret;
    if (!fd){
	printf("net_id_close: invalid fd\n");
	fprintf(stderr,"net_id_close: invalid fd\n");
	return -1;
    }
    snprintf(url,511,"http://%s/close?fd=%i",net_io_uri,fd->fd);
    if (net_io_debug)
	fprintf(stdout,"net_io_close: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_close);  
    (void) sceKernelWaitSema(__net_io_state.sema_close, 1, 0);
    __net_io_state.err = -1;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.err;
    (void) sceKernelSignalSema(__net_io_state.sema_close,1); 
    if (fd->res != CURLE_OK) {
	printf("net_io_close: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_close: %s\n", curl_easy_strerror(fd->res));
	return -1;
    }		    
    if (ret == 0 ){
	curl_easy_cleanup(fd->curl);
	free(fd);
	return 0;
    }
    return -1;
}

int net_io_read( netfd *fd, void *read_buffer, int size )
{
    
    char url[512];
    int ret;
    if (!fd){
	printf("net_io_read: invalid fd\n");
	fprintf(stderr,"net_io_read: invalid fd\n");
	return -1;
    }
    snprintf(url,511,"http://%s/read?fd=%i&size=%i",net_io_uri,fd->fd,size);
    if (net_io_debug) 
	fprintf(stdout,"net_io_read: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_read);  
    (void) sceKernelWaitSema(__net_io_state.sema_read, 1, 0);
    __net_io_state.read_offset = 0;
    __net_io_state.read_buffer = read_buffer;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.read_offset;
    (void) sceKernelSignalSema(__net_io_state.sema_read,1);
     
    if (fd->res != CURLE_OK) {
	printf("net_io_read: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_read: %s\n", curl_easy_strerror(fd->res));
	return -1;
    }
    curl_easy_getinfo(fd->curl,CURLINFO_SPEED_DOWNLOAD,&fd->speed); 
     __net_io_state.read_speed = fd->speed;
    return ret; 
} 

int net_io_lseek( netfd *fd,int offset, int whence)
{
    
    char url[512];
    int ret; 
    if (!fd){
	printf("net_io_lseek: invalid fd\n");
	fprintf(stderr,"net_io_lseek: invalid fd\n");
	return -1;
    }
    snprintf(url,511,"http://%s/lseek?fd=%i&offset=%i&whence=%i",net_io_uri,fd->fd,offset,whence);
    if (net_io_debug) 
	fprintf(stdout,"net_io_lseek: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_lseek);  
    (void) sceKernelWaitSema(__net_io_state.sema_lseek, 1, 0);
    __net_io_state.seek_offset = 0;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.seek_offset;
    (void) sceKernelSignalSema(__net_io_state.sema_lseek,1);
    if (fd->res != CURLE_OK) {
	printf("net_io_lseek: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_lseek: %s\n", curl_easy_strerror(fd->res));
	return -1;
    }
    return ret; 
} 

int net_io_listdir(char *path,void *dir_buffer,int max_buffer_size){
    
    char url[512];
    char *urldecoded_path;
    int ret;
    netfd *fd;
    net_io_init();
    fd = malloc(sizeof(netfd));
    fd->curl = curl_easy_init();
    urldecoded_path = curl_escape(path,0); 
    snprintf(url,511,"http://%s/listdir?path=%s&size=%i",net_io_uri,urldecoded_path,max_buffer_size);
    curl_free(urldecoded_path);
    if (net_io_debug)
	fprintf(stdout,"net_io_listdir: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_listdir);  
    (void) sceKernelWaitSema(__net_io_state.sema_listdir, 1, 0);
    __net_io_state.dir_offset = 0;
    __net_io_state.dir_buffer = dir_buffer;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.dir_offset;
    (void) sceKernelSignalSema(__net_io_state.sema_listdir,1);
     
    if (fd->res != CURLE_OK) {
	printf("net_io_listdir: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_listdir: %s\n", curl_easy_strerror(fd->res));
	curl_easy_cleanup(fd->curl);
	free(fd);
	return -1;
    }
    curl_easy_cleanup(fd->curl);
    free(fd);
    return ret; 
} 

int net_io_stat(char *filename,char *stat_buffer)
{
    
    char url[512];
    char *urldecoded_filename;
    int ret;
    netfd *fd;
    net_io_init();
    fd = malloc(sizeof(netfd));
    fd->curl = curl_easy_init();
    if (strstr(filename,"net:/"))
	filename = filename + 5;
    urldecoded_filename = curl_escape(filename,0); 
    snprintf(url,511,"http://%s/stat?filename=%s",net_io_uri,urldecoded_filename);
    curl_free(urldecoded_filename);
    if (net_io_debug)
	fprintf(stdout,"net_io_stat: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_stat);  
    (void) sceKernelWaitSema(__net_io_state.sema_stat, 1, 0);
    __net_io_state.stat_offset = 0;
    __net_io_state.stat_buffer = stat_buffer;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.stat_offset;
    (void) sceKernelSignalSema(__net_io_state.sema_stat,1);
     
    if (fd->res != CURLE_OK) {
	printf("net_io_stat: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_io_stat: %s\n", curl_easy_strerror(fd->res));
	curl_easy_cleanup(fd->curl);
	free(fd);
	return -1;
    }
    curl_easy_cleanup(fd->curl);
    free(fd);
    return ret; 
} 

int net_io_fstat( netfd *fd, char *fstat_buffer)
{
    
    char url[512];
    int ret;
    if (!fd){
	printf("net_io_fstat: invalid fd\n");
	fprintf(stderr,"net_io_fstat: invalid fd\n");
	return -1;
    }
    snprintf(url,511,"http://%s/fstat?fd=%i",net_io_uri,fd->fd);
    if (net_io_debug) 
	fprintf(stdout,"net_io_fstat: url=%s\n",url);
    curl_easy_setopt(fd->curl, CURLOPT_URL, url );
    curl_easy_setopt(fd->curl, CURLOPT_WRITEFUNCTION, callback_fstat);  
    (void) sceKernelWaitSema(__net_io_state.sema_fstat, 1, 0);
    __net_io_state.fstat_offset = 0;
    __net_io_state.fstat_buffer = fstat_buffer;
    fd->res = curl_easy_perform(fd->curl);
    ret = __net_io_state.fstat_offset;
    (void) sceKernelSignalSema(__net_io_state.sema_fstat,1);
     
    if (fd->res != CURLE_OK) {
	printf("net_id_fstat: %s\n", curl_easy_strerror(fd->res));
	fprintf(stderr,"net_id_fstat: %s\n", curl_easy_strerror(fd->res));
	return -1;
    }
    return ret; 
} 



enum protocols  net_io_isnet(char * filename)
{
    if (!(strncmp(filename,"net:/",5)))
	return NET_IO;
    
    return FILE_IO;
	
}

int net_io_stat_get_size(char * stat_buffer)
{
    int st_size; 
    int val;
    sscanf(stat_buffer,"%i %i %i %i %i %i %i ",&val,&val,&val,&val,&val,&val,&st_size);
    return st_size;
}

int net_io_stat_get_mtime(char *stat_buffer, ScePspDateTime *mtime)
{
    int val;
    int st_mtime;
    struct tm *t;
    sscanf(stat_buffer,"%i %i %i %i %i %i %i %i %i",&val,&val,&val,&val,&val,&val,&val,&val,&st_mtime); 
    t = gmtime((const time_t *)&st_mtime);
    mtime->year      = 1900 + t->tm_year;
    mtime->month     = t->tm_mon;
    mtime->day       = t->tm_mday;
    mtime->hour      = t->tm_hour;
    mtime->minute    = t->tm_min;
    mtime->second    = t->tm_sec;
    return 0;
}


int net_io_read_get_speed()
{
    return __net_io_state.read_speed; 
}


