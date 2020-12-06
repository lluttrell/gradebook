// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
// https://www.lemoda.net/c/unix-regex/

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>

#define DIRNAME "/home/registrar/studentgrades/"

extern int errno;

const char *get_username()
{
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  return pw->pw_name;
}

/**
 * Checks if the arguments for write_grade() are valid
 */
int check_valid_grade(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: gradebook -w username coursecode lettergrade\n");
    return -1;
  }

  regex_t recourse, regrade;
  regcomp(&recourse, "^[A-Z]{4}[0-9]{3}$", REG_EXTENDED);
  if (regexec(&recourse, argv[1], 0, NULL, 0) == REG_NOMATCH)
  {
    printf("Invalid coursecode: Must match the regex ^[A-Z]{4}[0-9]{3}$ eg: CPSC525\n");
    return -1;
  }

  regcomp(&regrade, "^([A-D][\\+\\-]?|F)$", REG_EXTENDED);
  if (regexec(&regrade, argv[2], 0, NULL, 0) == REG_NOMATCH)
  {
    printf("Invalid coursecode: Must match the regex ^([A-D][\\+\\-]?|F)$ eg: B+\n");
    return -1;
  }

  return 0;
}

int write_grade(int argc, char *argv[])
{
  if (check_valid_grade(argc, argv) != 0)
  {
    return -1;
  }
  const char *uname = get_username();

  char fname[64];
  strncpy(fname, DIRNAME, 31);
  strncat(fname, argv[0], 32);

  int ac = access(fname, W_OK);
  if (ac == 0)
  {
    FILE *fp = fopen(fname, "a");
    fprintf(fp, "%s %s %s\n", uname, argv[1], argv[2]);
    fclose(fp);
    printf("Submitted Grade!\n");
  }
  else if (errno == 2)
  {
    printf("Gradebook has not been initialized for %s\n", argv[0]);
    return -1;
  }
  else
  {
    printf("Permission Denied\n");
    return -1;
  }
  return 0;
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
    fprintf(fp, "Instructor Course Grade\n");
    fclose(fp);
    chmod(fname, S_IRWXU | S_IWGRP);
    printf("Initialized new gradebook for %s\n", uname);
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
    printf("Usage: %1$s [-o filename] -r\nusage: %1$s -w username coursecode grade \nusage: %1$s -i\n", argv[0]);
    return (1);
  }

  argc -= optind;
  argv += optind;

  if (initflag)
    init_gradebook();
  if (writeflag)
    write_grade(argc, argv);

  return (0);
}