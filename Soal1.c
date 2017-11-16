#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/ramaadiw/Documents"

static int xmp_getattr(const char *path, struct stat *stbuf)
{

	int res;
	char fpath[1000];

  	sprintf(fpath,"%s%s",dirpath, path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int res = 0;
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(fpath);

	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));

		if(res!=0) break;
	}
	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	 //sama seperti di get atrribute, cuma ini buat bisa untuk di read berdasarkan nama aslinya

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else {
		sprintf(fpath, "%s%s",dirpath,path);
	}

	int res = 0;
	int fd = 0;
	(void) fi;
	
     	if(strstr(fpath,".doc") != 0 || strstr(fpath, ".pdf") != 0 || strstr(fpath, ".txt") != 0)
	{
		char file[100];
		char ext[50];
		int rm;		
		system("zenity --error --text='Terjadi kesalahan! File berisi konten berbahaya.'");		
		sprintf(file,"%s",fpath);
		sprintf(ext,"%s.ditandai",file);
		
		rm = rename(file, ext);
		if(rm == -1) 
			return -errno;
		return -1;
      	}
  	else{
		fd = open(fpath, O_RDONLY);

		if (fd == -1)
			return -errno;

		res = pread(fd, buf, size, offset);

		if (res == -1)
			res = -errno;

		close(fd);
	}
	return res;
}
