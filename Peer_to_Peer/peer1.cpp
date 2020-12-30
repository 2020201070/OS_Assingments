#include <bits/stdc++.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <string>
#include <openssl/sha.h> 
#include "info.h"
#define CHUNK_SIZE 512
pthread_mutex_t lock1= PTHREAD_MUTEX_INITIALIZER;
using namespace std;
struct thread_data
{
	int ipeer_id;//start chunk
	int fpeer_id;//end chunk
	int total_peer;//total chunk
	int tnoc;    //file size
	string filenm;//file name
	string path;//destination path
	string chunk_id;//file sha
	string ser_ips;//servers ip
	int ser_prt;//server port
	int sock;   //sock
};
string ptserv;
void seperate(string user,vector<string> &v)
{
	string k="";
	for(int i=0;i<user.size();i++)
	{
		if(user[i]==' ')
		{
			v.push_back(k);
			k="";
		}
		else
		{
			k=k+user[i];
		}
	}
	v.push_back(k);

}

void* serves(void* inps)  
{
	    string inpf=*((string *)inps);
	   // cout<<"hi server"<<inpf<<ptserv<<"\n";
	    int server_fd, new_socket, valread; 
	    struct sockaddr_in address; 
	    const char* c = inpf.c_str();
	    int opt = 1; 
	    int addrlen = sizeof(address); 
	    
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	    { 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	    } 
	    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) 
	    { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	    } 
	    address.sin_family = AF_INET; 
	    address.sin_addr.s_addr = inet_addr(c); 
	    address.sin_port = htons(stoi(ptserv));
	    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
	    { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	    } 
	    if (listen(server_fd, 5) < 0) 
	    { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	    } 
	    while(1)
	    {
	    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) 
		{ 
			perror("not accept"); 
			exit(EXIT_FAILURE); 
		} 
		int valread;
		int read_byte;
		vector<string> temp;
		char buffer[1024];
		bzero(buffer,1024);
		valread = read(new_socket, buffer, 1024);
		seperate(buffer,temp);
		/*for(auto a:temp)
			cout<<a<<" ";*/
		FILE *fp=fopen(temp[0].c_str(),"rb");
		if(stoi(temp[2])==(stoi(temp[3])-1))
		{
			read_byte=(stoi(temp[4]))-(stoi(temp[1])*CHUNK_SIZE);
		}
		else
		{
			read_byte=((stoi(temp[2])-stoi(temp[1]))+1)*CHUNK_SIZE;
		
		}
		
		int start_r=stoi(temp[1])*CHUNK_SIZE;
		char chunk_buff[read_byte];//size
		bzero(chunk_buff,read_byte);
		fseek(fp,start_r,SEEK_SET);
		fread(chunk_buff, read_byte, 1, fp);
		fflush(stdout);
		send(new_socket,chunk_buff,strlen(chunk_buff),0);
   		//printf("%s\n",chunk_buff);
   		fclose(fp);
	    }
	    return NULL;
} 

void* down_write(void* info)
{
	struct thread_data *details;
	details=(struct thread_data *) info;
/*	cout<< details->ipeer_id<<" ";
	cout<< details->fpeer_id<<" ";
	cout<< details->chunk_id<<" ";
	cout<< details->ser_ips<<" ";
	cout<< details->ser_prt<<" ";
	cout<< details->total_peer<<" ";
	cout<< details->tnoc<<" ";
	cout<< details->filenm<<" ";
	cout<< details->sock<<" ";*/
	struct sockaddr_in serv_addr; 
    	int sock;
    	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return NULL; 
	 } 
	 serv_addr.sin_family = AF_INET; 
	 serv_addr.sin_port = htons(details->ser_prt);
	 const char* c =(details->ser_ips).c_str();
	 if(inet_pton(AF_INET, c, &serv_addr.sin_addr)<=0)  
	 { 
		printf("\nInvalid address/ Address not supported \n"); 
		return NULL; 
	 }
	 if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	 { 
		printf("\nConnection Failed \n"); 
		return NULL; 
	 }
	 int sz;
	 if(details->total_peer-1==details->fpeer_id)
	 {
	 	sz=(details->tnoc)-((details->ipeer_id)*CHUNK_SIZE);
	 }
	 else if(details->total_peer-1>details->fpeer_id)
	 {
	 	sz=(((details->fpeer_id)-(details->fpeer_id))+1)*CHUNK_SIZE;
	 }
	 char buffer[sz];//size
	 char obuf[20];
	 bzero(buffer,sz);
	 string match_sha="";
	 string snt=details->filenm+" "+to_string(details->ipeer_id)+" "+to_string(details->fpeer_id)+" "+to_string(details->total_peer)+" "+to_string(details->tnoc);
	 int value;
	 fflush(stdout);
	 send(sock,snt.c_str(),snt.size(),0);
	 value=read(sock,buffer,sz);
	 char a='/';
	 size_t l=details->filenm.find_last_of(a);
	 //cout<<details->filenm;
	 string fdst=details->filenm.substr(l+1,(details->filenm).size());
	// cout<<fdst;
	//cout<<details->path;

	 FILE *fp=fopen(((details->path)+"/"+(fdst)).c_str(),"wb+");
	 pthread_mutex_lock(&lock1);
	 fflush(stdout);
	 //printf("%s\n",buffer);
	 fseek(fp,details->ipeer_id*CHUNK_SIZE,SEEK_SET);
	 fwrite(buffer,sz,1,fp);
	 memset(buffer,0,CHUNK_SIZE);
	 fflush(stdout);
	 pthread_mutex_unlock(&lock1);
	 fclose(fp);
	 fflush(stdout);
	 return NULL;
}

int main(int argc,char** argv) 
{ 
    string inpf="";
    string outf="";
    string ipserv="";
    string arg=argv[3];
    ifstream input(arg,ios::binary);
    getline(input,inpf);
    getline(input,outf);
    if(argc == 4)
    {
	ipserv = argv[1];
	ptserv = argv[2];
    }
    else
    {
    	cout<<"No of arguments doesnt match"<<"\n";
    	return -1;
    }
    pthread_t p1;
   // cout<<ipserv<<" "<<ptserv;
    if(pthread_create(&p1, NULL,serves,(void *)&ipserv)!= 0) 
    {
	cout<<"Failed to create client thread\n";
    } 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    const char* c = inpf.c_str();
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(stoi(outf)); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, c, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    while(tracker!=0)
    {
    	    string input;
    	    vector<string> cmd;
	    getline(cin,input);
	    seperate(input,cmd);
	    if(cmd[0]=="create_user")
	    {
	    	if(cmd.size()!=3)
	    	{
	    		fflush(stdout);
	    	   cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	    
	    }
	    else if(cmd[0]=="login")
	    {
	    	if(cmd.size()!=3)
	    	{
	    		fflush(stdout);
	    	   cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		string nxt=input+" "+ipserv+" "+ptserv;
	    		int value;
	    		send(sock,nxt.c_str(),nxt.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);	
	    	}
	    }
	    else if(cmd[0]=="create_group")
	    {
	    	if(cmd.size()!=2)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		//cout<<input;
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		printf("%s\n",buffer);
	    	}
	    }
	    else if(cmd[0]=="join_group")
	    {
	    	if(cmd.size()!=2)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	    }
	    else if(cmd[0]=="leave_group")
	    {
	    	if(cmd.size()!=2)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	    }
	   else if(cmd[0]=="accept_request")
	   {
	   	if(cmd.size()!=3)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
	   else if(cmd[0]=="list_requests")
	   {
	   	if(cmd.size()!=2)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
	   else if(cmd[0]=="list_groups")
	   {
	   	if(cmd.size()!=1)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
	   else if(cmd[0]=="upload_file")
	   {
	   	if(cmd.size()!=3)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
	   else if(cmd[0]=="download_file")
	   {
	   	if(cmd.size()!=4)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		ifstream inp(cmd[2],ios::binary);
			inp.seekg(0,ios::end);
			int sz=inp.tellg();
			inp.seekg(0);
			inp.close();
			int total_size=sz;
			sz=3*sz;
			//cout<<sz<<"\n";
	    		char file_det[sz];
	    		bzero(file_det,sz);
	    		int del=0;
	    		int value;
	    		vector<string> chunk_sha;
	    		vector<string> seed;
	    		vector<vector<string>> seed_ls;
	    		string trace="";
	    		string flsha="";
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		
	    		value=recv(sock,file_det,sz,0);
	    		fflush(stdout);
	    		//cout<<strlen(file_det);
	    		for(int i=0;i<strlen(file_det);i++)
	    		{
	    			
	    			if(file_det[i]==';')
	    			{
	    				
	    				if(del==0)
	    				{
	    					flsha=trace;
	    					trace="";
	    					del++;
	    				}
	    				else if(del==1)
	    				{
	    					chunk_sha.push_back(trace);
	    					trace="";
	    					del++;
	    				}
	    				
	    			}
	    			else if(file_det[i]=='|')
	    			{
	    				seed.push_back(trace);
	    				seed_ls.push_back(seed);
	    				seed.clear();
	    				trace="";
	    			}
	    			else if(file_det[i]==',')
	    			{
	    				
	    				if(del==1)
	    				{
	    					chunk_sha.push_back(trace);
	    					trace="";
	    				}
	    				if(del==2)
	    				{
	    					
	    					seed.push_back(trace);
	    					trace="";
	    				}
	    			}
	    			else
	    			{
	    				//cout<<a<<" ";
	    				trace=trace+file_det[i];	
	    			} 
	    		}
	    		//cout<<seed_ls.size()<<" "<<chunk_sha.size();
	    		 pthread_t threads[seed_ls.size()];
	    		 struct thread_data td[seed_ls.size()];
	    		 int chunk_no=0;
	    		 int partition=(chunk_sha.size()-1)/seed_ls.size();
	    		for(int i=0;i<seed_ls.size();i++)
	    		{
	    			
	    			td[i].sock=sock;//sock of the client side peer
	    			td[i].total_peer=chunk_sha.size()-1;//total no of chunks
	    			td[i].tnoc=total_size;//file size
				td[i].chunk_id=flsha;//file sha
				td[i].filenm=cmd[2];//file name
				td[i].ser_ips=seed_ls[i][0];//server ip
				td[i].ser_prt=stoi(seed_ls[i][1]);//servers port
				td[i].path=cmd[3];	
	    			if(i==seed_ls.size()-1)
	    			{
	    				td[i].ipeer_id=chunk_no;//start chunk
					td[i].fpeer_id=chunk_sha.size()-2;//last chunks
	    				if(pthread_create(&threads[i], NULL,down_write,(void *)&td[i])!= 0) 
					{
						cout<<"Failed to create client thread\n";
					} 	
	    			}
	    			else
	    			{	
	    				td[i].ipeer_id=chunk_no;//start chunk
					td[i].fpeer_id=chunk_no+partition-1;//last chunks
					chunk_no=td[i].fpeer_id+1;
		    			if(pthread_create(&threads[i], NULL,down_write,(void *)&td[i])!= 0) 
					{
						cout<<"Failed to create client thread\n";
					} 
				}
				
	    		}
	    		for(int i=0;i<seed_ls.size();i++)
				pthread_join(threads[i],NULL);	
	    	}
	   
	   }
	   else if(cmd[0]=="list_files")
	   {
	   	if(cmd.size()!=2)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   
	   }
	   else if(cmd[0]=="show_downloads")
	   {
	   	if(cmd.size()!=1)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
	   else if(cmd[0]=="logout")
	   {
	   	if(cmd.size()!=1)
	    	{
	    		fflush(stdout);
	    		cout<<"Invalid command"<<"\n";
	    	}
	    	else
	    	{
	    		char buffer[1024];
	    		bzero(buffer,1024);
	    		int value;
	    		fflush(stdout);
	    		send(sock,input.c_str(),input.size(),0);
	    		value=read(sock,buffer,1024);
	    		fflush(stdout);
	    		printf("%s\n",buffer);
	    	}
	   }
    }
    close(sock);
    pthread_exit(NULL);
    input.close();
    return 0; 
}
