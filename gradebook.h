#define DIRNAME "/home/registrar/studentgrades/"
#define FACGRNAME "faculty"

extern int errno;

int read_grades(char *uname, char *ofarg, int fileoutputflag);
int check_valid_grade(int argc, char *argv[]);
int init_gradebook(char *username, int uflag);
int write_init_file(char *fname, char *uname);
int write_grade(int argc, char *argv[], char *uname);
void printhelp();