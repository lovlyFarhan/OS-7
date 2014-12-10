/*
FUSE: Filesystem in Userspace
Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

Source: fuse-2.8.7.tar.gz examples directory
http://sourceforge.net/projects/fuse/files/fuse-2.X/

See FUSE wiki for details:
http://sourceforge.net/apps/mediawiki/fuse/index.php?title=Hello_World

This program can be distributed under the terms of the GNU GPL.
See the file COPYING.

I should really learn to write a makefile:
gcc -Wall -Wextra `pkg-config fuse --cflags` encfs.c -o pa5-encfs `pkg-config fuse --libs` aes-crypt.c aes-crypt.h -l crypto
./pa5-encfs pass /home/nico/OS/five/mirror mount
*/

char *key_phrase;
char *mirror_dir;
char *mount_point;
char current_dir[256];

#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR
#include "aes-crypt.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/mman.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

void set_dir(const char * dir)
{
    strcpy(current_dir, "/dev/shm");
    strcat(current_dir, dir);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    set_dir(path);

    res = lstat(current_dir, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_access(const char *path, int mask)
{
    int res;
    set_dir(path);

    res = access(current_dir, mask);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
    int res;
    set_dir(path);

    res = readlink(current_dir, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;
    set_dir(path);

    (void) offset;
    (void) fi;

    dp = opendir(current_dir);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
    set_dir(path);

    /* On Linux this could just be 'mknod(current_dir, mode, rdev)' but this
       is more portable */
    if (S_ISREG(mode)) {
        res = open(current_dir, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(current_dir, mode);
    else
        res = mknod(current_dir, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    set_dir(path);

    res = mkdir(current_dir, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_unlink(const char *path)
{
    int res;
    set_dir(path);

    res = unlink(current_dir);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    set_dir(path);

    res = rmdir(current_dir);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_link(const char *from, const char *to)
{
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
    int res;
    set_dir(path);

    res = chmod(current_dir, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;
    set_dir(path);

    res = lchown(current_dir, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;
    set_dir(path);

    res = truncate(current_dir, size);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    int res;
    struct timeval tv[2];
    set_dir(path);

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(current_dir, tv);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
    set_dir(path);

	res = open(current_dir, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
	int fd;
	int res;
    set_dir(path);

	(void) fi;
	fd = open(current_dir, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
        off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
    set_dir(path);

	(void) fi;
	fd = open(current_dir, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
    int res;
    set_dir(path);

    res = statvfs(current_dir, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    (void) fi;
    set_dir(path);

    int res;
    res = creat(current_dir, mode);
    if(res == -1)
        return -errno;

    close(res);

    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */
    set_dir(path);

    (void) path;
    (void) fi;
    return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
        struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    set_dir(path);
    return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
        size_t size, int flags)
{
    set_dir(path);
    int res = lsetxattr(current_dir, name, value, size, flags);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
        size_t size)
{
    set_dir(path);
    int res = lgetxattr(current_dir, name, value, size);
    if (res == -1)
        return -errno;
    return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
    set_dir(path);
    int res = llistxattr(current_dir, list, size);
    if (res == -1)
        return -errno;
    return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
    set_dir(path);
    int res = lremovexattr(current_dir, name);
    if (res == -1)
        return -errno;
    return 0;
}
#endif /* HAVE_SETXATTR */

void *eread() {
    int action;
    int pid;
    FILE *out_fp;
    FILE *in_fp;

    in_fp = fopen(mirror_dir, "r");
    out_fp = fopen("/dev/shm/decrypted.tar", "w");
    action = 0;
    do_crypt(in_fp, out_fp, action, key_phrase);
    fclose(in_fp);
    fclose(out_fp);

    mkdir("/dev/shm/decrypted", 0700);
    char *args[] = { "/usr/bin/tar", "-xf", "/dev/shm/decrypted.tar", "-C", "/dev/shm/decrypted", NULL };
    if (!(pid = fork())) {
        execv(args[0], args);
        printf("WTF\n");
    } else {
        waitpid(-1, NULL, 0);
    }
    //unlink("/dev/shm/decrypted.tar");
    //unlink("/dev/shm/decrypted");
    return NULL;
}

void ewrite() {
    int action;
    int pid;
    FILE *out_fp;
    FILE *in_fp;
    char *args[] = {"/usr/bin/tar", "-cf", "/dev/shm/decrypted.tar", "/dev/shm/decrypted", NULL };

    if (!(pid = fork())) {
        execv(args[0], args);
        printf("WTF\n");
    } else {
        waitpid(-1, NULL, 0);
    }

    in_fp = fopen("/dev/shm/decrypted", "r");
    out_fp = fopen(mirror_dir, "w");
    action = 1;
    do_crypt(in_fp, out_fp, action, key_phrase);
    fclose(in_fp);
    fclose(out_fp);
    //unlink("/dev/shm/decrypted.tar");
    //unlink("/dev/shm/decrypted");
}

static struct fuse_operations xmp_oper = {
    .getattr	= xmp_getattr,
    .access		= xmp_access,
    .readlink	= xmp_readlink,
    .readdir	= xmp_readdir,
    .mknod		= xmp_mknod,
    .mkdir		= xmp_mkdir,
    .symlink	= xmp_symlink,
    .unlink		= xmp_unlink,
    .rmdir		= xmp_rmdir,
    .rename		= xmp_rename,
    .link		= xmp_link,
    .chmod		= xmp_chmod,
    .chown		= xmp_chown,
    .truncate	= xmp_truncate,
    .utimens	= xmp_utimens,
    .open		= xmp_open,
    .read		= xmp_read,
    .write		= xmp_write,
    .statfs		= xmp_statfs,
    .create     = xmp_create,
    .release	= xmp_release,
    .fsync		= xmp_fsync,
    .init       = eread,
    .destroy    = ewrite,
#ifdef HAVE_SETXATTR
    .setxattr	= xmp_setxattr,
    .getxattr	= xmp_getxattr,
    .listxattr	= xmp_listxattr,
    .removexattr	= xmp_removexattr,
#endif
};

int main(int argc, char *argv[])
{
    (void) argc;
    key_phrase  = argv[1];
    mirror_dir  = argv[2];
    mount_point = argv[3];
    char *args[3];
    args[0] = argv[0];
    args[1] = mount_point;
    args[2] = "-d";
    return fuse_main(3, args, &xmp_oper, NULL);
}
