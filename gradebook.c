// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DIRNAME "/home/registrar/studentgrades/"

const char *get_username()
{
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  return pw->pw_name;
}

gid_t get_group()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwnam("drwho");
  //struct passwd *pw = getpwuid(uid);
  printf("gid_t: %u", pw->pw_gid);
  return pw-> pw_gid;
}

int init_gradebook()
{
  char fname[64];
  char uname[32];
  strncpy(uname, get_username(), 31);
  strncpy(fname, DIRNAME, 31);
  strcat(fname, uname);
  if (faccessat(0, fname, F_OK, AT_EACCESS) != -1)
  {
    printf("Gradebook for %s already initialized!\n", uname);
  }
  else
  {
    FILE *fp = fopen(fname, "w");
    fprintf(fp, "Gradebook for %s\n", uname);
    fclose(fp);
    printf("Initialized new gradebook for %s\n", uname);
    chmod(fname, S_IRWXU | S_IWGRP );
    get_group();
    // chown(fname, )
  }
}

int main(int argc, char *argv[])
{
  int initflag = 0, writeflag = 0, readflag = 0, fileoutputflag = 0, errflag = 0;
  char *uname, *filename;
  int opt;
  while ((opt = getopt(argc, argv, "r::iwo:")) != -1)
  {
    switch (opt)
    {
    case 'o':
      fileoutputflag = 1;
      filename = optarg;
      break;
    case 'i':
      initflag = 1;
      break;
    case 'w':
      writeflag = 1;
      uname = optarg;
      break;
    case 'r':
      readflag = 1;
      if (optarg)
        uname = optarg;
      break;
    case '?':
    default:
      errflag = 1;
    }
    if ((initflag + writeflag + readflag) > 1)
      errflag = 1;
  }
  if (errflag)
  {
    printf("usage: %1$s [-o filename] -r\nusage: %1$s -w uname course grade \nusage: %1$s -i\n", argv[0]);
    return (1);
  }

  argc -= optind;
  argv += optind;

  if (initflag)
    init_gradebook();

  return (0);
}