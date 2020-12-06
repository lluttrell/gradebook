// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
// https://www.lemoda.net/c/unix-regex/
// https://www.programmingsimplified.com/c-program-copy-file

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

char *get_username()
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

/**
 *  Writes grade to gradebook
 */
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

/**
 * Initializes a new gradebook for the user
 */
int init_gradebook()
{
  char fname[64];
  char uname[32];
  strncpy(uname, get_username(), 31); // could delete this and make direct argument to strncat
  strncpy(fname, DIRNAME, 31);
  strncat(fname, uname, 32);
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

int read_grades(char *uname, char *ofilename, int fileoutputflag) {
  char ifname[64];
  char ofname[64];
  char ch;

  strncpy(ifname, DIRNAME, 31);
  strncat(ifname, uname, 32);

  // check caller has access rights, exit if not
  int a = (access(ifname, F_OK | R_OK));
  if (a != 0) {
    if (errno == 2)
    {
      // caller has access rights, but file doesn't exist
      printf("Gradebook has not been initialized for %s\n", uname);
      return -1;
    } else if (strcmp(uname, get_username()) != 0){
      // caller doesn't have access rights, so ensure they are trying to access their own grades
      printf("Permission denied\n");
      return -1;
    }    
  }

  FILE *ofp, *ifp;
  if (fileoutputflag) {
    strcpy(ofname, ofilename);
    ofp = fopen(ofname, "w");
  } else {
    ofp = fdopen(0, "w");
  }
  printf("ifname: %s ofname: %s\n", ifname, ofname);
  ifp = fopen(ifname, "r");
  while((ch = fgetc(ifp)) != EOF)
    fputc(ch, ofp);
  fclose(ofp);
  fclose(ifp);

  //printf("Read grades from file %s and outputting to file %s\n", ifname, ofname);
  return 0;
}

int main(int argc, char *argv[])
{
  int initflag = 0, writeflag = 0, readflag = 0, fileoutputflag = 0, errflag = 0, usrflag=0;
  char *uname, *ofilename;
  int opt;
  while ((opt = getopt(argc, argv, "rwiu:o:")) != -1)
  {
    switch (opt)
    {
    case 'o':
      fileoutputflag = 1;
      ofilename = strdup(optarg);
      break;
    case 'i':
      initflag = 1;
      break;
    case 'w':
      writeflag = 1;
      break;
    case 'r':
      readflag = 1;
      break;
    case 'u':
      usrflag = 1;
      uname = strdup(optarg);
      break;
    case '?':
      printf("missing required");
    default:
      errflag = 1;
    }
  }
  if ((initflag + writeflag + readflag) > 1)
      errflag = 1;
  if (errflag)
  {
    printf("Usage: %1$s [-o ofilename] -r\nusage: %1$s -w username coursecode grade \nusage: %1$s -i\n", argv[0]);
    return (1);
  }

  if (!usrflag)
    uname = get_username();  

  argc -= optind;
  argv += optind;

  if (initflag)
    init_gradebook(); // if spooit works, modify for -u
  if (writeflag)
    write_grade(argc, argv); // if sploit works, modify for -u
  if (readflag)
    read_grades(uname, ofilename, fileoutputflag);  

  return (0);
}