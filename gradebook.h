/*
 * FILENAME: gradebook.h
 * 
 * DESCRIPTION:
 * 
 *      Header file for gradebook.c
 *      Final Project for CPSC 525 (Computer Security)
 * 
 * AUTHOR: Richard Williams 10041850
 * 
 */

#define DIRNAME "/freeforall/richard.williams2/studentgrades/"
#define HELPCALL "cat /freeforall/richard.williams2/help.txt"

int read_grades(char *uname, int arcg, char *argv[], int fileoutputflag);
int check_valid_grade(int argc, char *argv[]);
int init_gradebook(char *username, int uflag);
int write_init_file(char *fname, char *uname);
int write_grade(int argc, char *argv[], char *uname);
void printhelp();
