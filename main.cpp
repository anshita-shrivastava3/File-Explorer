#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <cstring>
#include <vector>
#include <stack>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
struct termios orig_termios;
char switch_nc='n';
using namespace std;
class FileDetails{

   public:
   string f_name;
   string f_perm;
   long int f_size;
   string f_path;
   string f_type;

   FileDetails(string name, long int size, string perm, string path, string type){
      f_name=name;
      f_perm=perm;
      f_size=size;
      f_path=path;
      f_type=type;
   }
};

void clear_terminal(){
   cout << "\033c";
}

void disable_non_can_mode(){
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_non_can_mode(){
   //cout<<"inside can mode";
   tcgetattr(STDIN_FILENO, &orig_termios);
   atexit(disable_non_can_mode);
   struct termios raw=orig_termios;
   raw.c_lflag &= ~(ECHO | ICANON);
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void open_file(vector<FileDetails> &vec, int &curr){
   FILE *currfile;
   currfile=fopen((vec[curr].f_path).c_str(), "a");

   if(currfile!=NULL){
      // cout<<"inside if \n";
      pid_t pid=fork();
      if(pid<0){
         perror("fork failed");
         exit(0);
      }
      if(pid==0){
         // char *argv={}
         // cout<<"inside else of fork\n";
         char *exec_args[3] = {(char*)("/usr/bin/gedit"), (char*)((vec[curr].f_path).c_str()), NULL};
         execv(exec_args[0], exec_args);
      }
   }

   fclose(currfile);
}

void set_status(string print_path, int no_of_items){
   struct winsize ws;
   int x=print_path.size();
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
   // cout<<"columns "<<ws.ws_col<<"\n";
   // cout<<"rows "<<ws.ws_row<<"\n";
   // cout<<"STDOUT_FILENO: "<<STDOUT_FILENO<<"\n";
   //write(STDOUT_FILENO, "~\r\n", 10);
   int n=(ws.ws_row-no_of_items-1);
   for (int y = 0; y < n; y++) {
    write(STDOUT_FILENO, "\n", 1);
    if (y == n-1) {
      write(STDOUT_FILENO, "Normal Mode::", 14);
      write(STDOUT_FILENO, print_path.c_str(), x);
    }
  }
}

char print_files(vector<FileDetails> &vec, stack<string> &forward_stack, stack<string> &backward_stack){
   //cout<<"here";
   int curr=0;
   int st=0;         //defined for vertical overflow
   int ed=19;        //defined for vertical overflow
   char c='a';
   do{
   clear_terminal();
      cout<<"val of c is "<<c<<endl;
      if(vec.size()<=20){
         if(c == 'A' && curr!=0){
            curr--;
         }else if(c == 'B' && curr!=vec.size()-1){
            curr++;
         }else if(c == '\n' && vec[curr].f_type!="d"){
            //cout<<"enter detected\n";
            //cout<<"path of the file is: "<<vec[curr].f_path<<"\n";
            open_file(vec, curr);
         }
         //cout<<"val of curr "<<curr<<"file type: "<<vec[curr].f_type<<endl;
         for(int i=0; i<vec.size(); ++i){
            if(curr==i){
               cout<<"->\t";
               cout<<vec[i].f_perm<<"\t"<<vec[i].f_size<<"\t"<<vec[i].f_name<<"\n";
            }else{
               cout<<"\t"<<vec[i].f_perm<<"\t"<<vec[i].f_size<<"\t"<<vec[i].f_name<<"\n";
            }
         }
         //cout<<"end of display loop\n"; 
         set_status(backward_stack.top(), vec.size());
      }else{
         if(c == 'A' && st!=0 && curr!=0){
            if(curr==st){
               st--;
               ed--;
               curr=st;
            }else{
               curr--;
            }
         }else if(c == 'B' && ed!=vec.size()-1){
            if(curr==ed){
               st++;
               ed++;
               curr=ed;
            }else{
               curr++;
            }
         }else if(c == '\n' && vec[curr].f_type!="d"){
            //cout<<"enter detected\n";
            //cout<<"path of the file is: "<<vec[curr].f_path<<"\n";
            open_file(vec, curr);
         }
         //cout<<"val of curr "<<curr<<"file type: "<<vec[curr].f_type<<endl;
         for(int i=st; i<=ed; ++i){
            if(curr==i){
               cout<<"->\t";
               cout<<vec[i].f_perm<<"\t"<<vec[i].f_size<<"\t"<<vec[i].f_name<<"\t"<<vec[i].f_path<<"\n";
            }else{
               cout<<"\t"<<vec[i].f_perm<<"\t"<<vec[i].f_size<<"\t"<<vec[i].f_name<<"\t"<<vec[i].f_path<<"\n";
            }
         }
         //cout<<"end of display loop\n"; 
         set_status(backward_stack.top(), 20);
      }
         
   }while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q' && (c!='\n' || vec[curr].f_type!="d") && c!='D' && c!='C' && c!=127 && c!=';');
   //   cout<<"exit inner loop, val of c is: "<<c<<"\n";
 //  }while (/*read(STDIN_FILENO, &c, 1) == 1 && */c != 'q');
   if(c == '\n' && vec[curr].f_type=="d"){
      // cout<<"###inside else\n";
      //cout<<"$$$path: "<<vec[curr].f_path<<"\n";
      // cout<<"$$$dirpath: "<<dir_path<<"\n";
      //dir_path=vec[curr].f_path;
      //cout<<"$$$backward_stack: "<<backward_stack.top()<<"\n";
      if(vec[curr].f_path != backward_stack.top())
         backward_stack.push(vec[curr].f_path);
      
      //cout<<"$$$updateddirpath: "<<dir_path<<"\n";
      //c='q';
      //break;
   }

   if(c == 'D'){ //left arrow
      if(!backward_stack.empty()){
         forward_stack.push(backward_stack.top()); //extract top from backward stack and push to forward stack
         backward_stack.pop();   //pop form backward stack
      }    
   }

   if(c == 'C'){ //right arrow
      if(!forward_stack.empty()){
         backward_stack.push(forward_stack.top()); //extract top from forward stack and push to backward stack
         forward_stack.pop();   //pop form forward stack
      }
   }

   if( c == 127){
      cout<<"backspace detected\n";
      if(!backward_stack.empty()){
         string temp=backward_stack.top();
         int pos = temp.find_last_of("/");
         string b_path=temp.substr(0, pos);
         cout<<"back path is: "<<b_path<<"\n";
         if(b_path=="")
            b_path="/";
         forward_stack.push(backward_stack.top());
         backward_stack.pop();
         backward_stack.push(b_path);
      }  
   }
   return c;
}

char refresh_dir_normal(stack<string> &forward_stack, stack<string> &backward_stack){
   struct dirent **file_list;

   int n=scandir(backward_stack.top().c_str(), &file_list, 0, alphasort);
   vector<FileDetails> vec;
   for(int i=0; i<n; ++i){
      struct stat st;
      
      string temp=backward_stack.top()+"/";
      if(strcmp(file_list[i]->d_name, ".")==0){
         //cout<<"&&filenames current dir: "<<file_list[i]->d_name<<endl;
         //temp=file_list[i]->d_name;
         temp=backward_stack.top();
      }else if(strcmp(file_list[i]->d_name, "..")==0){
         //cout<<"&&filenames prev dir: "<<file_list[i]->d_name<<endl;
         //temp=file_list[i]->d_name;
         temp=backward_stack.top();
         int pos = temp.find_last_of("/");
         string b_path=temp.substr(0, pos);
         //cout<<"back path is: "<<b_path<<"\n";
         if(b_path=="")
            b_path="/";
         
         temp = b_path;
         //cout<<temp;
      }else{
         //cout<<"$$filenames: "<<file_list[i]->d_name<<endl;
         temp+=file_list[i]->d_name;
      }
      stat(temp.c_str(), &st);
      off_t size=st.st_size;

      string file_type;
      if(S_ISDIR(st.st_mode)==0)
         file_type="-";
      else
         file_type="d";

      mode_t perm=st.st_mode;
      string ch=""+file_type;
      ch += (perm & S_IRUSR)?"r":"-";
      ch += (perm & S_IWUSR)?"w":"-";
      ch += (perm & S_IXUSR)?"x":"-";
      ch += (perm & S_IRGRP)?"r":"-";
      ch += (perm & S_IWGRP)?"w":"-";
      ch += (perm & S_IXGRP)?"x":"-";
      ch += (perm & S_IROTH)?"r":"-";
      ch += (perm & S_IWOTH)?"w":"-";
      ch += (perm & S_IXOTH)?"x":"-";
      FileDetails f(file_list[i]->d_name, size, ch, temp, file_type);
      vec.push_back(f);
      //cout<<ch<<"\t"<<size<<"\t"<<file_list[i]->d_name<<file_list<<"\n";
      free(file_list[i]);
   }
   
   free(file_list);
   //cout<<"control goes## "<<dir_path<<"\n";
   char ret_ch = print_files(vec, forward_stack, backward_stack);
   //cout<<"control returns## "<<dir_path<<"\n";

   return ret_ch;
}

void enable_echo(){
   // tcgetattr(STDIN_FILENO, &orig_termios);
   // atexit(disable_non_can_mode);
   struct termios raw=orig_termios;
   raw.c_lflag |= ECHO ;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
   cout<<"echo enalbled \n";
}

string refresh_dir_command(string &forward_stack, string &backward_stack){
   enable_echo();
   string command;
   cout<<"checking echo: ";
   cin>>command;
   return test;
}

int main() {
   stack<string> forward_stack;
   stack<string> backward_stack;
   enable_non_can_mode();
   //string dir_path="/home/anshita";
   char c='a';
   string sc="sc";
   do{  
      //cout<<"On the directory: "<<dir_path;
      if(switch_nc=='n'){
         forward_stack=stack<string>();
         backward_stack=stack<string>();
         backward_stack.push("/");
         backward_stack.push("/home");
         backward_stack.push("/home/anshita");
         c=refresh_dir_normal(forward_stack, backward_stack);
      }

      if(switch_nc=='c'){
         forward_stack=stack<string>();
         backward_stack=stack<string>();
         backward_stack.push("/");
         backward_stack.push("/home");
         backward_stack.push("/home/anshita");
         sc=refresh_dir_command(forward_stack, backward_stack);
      }
      // cout<<"On the directory: "<<dir_path;
      // cout<<"\nvalue of c is "<<c;
      if(c==';'){
         cout<<"enter command mode";
         //sc=command_mode();
         switch_nc='c';

      }
      // if(check if esc){
      //    cout<<"switch to normal mode";
      //    switch_nc='n';
      // }
      if(sc!="quit"){
         sc="sc";
      }
      if(c!='q'){
         c='a';
      }
      clear_terminal();
   }while(c != 'q' && sc != "quit");
   
   // cout<<"end of program\n";
   // int ed;
   // cin>>ed;
   // cout<<ed;
   return 0;
}