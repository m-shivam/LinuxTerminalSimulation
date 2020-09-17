#include <bits/stdc++.h>

#include <iostream>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <cstdio>


using namespace std;

char pwd[256];
string path="/home/shivam/";


//function prototyping
void init_shell(bool);
void getpwd();
vector<string> getArgs(string);
void shell_sort(string, bool);
time_t get_mtime(string);
string getfilename(string);
int copy(string, bool);
int getCodeFromCmd(string);
int ls(bool);
int cat(vector<string>, bool);
mode_t getMode(string);
int makedir(vector<string>, bool);
int mysort(vector<string>, bool);
int substring(string line,string str,int flag);
int grep(vector<string> args, int ind);



void init_shell(bool shellpath=false)
{
	if(shellpath){
		string shell="\nShiRu@linux:~";
		shell = shell+(pwd+12)+"$ ";
		cout<<shell;
	}
	else cout<<"\nShiRu@linux:~$ ";
}

void getpwd(){
	getcwd(pwd, 256);
}

vector<string> getArgs(string cmd){
	string tmp;
	vector<string> args;
	int i, ind, len;
	bool flag=false;
	tmp="";
	len = cmd.length();
	for(i=0; i<len; i++){
		if(cmd[i]==' ' && i<len && tmp!="")
		{
			//cout<<i<<" "<<tmp<<endl;
			args.push_back(tmp);
			tmp="";
			flag=false;
		}
		else{
			tmp = tmp + cmd[i];
			flag=false;
		}
		while(cmd[i]==' ' && cmd[i+1]==' ' && i<len){
			i++;
			flag=true;
		}
	}
	if(!flag && tmp!=""){
		//cout<<i<<" "<<tmp<<endl;
		args.push_back(tmp);
	}
	return args;
}

void shell_sort(string cmd, bool revflag=false){
	string tmp;
	vector<string> buf, v;
	v = getArgs(cmd);
	int i, n = v.size();
	for (i = 1; i < n; ++i)
	{
		if(v[i]=="")continue;
		ifstream filevar(v[i]);
		if (filevar.is_open())
		{
			while ( getline (filevar,tmp) )
			{
				//cout<<tmp<<endl;
				buf.push_back(tmp);
			}
			filevar.close();
		}
	  else cout << "Couldn't open file *"<< v[i] <<"*"<<endl;
	}
  	n = buf.size();
  	sort(buf.begin(), buf.end());

  	if(revflag)for (i = n-1; i > 0; --i)cout<<buf[i]<<endl;
  	else for (i = 0; i < n; ++i)cout<<buf[i]<<endl;
}

time_t get_mtime(string path)
{
    struct stat statbuf;
    if ( stat(path.c_str(), &statbuf) == -1 ) {
        return -1;
    }
    return statbuf.st_mtime;
}

string getfilename(string topath)
{
    string tmp;
    tmp = topath.substr(topath.find_last_of('\//') + 1);
    cout<<"file to be copied: *"<<tmp<<"*"<<endl;
    return tmp;
}

int copy(string cmd, bool updateflag=false)
{
    string from, to, tmp;
    int toind, fromind;

    fromind = cmd.find(' ');
    toind = cmd.find_last_of(' ');
    from = cmd.substr(fromind+1, toind-fromind-1);
    to = cmd.substr(toind+1);   
    if(to[to.length()-1]=='/')to=to+getfilename(from);
    else to=to+"/"+getfilename(from);

    cout<<"from: *"<<from<<"*\nto: *"<<to<<"*"<<endl;

    if(updateflag && (get_mtime(to)!=-1) && get_mtime(to)>get_mtime(from) ){
        cout<<"to: "<<get_mtime(to)<<"\nfrom:"<<get_mtime(from)<<endl;
        cout<<"option -u condition violated!\n";
        return 1;
    }

    ifstream in{from};
    remove(const_cast<char*>(to.c_str()));
    ofstream out{to};
    if (!out) {
        cerr << "Could not copy file to " << to << '\n';
        return 1;
    }
    static constexpr size_t buffsize{1024};
    char buffer[buffsize];
    while (in.read(buffer, buffsize)) {
        out.write(buffer, buffsize);
    }
    out.write(buffer, in.gcount()); 

	return 0;
}


int mysort(vector<string> v, bool revflag=false)
{
	string tmp;
	vector<string> buf;

	int i=1, n = v.size();
	if(revflag)i=2;
	for (; i < n; ++i)
	{
		if(v[i]=="")continue;
		ifstream filevar(v[i]);
		if (filevar.is_open())
		{
			while ( getline (filevar,tmp) )
			{
				//cout<<tmp<<endl;
				buf.push_back(tmp);
			}
			filevar.close();
		}
	  else{
	  	cout << "Couldn't open file *"<< v[i] <<"*"<<endl;
	  	return 1;
	  } 
	}
  	n = buf.size();
  	sort(buf.begin(), buf.end());

  	if(revflag)for (i = n-1; i > 0; --i)cout<<buf[i]<<endl;
  	else for (i = 0; i < n; ++i)cout<<buf[i]<<endl;
  	return 0;
}

// this function converts string argument to mode_t type.
mode_t getMode(string mode)
{
	mode_t ans=0, modar[]={0400, 0200, 0100, 0040, 0020, 0010, 0004, 0002, 0001};
	int rwx=0, bitval=0, ind=8;
	for(rwx=3; rwx>0; rwx--){
		bitval = mode[rwx]-'0';
		//cout<<ind<<" "<<mode[rwx]<<" "<<bitval<<endl;
		while(bitval>0){
			if(bitval%2)ans += modar[ind];
			bitval /= 2;
			ind--;
		}
		bitval=0;
	}
	//cout<<"mode:*"<<ans<<"*\n";
	return ans;
}

int makedir(vector<string> arg, bool modeflag=false)
{
	int i=1, len = arg.size();
	mode_t mode=0755;
	if(modeflag){
		i=3;
		mode = getMode(arg[2]);
	}
	for (; i < len; ++i)
			if(mkdir ( arg[i].c_str(), mode))
				cout << "Error : Check path or folder name\n";
}

int ls(bool hid=true)
{
	DIR *dirptr=NULL;
	struct dirent* dirdata;
	//char path[]="/home/shivam/College/Sem2/HPC/";
	getpwd();
	struct stat isfolder;
	dirptr = opendir(pwd);
	

	if(dirptr == NULL){
		cout<<"Cannot open specified dir";
		return 1;
	}
	while(dirdata = readdir(dirptr)){
		if(strcmp(dirdata->d_name, ".") && strcmp(dirdata->d_name, "..")){
			if(dirdata->d_name[0]=='.' && hid)continue;
			stat(dirdata->d_name,&isfolder);
			if(S_ISDIR(isfolder.st_mode))cout<<"\'"<<dirdata->d_name<<"\'"<<endl;
			else cout<<dirdata->d_name<<endl;
		}
	}
	closedir(dirptr);
	return 0;
}

int cat(vector<string> pat, bool show_lines=false)
{
	int lines=0, len, i;
	ifstream fp;
	string text, curpath;
	len = pat.size();
	//cout<<"\n*in_cat_pat:"<<pat[1]<<"* len"<<len<<endl; 
	
	getpwd();
	curpath = pwd;
	i = show_lines?2:1;
	for (; i < len; ++i)
	{	
		curpath = curpath+"/"+pat[i];
		cout<<"curpath:"<<curpath<<"*"<<endl;
		fp.open(curpath);
		if (!fp.is_open()){
			cout<<"Cannot found specified file.\n";
			return 0;
		}

		if(show_lines)while(getline(fp, text))cout<<setfill(' ')<<setw(6)<<++lines<<" "<<text<<endl;
		else while(getline(fp, text))cout<<text<<endl;
		curpath = pwd;
		fp.close();
	}
	


	return 0;
}

int substring(string line,string str,int flag)
{
    int count=0,len1=line.size(),len2=str.size(),x;
    if(flag==0){
        for(int i=0; i<=len1-len2; i++){
            x=0;
            while(x<str.size() && line[i+x] == str[x])x++;
            if(x == len2)count++;
        }
        return count;
    }else{
        for(int i=0; i<=len1-len2; i++){
            x=0;
            while(x<len2){
                if(line[i+x] == str[x])x++;
                else if((str[x]>='a' && str[x]<='z') && (line[i+x]>='A' && line[i+x]<='Z') && (str[x]-32 == line[i+x]))x++; 
                else if((str[x]>='A' && str[x]<='Z') && (line[i+x]>='a' && line[i+x]<='z') && (str[x]+32 == line[i+x]))x++;
                else{
                    x++;
                    break;
                }
            }
            if(x == len2)count++;
        }
    }
    return count;
}


int grep(vector<string> args, int ind)
{
    bool mulex=false;
    int fi=2,x=1, strlen=args.size();
   
    if(ind==0 && strlen>3)mulex=true;
    
    if(ind!=0){
        fi=3;
        x=2;
        if(strlen>4)mulex=true;
    }
    for (; fi < strlen; ++fi)
    {
        string line,str="";
        int flag = 0;
        int total = 0,linecount=0, c=0;
        int len = args[x].size();

        str=args[x].substr(1, len-2);
        // cout<<"without:"<<str<<endl;

        ifstream fp;
        fp.open(args[fi]);
        if (!fp.is_open()){
            cout<<"Cannot found specified file.\n";
            return 1;
        }
        
        if(ind==2)flag = 1;

        while(getline(fp,line)){
            
            c = substring(line,str,flag);
            //cout<<"\nc: "<<c<<endl;
            if(mulex && c && ind!=1)cout<<args[fi]<<":";
            if(c && ind==0){
                cout<<line<<endl;
            }else if(c && ind==1){
                total += c;
            }else if(c && ind==2){
                cout<<line<<endl;
            }else if(c && ind==3){
                cout<<linecount<<" "<<line<<endl;
            }
            linecount++;
        }
        if(total!=0)cout<<args[fi]<<":"<<total<<endl;
    }
    return 0;
}

int getCodeFromCmd(string cmd)
{	
	int code, i, len, exist, ind;
	string st, realcmd;
	vector<string> v;
	map <string, int> cmd2code;
	cmd2code.insert({ "ls", 1 });
	cmd2code.insert({ "ls-a", 2 });
	cmd2code.insert({ "cd", 3 });
	cmd2code.insert({ "cat", 4 });
	cmd2code.insert({ "cat-n", 5 });
	cmd2code.insert({ "mkdir", 6 });
	cmd2code.insert({ "mkdir-m", 7 });
	cmd2code.insert({ "cp", 8 });
	cmd2code.insert({ "cp-u", 9 });
	cmd2code.insert({ "sort", 10 });
	cmd2code.insert({ "sort-r", 11 });
	cmd2code.insert({ "grep", 12 });
	cmd2code.insert({ "grep-c", 13 });
	cmd2code.insert({ "grep-i", 14 });
	cmd2code.insert({ "grep-n", 15 });

	v = getArgs(cmd);
	if(v.size()>1)realcmd = v[0]+v[1];
	else realcmd=v[0];
	//cout<<"realcmd*"<<realcmd<<"*\n";
	if(cmd2code.find(realcmd)!=cmd2code.end())return cmd2code[realcmd];
	else if(cmd2code.find(v[0])!=cmd2code.end())return cmd2code[v[0]];

	return 0;
}



int main(int argc, char const *argv[])
{	
	

	cout<<"\n\n\n\n\n\n\n\n###############*My Terminal*###############\n\n"<<endl;
	init_shell();
	int cmdCode=-1;
	vector<string> arglist;
	string cmd, arg1, arg2;
	while(getline(cin, cmd)){
		arglist = getArgs(cmd);
		//cout<<"*"<<cmd<<"*"<<endl;
		if(cmd=="exit")break;
		cmdCode = getCodeFromCmd(cmd);
		switch(cmdCode){
			case 1: ls(); break;
			case 2: ls(false); break;
			case 3:  break;
			case 4: cat(arglist); break;
			case 5: cat(arglist, true); break;
			case 6:	makedir(arglist); break;
			case 7: makedir(arglist, true); break;
			case 8:	copy(cmd); break;
			case 9: copy(cmd, true); break;
			case 10: mysort(arglist); break;
			case 11: mysort(arglist, true); break;
			case 12: grep(arglist,0); break;
			case 13: grep(arglist,1); break;
			case 14: grep(arglist,2); break;
			case 15: grep(arglist,3); break;
			default: cout<<"command not found!"<<endl;
		}

		//cout<<"Command is *"<<cmd<<"* code is "<<cmdCode;
		getpwd();
		init_shell();
	}
	cout<<"Exiting...\n";
	return 0;
}