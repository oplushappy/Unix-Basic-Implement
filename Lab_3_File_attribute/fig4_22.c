#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

/* function type that is called for each filename */
typedef int Myfunc(const char *, const struct stat *, int); // 

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


static off_t total_size;
void check_valid(const char *pathname);
static int valid = 0, invalid = 0;

int main(int argc, char *argv[])
{
		char path1[256] , path2[256];
		sprintf(path1, "%s/Test1", argv[1]);
		sprintf(path2, "%s/Test2", argv[1]);
		if (chmod(path1, S_IRWXU | S_IRWXG | S_IRWXO) <  0)
			err_quit("can't chmod");
		if (chmod(path2, S_IRWXU | S_IRWXG | S_IRWXO) <  0)
			err_quit("can't chmod");

		int ret;
		if (argc != 2)
			err_quit("usage: ftw <starting-pathname>");
		ret = myftw(argv[1], myfunc); /* does it all */
		ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
		if (ntot == 0) 
				ntot = 1; /* avoid divide by 0; print 0 for all counts */
		printf("regular files\t= %7ld, %5.2f %%\n", nreg,
						nreg*100.0/ntot);
		printf("directories\t= %7ld, %5.2f %%\n", ndir,
						ndir*100.0/ntot);
		printf("block special\t= %7ld, %5.2f %%\n", nblk,
						nblk*100.0/ntot);
		printf("char special\t= %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
		printf("FIFOs\t\t= %7ld, %5.2f %%\n", nfifo,
						nfifo*100.0/ntot);
		printf("symbolic links\t= %7ld, %5.2f %%\n", nslink,
						nslink*100.0/ntot);
		printf("sockets\t\t= %7ld, %5.2f %%\n", nsock,
						nsock*100.0/ntot);
    printf("valid = %d\n", valid);
    printf("invalid = %d\n", invalid);
    printf("Total size = %ld\n", (long)total_size);
		exit(ret);
}
/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4 /* file that we can't stat */

static char *fullpath; /* contains full pathname for every file */
                       // fullpath is address

static int myftw(char *pathname, Myfunc *func)/* we return whatever func() returns */
{
		int len;
		fullpath = path_alloc(&len); /* malloc's for PATH_MAX+1 bytes */
		/* (Figure 2.15) */
		strncpy(fullpath, pathname, len); /* protect against */
		fullpath[len-1] = 0; /* buffer overrun */
		return(dopath(func));
}
/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int dopath(Myfunc* func)/* we return whatever func() returns */
{
		struct stat statbuf; // stat.h provides
		struct dirent *dirp;
		DIR *dp;
		int ret;
		char *ptr;
	  //------------------------------------------------------------
		if (lstat(fullpath, &statbuf) < 0) /* stat error */ // return 0 is successful
				return(func(fullpath, &statbuf, FTW_NS));
		if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */ return(func(fullpath, &statbuf, FTW_F));
		/*
		 * It's a directory. First call func() for the directory,
		 * then process each filename in the directory.
		 */
		if ((ret = func(fullpath, &statbuf, FTW_D)) != 0) 
				return(ret);
    //----------------------------------------------------------------
		ptr = fullpath + strlen(fullpath); /* point to end of fullpath */
		*ptr++ = '/';
		*ptr = 0;
		if ((dp = opendir(fullpath)) == NULL) {/* can't read directory */
			return(func(fullpath, &statbuf, FTW_DNR));
    }
    //---------------------------------------------------------------
		while ((dirp = readdir(dp)) != NULL) {
				if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
						continue; /* ignore dot and dot-dot */
				strcpy(ptr, dirp->d_name); /* append name after slash */
				if ((ret = dopath(func)) != 0) /* recursive */
						break; /* time to leave */
		}
		ptr[-1] = 0; /* erase everything from slash onwards */
		if (closedir(dp) < 0)
				err_ret("can't close directory %s", fullpath);
		return(ret);
}
static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
		switch (type) {
				case FTW_F:
						total_size += statptr->st_size; // actually st_size only meaningful for FLE、LINK、DIC、(PIPE)
						switch (statptr->st_mode & S_IFMT) {
								case S_IFREG: 
										nreg++;
										// total_size += statptr->st_size;
										break;
								case S_IFBLK: nblk++; break;
								case S_IFCHR: nchr++; break;
								case S_IFIFO: nfifo++; break;
								case S_IFLNK: {
                    					nslink++;
                    					// total_size += statptr->st_size;
                   						 check_valid(pathname);
                    					break;
								}
								case S_IFSOCK: nsock++; break;
								case S_IFDIR:
											   err_dump("for S_IFDIR for %s", pathname);
											   /* directories should have type = FTW_D */
						}
						break;
				case FTW_D:
						ndir++;
            			total_size += statptr->st_size;
						break;
				case FTW_DNR:
						err_ret("can't read directory %s", pathname);
						break;
				case FTW_NS:
						err_ret("stat error for %s", pathname);
						break;
				default:
						err_dump("unknown type %d for pathname %s", type, pathname);
		}
		return(0);
}

void check_valid(const char *pathname) {
  char buf[1024];
  int res = readlink(pathname, buf, sizeof(buf));
  if(res == -1) invalid++;
  else {
    buf[res] = '\0';
    if(access(buf, F_OK) == 0) valid++;
    else invalid++;
  }
}
