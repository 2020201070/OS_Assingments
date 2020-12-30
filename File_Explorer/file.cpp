#include<bits/stdc++.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<time.h>
#include<termios.h>
#include <sys/ioctl.h>
#include<pwd.h>
#include<grp.h>
#define r "/home/code"
using namespace std;
struct termios in;
void navigate();
void enable();
void disable();
int height,width;
int ct=0,cm=5;;
string path;
vector<string> d;
stack<string> st;
stack<string> fw;
int main()
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	height = win.ws_row - 1;
	width=win.ws_col;
	system("clear");
	char t[100]="";
	path=r;
	string s=path;
	DIR *dir;
 	struct dirent *sd;
 	dir=opendir(".");
 	ct=0;
 	if(dir == NULL)
 	{
 		cout<<"Error";
 		exit(1);
 	}
 	while((sd=readdir(dir)) != NULL)
 	{
 		d.push_back(string(sd->d_name));
 		char *fn=sd->d_name;
		struct stat sb;
		struct passwd *pw;
		struct group *grp;
		ct++;
		if(stat(fn,&sb)!= -1)
		{
			printf(S_ISDIR(sb.st_mode)?"d":"-");
			printf((sb.st_mode & S_IRUSR)?"r":"-");
			printf((sb.st_mode & S_IWUSR)?"w":"-");
			printf((sb.st_mode & S_IXUSR)?"x":"-");
			printf((sb.st_mode & S_IRGRP)?"r":"-");
			printf((sb.st_mode & S_IWGRP)?"w":"-");
			printf((sb.st_mode & S_IXGRP)?"x":"-");
			printf("%10ldB",sb.st_size);
			cout<<"\t";
			pw = getpwuid(sb.st_uid);
			grp = getgrgid(sb.st_gid);
			printf("%s",pw->pw_name);
			cout<<"\t";
			printf("%s",grp->gr_name);
			cout<<"\t \t";
			strftime(t,100,"%x - %I:%M%p",localtime(&sb.st_mtime));
			cout<<t<<"\t \t";
			if(S_ISDIR(sb.st_mode))
			{
				printf("\033[1;32m");
				printf("\t%-20s\n", fn);
				printf("\033[0m");
			}
			else
				printf("\t%-20s\n", fn);
		}
 		
 	}

 	enable();
 	navigate();
 	disable();
 	closedir(dir);
 	
}
void opndict(const char* p)
{
	system("clear");
	ct=0;
	string s=p;
	//cout<<s;
	char t[100]="";
	DIR *dir;
 	struct dirent *sd;
 	dir=opendir(s.c_str());
 	d.clear();
 	if(dir == NULL)
 	{
 		cout<<"Error";
 		exit(1);
 	}
 	while((sd=readdir(dir)) != NULL)
 	{
 		
 		d.push_back(string(sd->d_name));
 		string temp=s+"/"+string(sd->d_name);
 		char *fn=new char[temp.length()+1];
 		strcpy(fn,temp.c_str());
		struct stat sb;
		struct passwd *pw;
		struct group *grp;
		ct++;
		if(stat(fn,&sb)== -1)
		{
		
		}
		{
			printf(S_ISDIR(sb.st_mode)?"d":"-");
			printf((sb.st_mode & S_IRUSR)?"r":"-");
			printf((sb.st_mode & S_IWUSR)?"w":"-");
			printf((sb.st_mode & S_IXUSR)?"x":"-");
			printf((sb.st_mode & S_IRGRP)?"r":"-");
			printf((sb.st_mode & S_IWGRP)?"w":"-");
			printf((sb.st_mode & S_IXGRP)?"x":"-");
			printf("%10ldB",sb.st_size);
			cout<<"\t";
			pw = getpwuid(sb.st_uid);
			grp = getgrgid(sb.st_gid);
			printf("%s",pw->pw_name);
			cout<<"\t";
			printf("%s",grp->gr_name);
			cout<<"\t \t";
			strftime(t,100,"%x - %I:%M%p",localtime(&sb.st_mtime));
			cout<<t<<"\t \t";
			if(S_ISDIR(sb.st_mode))
			{
				printf("\033[1;32m");
				printf("\t%-20s\n", fn);
				printf("\033[0m");
			}
			else
				printf("\t%-20s\n", fn);
		}
 		
 	}
 	closedir(dir);
}
void opnfl(const char* path)
{
	int fp=open(path,O_RDONLY);
	close(fp);
	pid_t processID = fork();
	if(processID == 0)
	{
		execlp("xdg-open","xdg-open",path,NULL);
		exit(0);
	}
}
void enable()
{
	struct termios ne;
	tcgetattr(STDIN_FILENO, &in);
	ne= in;
	ne.c_lflag &= ~(ECHO | ICANON);
	ne.c_cc[VMIN]=1;
	ne.c_cc[VTIME]=0;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &ne) != 0) 
		fprintf(stderr,"Could not set attributes\n");
	
}
void disable()
{
	tcsetattr(STDIN_FILENO,TCSANOW,&in);
}
void scroll(int min,int max)
{
	system("clear");
	int ran=0;
	string s=path;
	//cout<<s;
	char t[100]="";
	DIR *dir;
 	struct dirent *sd;
 	dir=opendir(s.c_str());
 	if(dir == NULL)
 	{
 		cout<<"Error";
 		exit(1);
 	}
 	while((sd=readdir(dir)) != NULL)
 	{
 		if(ran<min || ran>max)
 		{
 			ran++;
 		}
 		else
 		{
	 		string temp=s+"/"+string(sd->d_name);
	 		char *fn=new char[temp.length()+1];
	 		strcpy(fn,temp.c_str());
			struct stat sb;
			struct passwd *pw;
			struct group *grp;
			ran++;
			if(stat(fn,&sb)== -1)
			{
			
			}
			printf(S_ISDIR(sb.st_mode)?"d":"-");
			printf((sb.st_mode & S_IRUSR)?"r":"-");
			printf((sb.st_mode & S_IWUSR)?"w":"-");
			printf((sb.st_mode & S_IXUSR)?"x":"-");
			printf((sb.st_mode & S_IRGRP)?"r":"-");
			printf((sb.st_mode & S_IWGRP)?"w":"-");
			printf((sb.st_mode & S_IXGRP)?"x":"-");
			printf("%10ldB",sb.st_size);
			cout<<"\t";
			pw = getpwuid(sb.st_uid);
			grp = getgrgid(sb.st_gid);
			printf("%s",pw->pw_name);
			cout<<"\t";
			printf("%s",grp->gr_name);
			cout<<"\t \t";
			strftime(t,100,"%x - %I:%M%p",localtime(&sb.st_mtime));
			cout<<t<<"\t \t";
			if(S_ISDIR(sb.st_mode))
			{
				printf("\033[1;32m");
				printf("\t%-20s\n", fn);
				printf("\033[0m");
			}
			else
				printf("\t%-20s\n", fn);
		}	
 		
 	}
 	
 	closedir(dir);
}
void create_file(string src,string dest)
{
	int f;
	string temp;
	temp=dest+"/"+src;
	f=open(temp.c_str(),O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP);
	const char* p=path.c_str();
	opndict(p);
			
	
}
void create_dir(string src,string dest)
{
	string temp=dest+"/"+src;
	mkdir(temp.c_str(),S_IRWXU|S_IRWXG);
	const char* p=path.c_str();
	opndict(p);	
}
void copy(string src,string dest)
{
	int fo,fi,re;
	struct stat sb;
	char buf[100];
	char a='/';
	string temp;
	size_t l=src.find_last_of(a);
	temp=src.substr(l+1);
	temp=dest+"/"+temp;
	fi=open(src.c_str(),O_RDONLY);
	fo=open(temp.c_str(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
	while(re=read(fi,buf,sizeof(buf)))
		write(fo,buf,re);
	const char* p=path.c_str();
	opndict(p);

}
void copy_dir(string src,string dest)
{
	const char* fn=src.c_str();	
	struct stat sb;
	stat(fn,&sb);
	if(S_ISDIR(sb.st_mode))
	{
		char a='/';
		string temp;
		size_t l=src.find_last_of(a);
		temp=src.substr(l+1);
		create_dir(temp,dest);
		dest=dest+"/"+temp;
	}
	DIR *dir;
 	struct dirent *sd;
 	dir=opendir(src.c_str());
 	if(dir == NULL)
 	{
 		cout<<"Error";
 		exit(1);
 	}
 	while((sd=readdir(dir)) != NULL)
 	{	
 		if(string(sd->d_name)!=".." && string(sd->d_name)!=".")
 		{
	 		string temp=src+"/"+string(sd->d_name);
	 		char *fn=new char[temp.length()+1];
	 		strcpy(fn,temp.c_str());
	 		struct stat sb;
	 		if(stat(fn,&sb)!= -1)
	 		{
	 			if(S_ISDIR(sb.st_mode))
	 			{
	 				copy_dir(fn,dest);
	 			}
	 			else if(S_ISREG(sb.st_mode))
	 			{	
	 				
	 				copy(fn,dest);
	 			}
	 			else
	 			{
	 			}
	 		}
 		}
 	}
}
void rename(string src,string dest)
{
	src=path+"/"+src;
	dest=path+"/"+dest;
	if(rename(src.c_str(), dest.c_str())==-1)
		cout<<"Error";
	const char* p=path.c_str();
	opndict(p);
			
}

void ret_to_ab(string &src)
{
	string temp=r;	
	if(src[0]=='~')
	{
		temp=temp+src.substr(1);
	}
	else if(src[0]=='.' && src[1]=='.')
	{
		char a='/';
		size_t l=temp.find_last_of(a);
		temp=temp.substr(0,l);	
		temp=temp+src.substr(2);	
	}
	else if(src[0]=='.')
	{
		temp=temp+src.substr(1);
	}
	else
	{
		temp=src;
	}
	src=temp;
}
void del_file(string src)
{
	ret_to_ab(src);
	if(remove(src.c_str())==-1)
		cout<<"Error";
	const char* p=path.c_str();
	opndict(p);
}
void del_dict(string src)
{
	ret_to_ab(src);
	if(remove(src.c_str())==-1)
		cout<<"Error";
}
void del(string src)
{
	int flag=0;
	DIR *dir;
 	struct dirent *sd;
 	dir=opendir(src.c_str());
 	if(dir == NULL)
 	{
 		return;
 	}
 	while((sd=readdir(dir)) != NULL )
 	{
 		if(string(sd->d_name)!=".." && string(sd->d_name)!=".")
 		{
	 		string temp=src+"/"+string(sd->d_name);
	 		char *fn=new char[temp.length()+1];
	 		strcpy(fn,temp.c_str());
	 		struct stat sb;
	 		
			if(stat(fn,&sb)!= -1 )
			{
				if(S_ISREG(sb.st_mode))
					del_file(temp);
				if(S_ISDIR(sb.st_mode))
				{	
					del(temp);
					del_dict(temp);
				}
			}
		}
	}
	
}
void got(string src)
{
	path=src;
	const char* p=path.c_str();
	opndict(p); 
}
int search(string src,string dest)
{
	int flag=0;
	DIR *dir;
	dest=src+"/"+dest;
 	struct dirent *sd;
 	dir=opendir(src.c_str());
 	if(dir == NULL)
 	{
 		return 0;
 	}
 	while((sd=readdir(dir)) != NULL)
 	{
 		string temp=src+"/"+string(sd->d_name);
 		char *fn=new char[temp.length()+1];
 		strcpy(fn,temp.c_str());
		if(temp==dest)
		{
			flag=1;
			break;
		}
	}
	if(flag==0)
		return 0;
	else
		return 1;	
}
void sep(vector<char> &s,vector<string> &vc)
{
	string k="";
	for(auto a:s)
	{
		if(a==' ')
		{
			vc.push_back(k);
			k="";
		}
		else
		{
			k=k+a;
		}
	}
	vc.push_back(k);
}
void navigate()
{
	int total=ct;
	while(1)
	{
		char c;
		c=cin.get();
			if(c==27)
			{
				c=cin.get();
				c=cin.get();
				switch(c)
				{
					case 'A':
						{
							total--;
							printf("\033[1A");
							break;
						}
					case 'B':
						{
							total++;
							printf("\033[1B");
							break;
						}
					case 'C':
						{
							//right
							if(!fw.empty())
							{
								string a=fw.top();
										
								st.push(a);
								path=a;
								fw.pop();
								const char* p=a.c_str();
								struct stat sb;
								stat(p,&sb);
								if(S_ISDIR(sb.st_mode))
									opndict(p);
								total=ct;
							}
							break;
						}
					
					case 'D':
						{
							//left
							if(!st.empty())
							{
								string a=st.top();
								fw.push(path);
								path=a;
								st.pop();
								const char* p=a.c_str();
								struct stat sb;
								stat(p,&sb);
								if(S_ISDIR(sb.st_mode))
									opndict(p);
								total=ct;
							}
							break;
						}
					default:
						{
							
						}
				}
			}
			else if(c==104 || c ==72)
			{
				//home;
				st.push(path);
				string a=".";
				path=r;
				const char* p=a.c_str();
				opndict(p);
				total=ct;
			}
			else if(c==10)
			{
				//Enter
				string temp=path;
				string a=d[total];
				temp=temp+"/"+a;
				const char* p=temp.c_str();
				struct stat sb;
				stat(p,&sb);
				if(S_ISDIR(sb.st_mode))
				{
					string y;
					char i='/';
					size_t x=path.find_last_of(i);
					y=path.substr(0,x);
					const char* k;
					if(a!="." && a!="..")
					{
						st.push(path);
						path=temp;
						k=path.c_str();
					}
					else if(a==".")
					{
						k=path.c_str();
					}
					
					else if(a=="..")
					{
						int freq=0;
						for(auto j:path)
						{
							if(j==i)
								freq++;	
						}
						if(freq>1)
						{
							path=y;
							k=path.c_str();
						}
						else
						{
							k=path.c_str();
						}
					}
					opndict(k);
					total=ct;
					
					
				}
				else if(S_ISREG(sb.st_mode))
				{
					opnfl(p);
				}
					
				
			}
			else if(c==107)
			{
				//scroll up
				int line;
				if(ct<=height)
				{
					line=ct;
					
				}
				else
				{	if((total+1)<ct)
					{
						scroll((total+1)-height,total+1);
						line=total+1;
						line=line+1;
					}
				}
				total=line;
				
			}
			else if(c==108)
			{
				//scroll down
				int line;
				if(ct<=height)
				{
					line=ct;
				}
				else
				{	if((total-1)-height>0)
					{
						scroll((total-1)-height,total-1);
						line=total-1;
						line=line+1;
					}
				}
				total=line;
				
			}
			else if(c==127)
			{
				//baskspace
				while(!fw.empty())
				{
					fw.pop();
				}
				
				char a='/';
				int freq=0;
				const char* p;
				for(auto i:path)
				{
					if(i==a)
						freq++;	
				}
				if(freq>1)
				{
					size_t l=path.find_last_of(a);
					path=path.substr(0,l);
					p=path.c_str();
				}
				else
				{
					p=path.c_str();
				}
				struct stat sb;
				stat(p,&sb);
				if(S_ISDIR(sb.st_mode))
				{
					const char* k=path.c_str();
					opndict(k);
					total=ct;
				}
				
			}
			else if(c==58)
			{
				//command mode
			 	while(1)
			 	{
			 		cm=cm+ct+1;
				 	cout<<"\033["<<cm+ct<<";"<<0<<"H";
				 	printf("======COMMAND MODE======\n");
					 char ci;
					 ci=cin.get();
					 if(ci==27)
					 {
					 	const char* p=path.c_str();
					 	opndict(p);
					 	break;
					 }
					 else
					 {
						 vector<char> v;
						 vector<string> vc;
						 while(ci!=10)
						 {
						 	if(ci==127)
						 	{
						 		printf("\033[1K");
						 		cout<<"\033["<<cm+1<<";"<<0<<"H";
						 		v.pop_back();
						 		for(int i=0;i<v.size();i++)
						 			cout<<v[i];
						 		ci=cin.get();	
						 	}
						 	else
						 	{
							 	cout<<ci;
							 	v.push_back(ci);
							 	ci=cin.get();
						 	}
						 }
						sep(v,vc);
						if(vc[0]=="copy")
						{
							ret_to_ab(vc[vc.size()-1]);
							const char* fn=vc[1].c_str();	
							struct stat sb;
							stat(fn,&sb);
							if(S_ISREG(sb.st_mode))
							{
							 	for(int k=1;k<vc.size()-1;k++)
							 	{
							 		ret_to_ab(vc[k]);
							 		copy(vc[k],vc[vc.size()-1]);
							 	}
						 	}
						 	else if(S_ISDIR(sb.st_mode))
						 	{
						 		for(int k=1;k<vc.size()-1;k++)
							 	{
							 		ret_to_ab(vc[k]);
							 		copy_dir(vc[k],vc[vc.size()-1]);
							 	}
						 	}
						}
						else if(vc[0]=="move")
						{
							ret_to_ab(vc[vc.size()-1]);
							const char* fn=vc[1].c_str();	
							struct stat sb;
							stat(fn,&sb);
							if(S_ISREG(sb.st_mode))
							{
								for(int k=1;k<vc.size()-1;k++)
							 	{
							 		ret_to_ab(vc[k]);
							 		copy(vc[k],vc[vc.size()-1]);
							 		del_file(vc[k]);
							 	}
						 	}
						 	else if(S_ISDIR(sb.st_mode))
						 	{
						 		for(int k=1;k<vc.size()-1;k++)
							 	{
							 		ret_to_ab(vc[k]);
							 		copy_dir(vc[k],vc[vc.size()-1]);
							 		del(vc[k]);
							 		del_dict(vc[k]);
							 	}
						 	}
						 	//break;
						}
						else if(vc[0]=="rename")
						{
							ret_to_ab(vc[2]);
							const char* fn=vc[1].c_str();	
							struct stat sb;
							stat(fn,&sb);
							if(S_ISREG(sb.st_mode))
							{
						 		rename(vc[1],vc[2]);
						 	}
						 	else if(S_ISDIR(sb.st_mode))
						 	{	
						 		rename(vc[1],vc[2]);
						 	}
						}
						else if(vc[0]=="create_file")
						{
							ret_to_ab(vc[2]);
							create_file(vc[1],vc[2]);
							
						}
						else if(vc[0]=="create_dir")
						{
							ret_to_ab(vc[2]);
							create_dir(vc[1],vc[2]);
							
						}
						else if(vc[0]=="delete_file")
						{
							del_file(vc[1]);
							
						}
						else if(vc[0]=="goto")
						{
							got(vc[1]);
							
						}
						else if(vc[0]=="search")
						{	
							
							int ret=search(path,vc[1]);
							const char* p=path.c_str();
							opndict(p);
							if(ret==0)
								cout<<"\n"<<"NO"<<endl;
							else
								cout<<"\n"<<"YES"<<endl;
						}
						else if(vc[0]=="delete_dir")
						{	
							ret_to_ab(vc[1]);
							del(vc[1]);
							del_dict(vc[1]);
							const char* p=path.c_str();
							opndict(p);
						}
					}
				}
			}	
	}	
}
