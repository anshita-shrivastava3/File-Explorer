#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <cstring>
#include <iomanip>
#include <vector>
#include <stack>
#include <pwd.h>
#include <grp.h>
#include <time.h>
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
   string f_size;
   string f_path;
   string f_type;
   string f_user;
   string f_group;
   string f_time;

   FileDetails(string name, string size, string perm, string path, string type, string ouser, string ogroup, string ftime){
      f_name=name;
      f_perm=perm;
      f_size=size;
      f_path=path;
      f_type=type;
      f_user=ouser;
      f_group=ogroup;
      f_time=ftime;
   }
};



void clear_terminal(){
   cout << "\033c";
}

void disable_non_can_mode(){
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

string FormatBytes(long long bytes)
{
    string arr[5]={"B","KB","MB","GB","TB"};
    string s="";
    int i;
    int res = bytes;
    for (i = 0; i < 5 && bytes >= 1024; i++, bytes /= 1024){
        res = bytes / 1024;
    }
 
    s=to_string(res);
 
    return (s + " " + arr[i]);
}

void enable_non_can_mode(){
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
      pid_t pid=fork();
      if(pid<0){
         perror("fork failed");
         exit(0);
      }
      if(pid==0){
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
   int n=(ws.ws_row-no_of_items-1);
   for (int y = 0; y < n; y++) {
    write(STDOUT_FILENO, "\n", 1);
    if (y == n-1) {
      if(switch_nc=='n'){
         write(STDOUT_FILENO, "Normal Mode::", 14);
         write(STDOUT_FILENO, print_path.c_str(), x);
      }else{
         write(STDOUT_FILENO, "Command Mode::", 14);
         write(STDOUT_FILENO, print_path.c_str(), x);
      }
    }
  }
}

char print_files(vector<FileDetails> &vec, stack<string> &forward_stack, stack<string> &backward_stack){
    int curr=0;
    int st=0;         //defined for vertical overflow
    int ed=19;        //defined for vertical overflow
    char k, e, y;

    if(vec.size()<=20){
        while(k!=58 && k!='q' && k!=127){
            clear_terminal();
            for(int i=0; i<vec.size(); ++i){
                if(curr==i){
                cout<<"->";
                cout<<setw(18)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(20)<<vec[i].f_name<<"\n";
                }else{
                cout<<setw(20)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(20)<<vec[i].f_name<<"\n";
                }
            }
            set_status(backward_stack.top(), vec.size());
            k=cin.get();
            cout<<"val of k: "<<int(k);
            if(k==27){
                cin>>e>>y;
                if(y==65){
                    //cout<<"up arrow\n";
                    if(curr>0)
                        curr--;
                }else if(y==66){
                    //cout<<"down arrow\n";
                    if(curr<vec.size()-1)
                        curr++;
                }else if(y==67){
                    //cout<<"right key\n";
                    break;
                }else if(y==68){
                    // cout<<"left key\n";
                    break;
                }else if(y==72){
                    // cout<<"home key\n";        
                }
            }else if(k==10 && vec[curr].f_type!="d"){
                open_file(vec, curr);
            }else if(k==10 && vec[curr].f_type=="d"){
                break;
            }
        }
    }else{
        while(k!=58 && k!='q' && k!=127){
            clear_terminal();
            for(int i=st; i<=ed; ++i){
                if(curr==i){
                cout<<"->";
                cout<<setw(18)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(20)<<vec[i].f_name<<"\n";
                }else{
                cout<<setw(20)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(20)<<vec[i].f_name<<"\n";
                }
            }
            set_status(backward_stack.top(), 20);
            k=cin.get();
            if(k==27){
                cin>>e>>y;
                if(y==65 && curr!=0){
                    if(curr==st){
                        st--;
                        ed--;
                        curr=st;
                    }else{
                        curr--;
                    }
                }else if(y==66 && ed!=vec.size()-1){
                    if(curr==ed){
                        st++;
                        ed++;
                        curr=ed;
                    }else{
                        curr++;
                    }
                }else if(y==67){
                    break;
                }else if(y==68){
                    break;
                }else if(y==72){
                }
            }else if(k==10 && vec[curr].f_type!="d"){
                open_file(vec, curr);
            }else if(k==10 && vec[curr].f_type=="d"){
                break;
            }
        }
    }

    if(k==10 && vec[curr].f_type=="d"){         //enter into a directory
        if(vec[curr].f_path != backward_stack.top())
            backward_stack.push(vec[curr].f_path);
    }

    if(k==27 && y==67){     //right key
        if(!forward_stack.empty()){
            backward_stack.push(forward_stack.top()); //extract top from forward stack and push to backward stack
            forward_stack.pop();   //pop form forward stack
        }
    }

    if(k==27 && y==68){     //left key
        if(!backward_stack.empty()){
            forward_stack.push(backward_stack.top()); //extract top from backward stack and push to forward stack
            backward_stack.pop();   //pop form backward stack
        }  
    }

    if(k==127){
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

    if(k==58){
        switch_nc='c';
    }

    return k;
}

char refresh_dir_normal(stack<string> &forward_stack, stack<string> &backward_stack){
    struct dirent **file_list;

    int n=scandir(backward_stack.top().c_str(), &file_list, 0, alphasort);
    vector<FileDetails> vec;
    for(int i=0; i<n; ++i){
        struct stat st;
        
        string temp=backward_stack.top()+"/";
        if(strcmp(file_list[i]->d_name, ".")==0){
            temp=backward_stack.top();
        }else if(strcmp(file_list[i]->d_name, "..")==0){
            temp=backward_stack.top();
            int pos = temp.find_last_of("/");
            string b_path=temp.substr(0, pos);
            if(b_path=="")
                b_path="/";
            
            temp = b_path;
        }else{
            temp+=file_list[i]->d_name;
        }
        stat(temp.c_str(), &st);
        off_t size=st.st_size;
        string fsize = FormatBytes(size);
        string file_type;
        if(S_ISDIR(st.st_mode)==0)
            file_type="-";
        else
            file_type="d";
        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);  

        struct tm lt;
        time_t t = st.st_mtime; /*st_mtime is type time_t */
        char mtime[80];
        localtime_r(&t, &lt); /* convert to struct tm */
        strftime(mtime, sizeof mtime, "%d %b %Y %T", &lt);

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
        FileDetails f(file_list[i]->d_name, fsize, ch, temp, file_type, pw->pw_name, gr->gr_name, mtime);
        vec.push_back(f);
        free(file_list[i]);
    }
   
    free(file_list);
    char ret_ch = print_files(vec, forward_stack, backward_stack);

    return ret_ch;
}

string get_home(){
    return getpwuid(getuid())->pw_dir;
}


string refresh_dir_command(string pwd){
    clear_terminal();
    cout<<"\n";
    set_status(pwd, 2);
    cout<<"\nType command: ";
    string s;
    cin>>s;
    return s;
}

int main(){
    enable_non_can_mode();
    stack<string> forward_stack;
    stack<string> backward_stack;
    backward_stack.push(get_home());
    char c='a';
    string sc="sc";
    do{  
        if(switch_nc=='n'){
            c=refresh_dir_normal(forward_stack, backward_stack);
        }

        if(switch_nc=='c'){
          //  cout<<"switched to command mode\n";
            sc=refresh_dir_command(backward_stack.top());
           //cout<<"control returns :"<<sc<<endl;
        }
        
        if(sc!="quit"){
            sc="sc";
        }
        if(c!='q'){
            c='a';
        }
        clear_terminal();
    }while(c != 'q' && sc != "quit");
 
    return 0;
}