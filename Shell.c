
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

char *history_file,*ip_redirection,*op_redirection;
char *cmd_exec[100];
static char* args[512];
char cwd[1000],input_buffer[1000],history_data[1000][1000],current_directory[1000],histat_var[1000];
pid_t pid;
int fd,flag,len,lines=0,pipe1=0,output_redirection,input_redirection,flag1;
static int command(int input, int first, int last, char *cmd_exec);

void prompt()
{
  char my_shell[1000];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
          strcpy(my_shell, "My_shell");
          strcat(my_shell, cwd);
          strcat(my_shell, "$ ");

          printf("%s", my_shell);
        }
   else
       perror("getcwd() error");

}
void clearv()
{
  fd =0,flag=0,len=0,lines=0,pipe1=0,output_redirection=0,input_redirection=0,input_buffer[0]='\0',  cwd[0] = '\0', pid=0,flag1=0;
}
void process_read()
{
 int fd;
  history_file=(char *)malloc(100*sizeof(char));
  strcpy(history_file,current_directory);
  strcat(history_file, "/");
  strcat(history_file, "example.txt"); 
  fd=open(history_file, O_RDONLY|O_CREAT,S_IRUSR|S_IWUSR);
  int read_bytes=0, i=0, a=0, index=0;
  char buffer[1], temp[2000];
 do 
    {
        read_bytes = read (fd, buffer, sizeof (buffer));
        for (i=0; i<read_bytes; ++i) 
                  {
                    temp[index]=buffer[i];
                    index++;
                    if(buffer[i]=='\n')
                        {
                          temp[index-1]='\0';
                          lines++; 
                          index=0;
                          strcpy(history_data[a], temp);
                          a++;
                          temp[0]='\0';
                        }
                  }
    }
    while (read_bytes == sizeof (buffer)); 
    close (fd); 
}
void process_write()
{
  int fd1,write_data,stringlength=0;
  char input_data[2000];
  lines++;
  char number[10];
  sprintf (number, "%d", lines++ ); 
  strcpy(input_data, " ");
  strcat(input_data, number);
  strcat(input_data, " ");
  strcat(input_data, input_buffer);

  stringlength = strlen(input_data);
  fd1=open(history_file,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
  len=strlen(input_buffer);
  write_data=write(fd1,input_data,stringlength);
  if(write_data<0) 
        {
          printf("Nothing to write\n");
          return;
        }
  close(fd1);

}
void execute()
{
  char exec_val[1000];
  char *tokenise_exec[100], *exec_num[10];
  int i, m=1, num, index=0;
  i=1;
  if(input_buffer[i]=='!')
        {
           strcpy(exec_val, history_data[lines-1]);
        }
  else if(input_buffer[i]=='-')
    {
        m=1;
        exec_num[0]=strtok(input_buffer,"-");
        while ((exec_num[m]=strtok(NULL,"-"))!=NULL)
              m++;
        exec_num[m]=NULL;
        num = atoi(exec_num[1]);

        index =lines-num;
        strcpy(exec_val, history_data[index]);
              
    }
  else 
    {
      exec_num[0]=strtok(input_buffer,"!");
      num = atoi(exec_num[0]);
      strcpy(exec_val, history_data[num-1]);
    }
  tokenise_exec[0]=strtok(exec_val," ");
  while ((tokenise_exec[m]=strtok(NULL,""))!=NULL)
              m++;
  tokenise_exec[m]=NULL;
  strcpy(exec_val, tokenise_exec[1]);
  printf("%s\n", exec_val );
  strcpy(input_buffer, exec_val);

    
}
void chdirectory()
{
char *h="/home";   
if(args[1]==NULL)
        chdir(h);
else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
        chdir(h);
else if(chdir(args[1])<0)
    printf("bash: cd: %s: No such file or directory\n", args[1]);

}
void prdirectory()
{
if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
         printf("%s\n", cwd );
        }
else
       perror("getcwd() error");


}

void history_exec()
{
  int num, i, index;
  if(flag1==1)
        {
         for(i=0; i<lines; i++)
            printf("%s\n", history_data[i] ); 
        }
  else if(args[1]==NULL)
      {
        for(i=0; i<lines-1; i++)
            printf("%s\n", history_data[i] );
        printf(" %d %s\n",lines,histat_var );
      }
  else
      {
        if(args[1]!=NULL)
        num = atoi(args[1]);
        if(num>lines) 
        {
          for(i=0; i<lines-1; i++)
            printf("%s\n", history_data[i] );
          printf(" %d %s\n",lines,histat_var );
        }
        index=lines-num;
        for(i=index; i<lines-1; i++)
            printf("%s\n", history_data[i] );
          printf(" %d %s\n", lines, histat_var );
      } 

}

void histat_exec()
{
  int num, i,index;
  num = 10;
  if (lines >= num)
    index = lines - num;
  else
    index = 0;
  for(i=index; i<lines-1; i++)
            printf("%s\n", history_data[i] );
          printf(" %d %s\n", lines, histat_var );
}

void tokenise_inputs(char *input)
{
int m=1;
args[0]=strtok(input," ");       
while((args[m]=strtok(NULL," "))!=NULL)
        m++;
}

static char* space(char* s)
{
  while (isspace(*s)) ++s;
  return s;
}

void input_output_redirection(char *input)
{
  char *ipop[100];
  char *input1;  
  input1=strdup(input);
  int a=1;
  ipop[0]=strtok(input1,"<");       
  while((ipop[a]=strtok(NULL,">"))!=NULL)
        a++;
  ipop[1]=space(ipop[1]);
  ipop[2]=space(ipop[2]);
  ip_redirection=strdup(ipop[1]);
  op_redirection=strdup(ipop[2]);
  tokenise_inputs(ipop[0]);
  
}
void tokenise_redirect_input(char *input)
{
  char *ipop[100];
  char *input1;  
  input1=strdup(input);
  int a=1;
  ipop[0]=strtok(input1,"<");       
  while((ipop[a]=strtok(NULL,"<"))!=NULL)
        a++;
  ipop[1]=space(ipop[1]);
  ip_redirection=strdup(ipop[1]);
  tokenise_inputs(ipop[0]);
}
void tokenise_redirect_output(char *output)
{
  char *opop[100];
  char *output1;  
  output1=strdup(output);
  int a=1;
  opop[0]=strtok(output1,">");       
  while((opop[a]=strtok(NULL,">"))!=NULL)
          a++;
  opop[1]=space(opop[1]);
  op_redirection=strdup(opop[1]); 
  tokenise_inputs(opop[0]);   
  
}
char* skipcomma(char* str)
{
  int i=0, j=0;
  char temp[1000];
  while(str[i++]!='\0')
            {
              if(str[i-1]!='"')
                    temp[j++]=str[i-1];
            }
        temp[j]='\0';
        str = strdup(temp);
  
  return str;
}
static int split(char *cmd_exec, int input, int first, int last)
{
    char *new_cmd_exec1;  
    new_cmd_exec1=strdup(cmd_exec);
   //else
      {
        int m=1;
        args[0]=strtok(cmd_exec," ");       
        while((args[m]=strtok(NULL," "))!=NULL)
              m++;
        args[m]=NULL;
        if (args[0] != NULL) 
            {

            if (strcmp(args[0], "exit") == 0) 
                    exit(0);
            if (strcmp(args[0], "echo") != 0) 
                    {
                      cmd_exec = skipcomma(new_cmd_exec1);
                      int m=1;
                      args[0]=strtok(cmd_exec," ");       
                      while((args[m]=strtok(NULL," "))!=NULL)
                                m++;
                      args[m]=NULL;

                    }
            if(strcmp("cd",args[0])==0)
                    {
                    chdirectory();
                    return 1;
                    }
            else if(strcmp("pwd",args[0])==0)
                    {
                    prdirectory();
                    return 1;
                    }
           
            }
        }
    return command(input, first, last, new_cmd_exec1);
}
void pipe_execute()
{

int i, n=1, input, first;

input=0;
first= 1;

cmd_exec[0]=strtok(input_buffer,"|");

while ((cmd_exec[n]=strtok(NULL,"|"))!=NULL)
      n++;
cmd_exec[n]=NULL;
pipe1=n-1;
for(i=0; i<n-1; i++)
    {
      input = split(cmd_exec[i], input, first, 0);
      first=0;
    }
input=split(cmd_exec[i], input, first, 1);
input=0;
return;

}
static int command(int input, int first, int last, char *cmd_exec)
{
  int mypipefd[2], ret, input_fd, output_fd;
  ret = pipe(mypipefd);
  if(ret == -1)
      {
        perror("pipe");
        return 1;
      }
  pid = fork();
 
  if (pid == 0) 
  {
    if (first==1 && last==0 && input==0) 
    {
      dup2( mypipefd[1], 1 );
    } 
    else if (first==0 && last==0 && input!=0) 
    {
      dup2(input, 0);
      dup2(mypipefd[1], 1);
    } 
    else 
    {
      dup2(input, 0);
    }
    if (strchr(cmd_exec, '<') && strchr(cmd_exec, '>')) 
            {
              input_redirection=1;
              output_redirection=1;
              input_output_redirection(cmd_exec);
            }
   else if (strchr(cmd_exec, '<')) 
        {
          input_redirection=1;
          tokenise_redirect_input(cmd_exec);
        }
   else if (strchr(cmd_exec, '>')) 
        {
          output_redirection=1;
          tokenise_redirect_output(cmd_exec);
        }
    if(output_redirection == 1)
                {                    
                        output_fd= creat(op_redirection, 0644);
                        if (output_fd < 0)
                          {
                          fprintf(stderr, "Failed to open %s for writing\n", op_redirection);
                          return(EXIT_FAILURE);
                          }
                        dup2(output_fd, 1);
                        close(output_fd);
                        output_redirection=0;
                }
    if(input_redirection  == 1)
                  {
                         input_fd=open(ip_redirection,O_RDONLY, 0);
                         if (input_fd < 0)
                          {
                          fprintf(stderr, "Failed to open %s for reading\n", ip_redirection);
                          return(EXIT_FAILURE);
                          }
                        dup2(input_fd, 0);
                        close(input_fd);
                        input_redirection=0;
                  }
    else if (strcmp(args[0], "history") == 0)
             {
              history_exec();
              }
    else if (strcmp(args[0], "histat") == 0)
             {
              histat_exec();
              } 
 
    else if(execvp(args[0], args)<0) printf("%s: command not found\n", args[0]);
              exit(0);
  }
  else 
  {
     waitpid(pid, 0, 0);  
   }
 
  if (last == 1)
    close(mypipefd[0]);
  if (input != 0) 
    close(input);
  close(mypipefd[1]);
  return mypipefd[0];

}
int main()
{   
    int status;
    char ch[2]={"\n"};
    getcwd(current_directory, sizeof(current_directory));
    while (1)
    {
      clearv();
      prompt();
      fgets(input_buffer, 1024, stdin);
      if(strcmp(input_buffer, ch)==0)
            {
              continue;
            }
      if(input_buffer[0]!='!')
            {
                process_read();
                process_write(); 
            }         
      len = strlen(input_buffer);
      input_buffer[len-1]='\0';
      strcpy(histat_var, input_buffer);
      if(strcmp(input_buffer, "exit") == 0) 
            {
              flag = 1;
              break;
            }
      if(input_buffer[0]=='!')  
              {
                process_read();
                flag1=1;
                execute();
              }
      pipe_execute();
      waitpid(pid,&status,0);
         
    }  
    if(flag==1)
      {
      printf("Exited\n");
      exit(0);       
      return 0;
      }
return 0;
}
