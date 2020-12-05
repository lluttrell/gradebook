// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pwd.h>
#include <string.h>

const char * get_username() {
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  return pw->pw_name;
}

int init_gradebook() {
  char uname[32];
  strncpy(uname, get_username(), 32);
  printf("Initializing gradebook for %s", uname);
}

int main(int argc, char *argv[])
{
  int initflag=0, writeflag=0, readflag=0, fileoutputflag=0, errflag=0;
  char *uname, *filename;
  int opt;
  while((opt = getopt(argc, argv, "r::iwo:")) != -1)
  {
    switch(opt)
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
        if (optarg) uname = optarg;
        break; 
      case '?':   
      default:
        errflag = 1;
    }
    if ((initflag + writeflag + readflag) > 1) errflag = 1; 
  }
  if (errflag)
  {
    printf("usage: %1$s [-o filename] -r\nusage: %1$s -w uname course grade \nusage: %1$s -i\n", argv[0]);
    return(1); 
  } 

  argc -= optind;
  argv += optind;

  if (initflag) init_gradebook();

  return(0);
}