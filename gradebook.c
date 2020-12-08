/*
 * FILENAME: gradebook.c
 * 
 * DESCRIPTION:
 * 
 *      University grade storage and retrieval system
 *      Final Project for CPSC 525 (Computer Security)
 * 
 * AUTHOR: Richard Williams 10041850
 * 
 * REFERENCES:
 * 
 *      https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
 *      https://www.lemoda.net/c/unix-regex/
 *      https://www.programmingsimplified.com/c-program-copy-file
 *
 */

#include <stdlib.h>
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
#include <grp.h>
#include "gradebook.h"

extern int errno;

/*
* Returns the username for whoever is running the program.
*/
char *get_username()
{
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  return pw->pw_name;
}

/**
 * Checks if the arguments for write_grade() are valid
 * @param arcg Number of arguments passed to function
 * @param argv Array containing the arguments
 */
int check_valid_grade(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: gradebook -w -u username coursecode lettergrade\n");
    return -1;
  }

  regex_t recourse, regrade;
  regcomp(&recourse, "^[A-Z]{4}[0-9]{3}$", REG_EXTENDED);
  if (regexec(&recourse, argv[0], 0, NULL, 0) == REG_NOMATCH)
  {
    printf("Invalid coursecode: Must match the regex ^[A-Z]{4}[0-9]{3}$ eg: CPSC525\n");
    return -1;
  }

  regcomp(&regrade, "^([A-D][\\+\\-]?|F)$", REG_EXTENDED);
  if (regexec(&regrade, argv[1], 0, NULL, 0) == REG_NOMATCH)
  {
    printf("Invalid coursecode: Must match the regex ^([A-D][\\+\\-]?|F)$ eg: B+\n");
    return -1;
  }
  return 0;
}

/**
 *  Writes grade to gradebook
 * @param arcg Number of arguments passed to function
 * @param argv Array containing the arguments
 * @param uname Username to write grade for
 */
int write_grade(int argc, char *argv[], char *uname)
{
  if (check_valid_grade(argc, argv) == -1)
  {
    return -1;
  }
  const char *puname = get_username();
  printf("%s\n", puname);

  char fname[96];
  strncpy(fname, DIRNAME, 64);
  strncat(fname, uname, 32);
  printf("%s\n", fname);

  int ac = access(fname, W_OK);
  if (ac == 0)
  {
    FILE *fp = fopen(fname, "a");
    fprintf(fp, "%s %s %s\n", puname, argv[0], argv[1]);
    fclose(fp);
    printf("Submitted Grade!\n");
  }
  else
  {
    printf("Error: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

/**
 * Initializes a new gradebook for the user
 */
int init_gradebook(char *username, int uflag)
{
  char fname[96];
  char uname[32];
  char *errstring;
  strncpy(fname, DIRNAME, 64);
  if (uflag)
    strncpy(uname, username, 31);
  else
    strncpy(uname, get_username(), 31);
  strcat(fname, uname);

  if (access(fname, W_OK) == -1)
  {
    errstring = strerror(errno);
    switch (errno)
    {
    case ENOENT:
      break; // file doesn't need to exist
    case EACCES:
      // if user doesn't have read access, they must be accessing their own directory
      if (strcmp(uname, get_username()) == 0)
        break;
    default:
      printf("Error: %s\n", errstring);
      return -1;
    }
  }

  if (faccessat(0, fname, F_OK, AT_EACCESS) == -1)
  {
    errstring = strerror(errno);
    switch (errno)
    {
    case ENOENT:
      if (write_init_file(fname, uname) == 0)
        printf("Initialized new gradebook for %s\n", uname);
      else
        printf("Error: %s\n", strerror(errno));
      break;
    default:
      printf("Error: %s\n", errstring);
      return -1;
    }
  }
  else
  {
    printf("Gradebook for %s has already been initialized!\n", uname);
  }
  return 0;
}

/**
 * Writes new gradebook file
 * @param fname pathname to gradebook director
 * @param username username to create gradebook for
 */
int write_init_file(char *fname, char *uname)
{
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
    return -1;

  fprintf(fp, "Gradebook for %s\n", uname);
  fprintf(fp, "Instructor Course Grade\n");
  fclose(fp);

  if (chmod(fname, S_IRWXU | S_IWGRP) == -1)
    return -1;
  return 0;
}

/**
 * Reads grades from file
 * @param uname username of files you wnat
 * @param ofarg argument for file path
 * @param fileoutputflag 1 for output to ofarg, 0 for output to stdout
 */
int read_grades(char *uname, int argc, char *argv[], int fileoutputflag)
{
  char ifpath[96];
  char ofname[64];
  char ch;

  // store input filename in ifpath
  strncpy(ifpath, DIRNAME, 64);
  strncat(ifpath, uname, 32);

  printf("%s\n", ifpath);

  // check caller has access rights, exit if not
  if (access(ifpath, R_OK) == -1)
  {
    char *errstring = strerror(errno);
    switch (errno)
    {
    case EACCES:
      // if user doesn't have read access, they must be accessing their own directory
      if (strcmp(uname, get_username()) == 0)
        break;
    default:
      printf("Error: %s\n", errstring);
      return -1;
    }
  }

  FILE *ofp, *ifp;
  if (fileoutputflag)
  {
    if (argc == 1)
      strcpy(ofname, argv[0]);
    else
      sprintf(ofname, "./%s_gradebook_log", uname);
    ofp = fopen(ofname, "w");
    if (ofp == NULL)
    {
      printf("Error: %s\n", strerror(errno));
      return -1;
    }
  }
  else
  {
    ofp = fdopen(0, "w");
  }

  ifp = fopen(ifpath, "r");
  if (ifp == NULL)
  {
    printf("Error: %s\n", strerror(errno));
    return -1;
  }
  while ((ch = fgetc(ifp)) != EOF)
    fputc(ch, ofp);

  fclose(ofp);
  fclose(ifp);

  return 0;
}

void printhelp()
{
  system("cat /home/registrar/help.txt");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  int flags = 0;
  int initflag = 0, writeflag = 0, readflag = 0, fileoutputflag = 0, usrflag = 0, errflag = 0, helpflag = 0;
  char *uname, *ofilename;
  int opt;
  while ((opt = getopt(argc, argv, "hrwiou:")) != -1)
  {
    switch (opt)
    {
    case 'o':
      fileoutputflag = 1;
      //ofilename = strdup(optarg);
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
    case 'h':
      helpflag = 1;
      break;
    default:
      errflag = 31;
      break;
    }
  }

  if (!usrflag)
    uname = get_username();

  if (helpflag)
    printhelp();

  argc -= optind;
  argv += optind;

  int success;
  flags = (readflag << 4 | writeflag << 3 | initflag << 2 | usrflag << 1 | fileoutputflag);
  switch (flags | errflag)
  {
  case 0b00000:
  case 0b00001:
  case 0b00010:
  case 0b00011:
  case 0b10000:
  case 0b10001:
  case 0b10010:
  case 0b10011:
    success = read_grades(uname, argc, argv, fileoutputflag);
    break;
  case 0b01010:
    success = write_grade(argc, argv, uname);
    break;
  case 0b00100:
  case 0b00110:
    success = init_gradebook(uname, usrflag);
    break;
  default:
    errflag = 1;
  }

  if (errflag)
    printf("Invalid Arguments \nTry 'gradebook -h for more information.\n");

  return (success);
}