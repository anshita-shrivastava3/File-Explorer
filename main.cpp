/*==============================================================================
Advanced Operating System: Assignment 1
Anshita Shrivastava
2022202014
Terminal based File Explorer
================================================================================*/
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <cstring>
#include <string>
#include <iomanip>
#include <vector>
#include <stack>
#include <pwd.h>
#include <grp.h>
#include <utility>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
using namespace std;

struct termios orig_termios;    
char switch_nc='n';
string pwds;

/*==============================================================================
Class to store files and info of the directory currently being displayed in Normal 
mode.
================================================================================*/
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

/*==============================================================================
To clear the terminal.
================================================================================*/
void clear_terminal(){
   cout << "\033c";
}

/*==============================================================================
Disables non canonical mode to go back to canonical mode.
================================================================================*/
void disable_non_can_mode(){
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/*==============================================================================
Helper function to converted file size to bytes.
================================================================================*/
string change_to_bytes(long long inp)
{
    string arr[5]={"B","KB","MB","GB","TB"};
    string s="";
    int i;
    int res = inp;
    for (i = 0; i<5 && inp>=1024; i++, inp/=1024){
        res = inp / 1024;
    }
 
    s=to_string(res);
 
    return (s + " " + arr[i]);
}

/*==============================================================================
To enable non canonical mode.
================================================================================*/
void enable_non_can_mode(){
   tcgetattr(STDIN_FILENO, &orig_termios);
   atexit(disable_non_can_mode);
   struct termios raw=orig_termios;
   raw.c_lflag &= ~(ICANON);
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*==============================================================================
Error handler function being called at multiple places. Parameter 'err' is being
passed by all calling function giving details of the error. 
================================================================================*/
void error_handler(string err=""){
    cout<<"Error "<<err<<" (screen refreshes in 3 seconds)";
    fflush(stdout);
    unsigned int microsecond = 1000000;
    usleep(3 * microsecond);
}

/*==============================================================================
To open the selected file in gedit.
================================================================================*/
void open_file(vector<FileDetails> &vec, int &curr){
   FILE *currfile;
   currfile=fopen((vec[curr].f_path).c_str(), "a");

   if(currfile!=NULL){
        pid_t pid=fork();
        if(pid<0){
            error_handler(":in create fork.");
            exit(0);
        }
        if(pid==0){
            char *exec_args[3] = {(char*)("/usr/bin/gedit"), (char*)((vec[curr].f_path).c_str()), NULL};
            execv(exec_args[0], exec_args);
        }
   }

   fclose(currfile);
}


/*==============================================================================
To set the status at the bottomof the terminal in both Normal and Command mode
================================================================================*/
void set_status(string print_path, int no_of_items){
    char to_set [PATH_MAX+1];
    realpath(print_path.c_str(), to_set);
    struct winsize ws;
    int x=print_path.size();
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    int n=(ws.ws_row-no_of_items-1);
    for (int y = 0; y < n; y++) {
        write(STDOUT_FILENO, "\n", 1);
        if (y == n-1) {
        if(switch_nc=='n'){
            write(STDOUT_FILENO, "Normal Mode::", 14);
            write(STDOUT_FILENO, to_set, x);
        }else{
            write(STDOUT_FILENO, "Command Mode::", 14);
            write(STDOUT_FILENO, to_set, x);
            write(STDOUT_FILENO, "\n", 1);
        }
        }
    }
}


/*==============================================================================
To get the home directory of the user.
================================================================================*/
string get_home(){
    return getpwuid(getuid())->pw_dir;
}


/*==============================================================================
To display files and directory of the pwd. Also handles arrow movements, h to 
go to home of the user and backspace to go one level up the pwd. 
================================================================================*/
char print_files(vector<FileDetails> &vec, stack<string> &forward_stack, stack<string> &backward_stack){
    int curr=0;
    int st=0;         
    int ed=19;        
    char k, e, y;

    if(vec.size()<=20){
        while(k!=58 && k!='q' && k!=127){
            clear_terminal();
            for(int i=0; i<vec.size(); ++i){
                if(curr==i){
                cout<<"->";
                cout<<setw(18)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(50)<<vec[i].f_name<<"\n";
                }else{
                cout<<setw(20)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(50)<<vec[i].f_name<<"\n";
                }
            }
            set_status(pwds, vec.size());
            
            k=cin.get();
            if(k==27){
                cin>>e>>y;
                if(y==65){
                    if(curr>0)
                        curr--;
                }else if(y==66){
                    if(curr<vec.size()-1)
                        curr++;
                }else if(y==67){
                    break;
                }else if(y==68){
                    break;
                }
            }else if(k==10 && vec[curr].f_type!="d"){
                open_file(vec, curr);
            }else if(k==10 && vec[curr].f_type=="d"){
                break;
            }else if(k=='h'){
                break;
            }
        }
    }else{
        while(k!=58 && k!='q' && k!=127){
            clear_terminal();
            for(int i=st; i<=ed; ++i){
                if(curr==i){
                cout<<"->";
                cout<<setw(18)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(50)<<vec[i].f_name<<"\n";
                }else{
                cout<<setw(20)<<vec[i].f_perm<<setw(20)<<vec[i].f_user<<setw(20)<<vec[i].f_group<<setw(20)<<vec[i].f_size<<setw(30)<<vec[i].f_time<<setw(50)<<vec[i].f_name<<"\n";
                }
            }
            set_status(pwds, 20);
            
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
                }else if(y==66 && curr!=vec.size()-1){
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
                }
            }else if(k==10 && vec[curr].f_type!="d"){
                open_file(vec, curr);
            }else if(k==10 && vec[curr].f_type=="d"){
                break;
            }else if(k=='h'){
                break;
            }
        }
    }

    if(k==10 && vec[curr].f_type=="d"){         

        if(pwds!=vec[curr].f_path){
            backward_stack.push(pwds);
            pwds=vec[curr].f_path;

            while(!forward_stack.empty()){
                forward_stack.pop();
            }
        }
        
    }

    if(k==27 && y==67){     //right

        if(!forward_stack.empty()){
            if(backward_stack.empty() || backward_stack.top()!=pwds)
                backward_stack.push(pwds);

            pwds=forward_stack.top();
            forward_stack.pop();
        }


    }

    if(k==27 && y==68){     //left

        if(!backward_stack.empty()){
            if(forward_stack.empty() || forward_stack.top()!=pwds)
                forward_stack.push(pwds);

            pwds=backward_stack.top();
            backward_stack.pop();
        }
    }

    if(k==127){   
        string temp=pwds;
        int pos = temp.find_last_of("/");
        string b_path=temp.substr(0, pos);
        if(b_path=="")
            b_path="/";

        if(backward_stack.empty() || backward_stack.top()!=pwds)
            backward_stack.push(pwds);
        pwds=b_path;        
    }

    if(k==58){
        switch_nc='c';
    }

    if(k=='h'){

        if(backward_stack.empty() || backward_stack.top()!=pwds)
            backward_stack.push(pwds);

        pwds=get_home();

        while(!forward_stack.empty()){
            forward_stack.pop();
        }
    }

    return k;
}


/*==============================================================================
To get the details of all the files in the pwd along with their meta data like
permission, owner/group, lastmodified date/time, size and name.
================================================================================*/
char refresh_dir_normal(stack<string> &forward_stack, stack<string> &backward_stack){
    struct dirent **file_list;
    vector<FileDetails> vec;
    char ret_ch;
    int n;

    n=scandir(pwds.c_str(), &file_list, 0, alphasort);
    
    for(int i=0; i<n; ++i){
        struct stat st;
        
        string temp=pwds+"/";
        if(strcmp(file_list[i]->d_name, ".")==0){
            temp=pwds;
        }else if(strcmp(file_list[i]->d_name, "..")==0){
            temp=pwds;
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
        string fsize = change_to_bytes(size);
        string file_type;
        if(S_ISDIR(st.st_mode)==0)
            file_type="-";
        else
            file_type="d";
        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);  

        struct tm lt;
        time_t t = st.st_mtime;
        char mtime[80];
        localtime_r(&t, &lt);
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
    ret_ch = print_files(vec, forward_stack, backward_stack);

    return ret_ch;
}


/*==============================================================================
To to break the space setaparated command into individual words for further 
processing.
================================================================================*/
vector<string> get_command(string s){
    stringstream temp(s);
    vector<string> command;
    string t;
    while (getline(temp, t, ' ')) {  
        command.push_back(t);
    }  

    return command;
}


/*==============================================================================
To get the absolute path from relative path of the file using stack. Handles ~(home), .(current
directory) , ..(previous directory).
================================================================================*/
string get_absolute_path(string rel_path){
    string ret;
    string temp;
    stack<string> p_st;

    if(rel_path.size()>0){
        if(rel_path=="~" || rel_path=="~/"){
            ret=get_home();
        }else{
            ret.append("/");
            if(rel_path[0]=='~'){
                rel_path=rel_path.substr(1);
                rel_path=get_home()+"/"+rel_path;
            }else{
                rel_path=pwds+"/"+rel_path;
            }
            int n = rel_path.length();
            for (int i=0; i< n; ++i) {
                temp.clear();
        
                while (rel_path[i] == '/')
                    i++;

                while (i<n && rel_path[i] != '/') {
                    temp.push_back(rel_path[i]);
                    i++;
                }

                if (temp.compare("..") == 0) {
                    if (!p_st.empty())
                        p_st.pop();           
                }else if(temp.compare(".") == 0){
                    continue;
                }else if(temp.length() != 0){
                    p_st.push(temp);
                }
            }

            stack<string> st1;
            while (!p_st.empty()) {
                st1.push(p_st.top());
                p_st.pop();
            }

            while (!st1.empty()) {
                string temp = st1.top();
                
                if (st1.size()!=1)
                    ret.append(temp+"/");
                else
                    ret.append(temp);
        
                st1.pop();
            }
        }
    }

    return ret;
}


/*==============================================================================
To get permissions of a file or directory. Being used in copying functions.
================================================================================*/
__mode_t get_permissions(string path){
    struct stat st;

    stat(path.c_str(), &st);
    __mode_t perms=st.st_mode;
    return perms;
}


/*==============================================================================
To get the owner and group of a file or directory. Being used in copying functions.
================================================================================*/
pair<uid_t, gid_t> get_owner_group(string path){
    struct stat st;

    stat(path.c_str(), &st);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);  

    uid_t uid = pw->pw_uid;
    gid_t gid = gr->gr_gid;

    return {uid, gid};
}

/*==============================================================================
Being called to handle create_dir command in Command mode.
================================================================================*/
string create_dir(string dir_name, string destination_path, mode_t mode){
    string d=get_absolute_path(destination_path);
    int res;

    d=d+"/"+dir_name;
    res=mkdir(d.c_str(), mode);
    if(res!=0){
        error_handler(":in creating directory.");
        return "";
    }    

    return d;
}


/*==============================================================================
Being called to handle create_file command in Command mode.
================================================================================*/
void create_file(string file_name, string destination_path){
    int fp;

    string file_path=get_absolute_path(destination_path)+"/"+file_name;
    fp=open(file_path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    close(fp);
}

void copy(string source_file, string destination_directory);

/*==============================================================================
Being called to handle copy of a directory command in Command mode.
================================================================================*/
void copy_dir(string source_file, string destination_directory){
    string f, s;

    f=get_absolute_path(source_file);
    s=get_absolute_path(destination_directory);

    vector<string> vec;
    int pos = f.find_last_of("/");
    string dir_name=f.substr(pos+1);
    __mode_t perms = get_permissions(f.c_str());
    string d_path=create_dir(dir_name, destination_directory, perms);
    pair<uid_t, gid_t> p=get_owner_group(f);
    if(chown(d_path.c_str(), p.first, p.second)==-1){
        error_handler(":in changing owner/group of copied dir.");
        return ;
    }
    struct dirent *entry;
    DIR *dir = opendir(f.c_str());
    if (dir == NULL) {
        return ;
    }
    while ((entry = readdir(dir)) != NULL) {
        vec.push_back(entry->d_name);
    }
    closedir(dir);
    sort(vec.begin(), vec.end());

    for(auto i=vec.begin()+2; i!=vec.end(); ++i){
        struct stat st;
        string new_path=f+"/"+(*i);
        stat(new_path.c_str(), &st);
        if(S_ISDIR(st.st_mode)!=0){
            string x= s+"/"+dir_name;
            copy_dir(new_path, x);
        }else{
            string temp=s+"/"+dir_name;
            copy(new_path, temp);
        }
    }
}

/*==============================================================================
Being called to handle copying a file or a directory command in Command mode.
================================================================================*/
void copy(string source_file, string destination_directory){
    char buf;
    int fd_one, fd_two;
    string f, s;
    int pos;
    
    f=get_absolute_path(source_file);
    s=get_absolute_path(destination_directory);

    struct stat st;
    stat(f.c_str(), &st);
    if(S_ISDIR(st.st_mode)!=0){
        copy_dir(source_file, destination_directory);
    }else{
        pos = f.find_last_of("/");
        s=s+f.substr(pos);
        __mode_t perms = get_permissions(f.c_str());
        fd_one = open(f.c_str(), O_RDONLY);
        if (fd_one == -1)
        {
            close(fd_one);
            error_handler(":in opening source file.");
            return ;
        }
        fd_two = open(s.c_str(), O_WRONLY | O_CREAT, perms);
        while(read(fd_one, &buf, 1))
        {
            write(fd_two, &buf, 1);
        }
        close(fd_one);
        close(fd_two);
        pair<uid_t, gid_t> p=get_owner_group(s);
        if(chown(s.c_str(), p.first, p.second)==-1){
            error_handler(":in changing owner/group of copied file.");
            return ;
        }
    }
}

/*==============================================================================
Being called to handle delete_file command in Command mode.
================================================================================*/
void delete_file(string d_file){
    string f;

    f=get_absolute_path(d_file);
    if( unlink(f.c_str()) != 0 ){
        error_handler(":in deleting file.");
        return ;
    }
}

/*==============================================================================
Helper function to check if a path is valid or not. Being used in goto_location()
function.
================================================================================*/
bool check_valid_path(string v_path){
    bool flag=false;

    int pos = v_path.find_last_of("/");
    string path=v_path.substr(0, pos);
    string dir_s=v_path.substr(pos+1);
    if(path=="")
        path="/";

    struct dirent *entry;
    DIR *dir = opendir(path.c_str());

    if (dir == NULL) {
        return false;
    }
    while ((entry = readdir(dir)) != NULL) {
        if(dir_s==entry->d_name){
            struct stat st;
            string goto_path=path+"/"+entry->d_name;
            stat(goto_path.c_str(), &st);
            if(S_ISDIR(st.st_mode)!=0){
                flag=true;
                break;
            }
        }
    }
    closedir(dir);

    return flag;
}

/*==============================================================================
Being called to handle goto command in Command mode.
================================================================================*/
void goto_location(string destination_path){
    string d_path=get_absolute_path(destination_path);

    if(check_valid_path(d_path))
        pwds=d_path;
    else
        error_handler(":not a valid path");
}

/*==============================================================================
Being called to handle search command in Command mode.
================================================================================*/
bool search(string psd, string search_for){
    vector<string> vec;
    struct dirent *entry;
    DIR *dir = opendir(psd.c_str());

    if (dir == NULL) {
        return false;
    }
    while ((entry = readdir(dir)) != NULL) {
        vec.push_back(entry->d_name);
    }
    closedir(dir);

    sort(vec.begin(), vec.end());
    
    for(auto i=vec.begin()+2; i!=vec.end(); ++i){
        struct stat st;
        string new_psd=psd+"/"+(*i);
        stat(new_psd.c_str(), &st);
        if(*i == search_for){
            return true;
        }else if(S_ISDIR(st.st_mode)!=0){
            if(search(new_psd, search_for))
                return true;
        }
    }

    return false;
}

/*==============================================================================
Being called to handle delete_dir command in Command mode.
================================================================================*/
void delete_dir(string dir_path){
    string path=get_absolute_path(dir_path);
    vector<string> vec;
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());

    if (dir == NULL) {
        return ;
    }
    while ((entry = readdir(dir)) != NULL) {
        vec.push_back(entry->d_name);
    }
    closedir(dir);

    sort(vec.begin(), vec.end());

    if(vec.size()<=2){
        if(rmdir(path.c_str())!=0){
            error_handler(":in deleting directory.");
            return ;
        }
        return ;
    }else{
        for(auto i=vec.begin()+2; i!=vec.end(); ++i){
            struct stat st;
            string new_path=path+"/"+(*i);
            stat(new_path.c_str(), &st);
            if(S_ISDIR(st.st_mode)!=0){
                delete_dir(new_path);
            }else{
                delete_file(new_path);
            }
        }
    }

    if(rmdir(path.c_str())!=0){
        error_handler(":in deleting directory.");
        return ;
    }
}

/*==============================================================================
Being called to handle move command in Command mode.
================================================================================*/
void move_file_dir(string source_file, string destination_directory){
    string s=get_absolute_path(source_file);
    string d=get_absolute_path(destination_directory);
    int pos;

    pos = s.find_last_of("/");
    d= d + s.substr(pos);

    if(rename(s.c_str(), d.c_str())==-1){
        error_handler(":in moving directory/file.");
        return ;
    }
}

/*==============================================================================
Being called to handle rename command in Command mode.
================================================================================*/
void rename_file_dir(string source_file, string destination_directory){
    string s=get_absolute_path(source_file);
    string d=get_absolute_path(destination_directory);
    int pos;

    if(rename(s.c_str(), d.c_str())==-1){
        error_handler(":in renaming file/directory.");
        return ;
    }
}

/*==============================================================================
To handle command mode. Responible for taking the input and breaking the commands
for further processing.
================================================================================*/
string refresh_dir_command(){
    cout<<"\n";
    set_status(pwds, 2);
    char ch;
    string s;

    ch=cin.get();
    if(ch==27){
        switch_nc='n';
        return "nm";
    }
    
    disable_non_can_mode();
    getline(cin, s);
    s= ch + s;
    vector<string> command=get_command(s);
    if(command.size()>=3 && command[0]=="copy"){
        int n=command.size();
        for(int i=1; i<n-1; ++i){
            copy(command[i], command[n-1]);
        }
    }else if(command.size()==2 && command[0]=="delete_file"){
        delete_file(command[1]);
    }else if(command.size()>=3 && command[0]=="move"){
        int n=command.size();
        for(int i=1; i<n-1; ++i){
            move_file_dir(command[i], command[n-1]);
        }
    }else if(command.size()==3 && command[0]=="rename"){
        rename_file_dir(command[1], command[2]);
    }else if(command.size()==3 && command[0]=="create_file"){
        create_file(command[1], command[2]);
    }else if(command.size()==2 && command[0]=="goto"){
        goto_location(command[1]);
    }else if(command.size()==3 && command[0]=="create_dir"){
        create_dir(command[1], command[2], 0777);
    }else if(command.size()==2 && command[0]=="search"){
        bool b=search(pwds, command[1]);
        if(b)
            cout<<"True (screen refreshes in 3 seconds)";
        else
            cout<<"False (screen refreshes in 3 seconds)";
        
        fflush(stdout);
        unsigned int microsecond = 1000000;
        usleep(3 * microsecond);
    }else if(command.size()==2 && command[0]=="delete_dir"){
        delete_dir(command[1]);
    }else if(s=="quit"){
        //do nothing
    }else{
        cout<<"Not a valid command (screen refreshes in 3 seconds)";
        fflush(stdout);
        unsigned int microsecond = 1000000;
        usleep(3 * microsecond);
    }

    enable_non_can_mode();
    return s;
}

/*==============================================================================
Main function
================================================================================*/
int main(){
    enable_non_can_mode();
    stack<string> forward_stack;
    stack<string> backward_stack;
    pwds=get_home();
    char c='a';
    string sc="sc";
    do{  
        if(switch_nc=='n'){
            c=refresh_dir_normal(forward_stack, backward_stack);
        }
        clear_terminal();
        if(switch_nc=='c'){
            sc=refresh_dir_command();
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