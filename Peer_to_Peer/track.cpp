#include <bits/stdc++.h>
#include <unistd.h> 
#include <stdio.h> 
#include <cmath>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <pthread.h>
#include <openssl/sha.h>
#include "info.h"
#define CHUNK_SIZE 512
#define PORT 8000
using namespace std;
vector<string> fl_chunk;
unordered_map<string,FILE_INFO> file_detail;
unordered_map<string,vector<int>> grp_detail;
unordered_map<int, int> umap;
unordered_map<string,vector<int>> pend_req;
unordered_map<string,vector<string>> grp_file;
unordered_map<string,vector<int>> seeder_list;
vector<vector<string>> up_ls;
vector<string> user;
PEER peer[10];
int no_peer=0;

void seperate(char c[],vector<string> &v)
{
	string k="";
	for(int i=0;i<strlen(c);i++)
	{
		if(c[i]==' ')
		{
			v.push_back(k);
			k="";
		}
		else
		{
			k=k+c[i];
		}
	}
	v.push_back(k);
}

string file_sha(string fl_nm)
{
	FILE *fp;
	int i;
	fp=fopen(fl_nm.c_str(),"rb");
	if(fp==NULL)
	{
		perror("Unable to open file\n");
	}
	char buf[CHUNK_SIZE];
	char out[70];
	size_t sz;
	SHA_CTX md;
	
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Init(&md);
	int blk=0;
	string str="";
	fl_chunk.clear();
	while(!feof(fp))
	{
		if((sz=fread((void*)buf,1,CHUNK_SIZE,fp))>0) 
		{
			string c_sha="";
			SHA1_Init(&md);
			SHA1_Update(&md,buf,sz);	
			SHA1_Final(hash,&md);
			blk++;
			for(i = 0; i < SHA_DIGEST_LENGTH; i++)
		    	{
				sprintf(out + (i * 2), "%02x", hash[i]);
		    	}
		    	for(i=0;i<20;i++)
		    	{
		    		c_sha+=out[i];
		    		str+=out[i];
		    	}
		    	fl_chunk.push_back(c_sha);
		    	memset(out,0,sizeof(out));
			memset(hash,0,sizeof(hash));
		}
	}
	cout<<"\nfile sha:";
	cout<<str<<"\n";
	return str;	
}

void* communicate(void* sckt)
{
	int newsock=*((int *)sckt);
	//cout<<newsock;
	while(1)
	{
		int valread;
		vector<string> temp; 
		char buffer[1024];
		bzero(buffer,1024);
		//cout<<peer[umap[newsock]].port<<"\n";
		valread = read(newsock, buffer, 1024);
		seperate(buffer,temp);
		cout<<"cmd:"<<buffer<<"\n";
		if(temp[0]=="create_user")
		{
			string msg="User created!!\n";
			peer[umap[newsock]].pass=temp[2];
			peer[umap[newsock]].username=temp[1];
			user.push_back(temp[1]);
			fflush(stdout);
			send(newsock,msg.c_str(),msg.size(),0);
			temp.clear();			
		}
		else if(temp[0]=="login")
		{
			/*for(auto a:user)
				cout<<a<<" ";
			cout<<"\n";*/
			string msg1="Logged in!!\n";
			string msg2="Inavlid password!!\n";
			cout<<peer[umap[newsock]].username<<"->"<<peer[umap[newsock]].pass<<"\n";
			//if(find(user.begin(),user.end(),temp[1])!=user.end())
			//	cout<<"found"<<temp[2]<<"\n";
			if(peer[umap[newsock]].pass==temp[2] && find(user.begin(),user.end(),temp[1])!=user.end())
			{
				int val;
				string ipser="",pnser="";
				char buffer1[1024];
				bzero(buffer1,1024);
				fflush(stdout);
				peer[umap[newsock]].login=1;
				peer[umap[newsock]].server_ip=temp[3];
				peer[umap[newsock]].server_port=stoi(temp[4]);
				send(newsock,msg1.c_str(),msg1.size(),0);
				fflush(stdout);
			}
			else
			{
				fflush(stdout);
				send(newsock,msg2.c_str(),msg2.size(),0);
				fflush(stdout);
			}
			temp.clear();	
		}
		else if(temp[0]=="create_group")
		{
			
			if(peer[umap[newsock]].login==1)
			{
				
				string msg1="Group created!!\n";
				vector<int> mem;
				mem.push_back(newsock);
				//cout<<"1"<<temp[1];
				grp_detail.insert({temp[1],mem});
				(peer[umap[newsock]].gid).push_back(temp[1]);
				fflush(stdout);
				send(newsock,msg1.c_str(),msg1.size(),0);
				fflush(stdout);
			}
			temp.clear();
		}
		else if(temp[0]=="join_group")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
			string msg1="Group doesn't exist!!\n";
			string msg2="Pending group exist!!\n";
			string msg3="Already member!!\n";
			if(grp_detail.find(temp[1])!=grp_detail.end())
			{
				if(find((grp_detail[temp[1]]).begin(),(grp_detail[temp[1]]).end(),newsock)==(grp_detail[temp[1]]).end())
				{
				
						string msg4="Group join request made!!\n";
						if(pend_req.find(temp[1])==pend_req.end())
						{
							vector<int> pnd;
							pnd.push_back(newsock);
							pend_req.insert({temp[1],pnd});
						}
						else
							pend_req[temp[1]].push_back(newsock);
						
						fflush(stdout);
						send(newsock,msg4.c_str(),msg4.size(),0);
				}
				else
				{
					fflush(stdout);
					send(newsock,msg3.c_str(),msg3.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg1.c_str(),msg1.size(),0);
			}
			}
			else
			{	
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="leave_group")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Group doesn't exist!!\n";
				if(grp_detail.find(temp[1])!=grp_detail.end())
				{
				string msg3="Not a member!!\n";
				auto it=find((grp_detail[temp[1]]).begin(),(grp_detail[temp[1]]).end(),newsock);
				if(it!=(grp_detail[temp[1]]).end())
				{
					string msg4="Admin can't leave!!\n";
					if((grp_detail[temp[1]])[0]!=newsock)
					{
							string msg2="Left!!\n";
							grp_detail[temp[1]].erase(it);
							fflush(stdout);
							send(newsock,msg2.c_str(),msg2.size(),0);
					}
					else
					{
						fflush(stdout);
						send(newsock,msg4.c_str(),msg4.size(),0);
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg3.c_str(),msg3.size(),0);
				}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="list_requests")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="No request for this group!!\n";
				if(pend_req.find(temp[1])!=pend_req.end())
				{
					if(pend_req[temp[1]].size()>0)
					{
						string k="";
						for(auto a:(pend_req[temp[1]]))
						{
							k=k+peer[umap[a]].username+"\n";
						}
						fflush(stdout);
						send(newsock,k.c_str(),k.size(),0);
					}
					else
					{
						fflush(stdout);
						send(newsock,msg1.c_str(),msg1.size(),0);
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="accept_request")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Group doesn't exist!!\n";
				if(grp_detail.find(temp[1])!=grp_detail.end())
				{
					string msg2="No such request made!!\n";
					int id=-1;
					for(auto a:(pend_req[temp[1]]))
					{
						if((peer[umap[a]]).username==temp[2])
						{
							id=a;
							break;
						}	
					}
					if(id!=-1)
					{
						string msg3="Not authorize to accept!!\n";
						auto it=find((pend_req[temp[1]]).begin(),(pend_req[temp[1]]).end(),id);
						if((grp_detail[temp[1]])[0]==newsock)
						{
							string msg4="Accepted!!\n";
							grp_detail[temp[1]].push_back(id);
							pend_req[temp[1]].erase(it);
							fflush(stdout);
							send(newsock,msg4.c_str(),msg4.size(),0);
						}
						else
						{
							fflush(stdout);
							send(newsock,msg3.c_str(),msg3.size(),0);
						}
					}
					else
					{
						fflush(stdout);
						send(newsock,msg2.c_str(),msg2.size(),0);
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="list_groups")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				for(auto a:grp_detail)
				{
					string k="";
					k=k+a.first+"\n";
					fflush(stdout);
					send(newsock,k.c_str(),k.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="upload_file")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Group doesn't exist!!\n";
				if(grp_detail.find(temp[2])!=grp_detail.end())
				{
				string msg2="Only members can upload!!\n";
				if(find((grp_detail[temp[2]]).begin(),(grp_detail[temp[2]]).end(),newsock)!=(grp_detail[temp[2]]).end())
				{
							string msg3="Successfully uploaded!!\n";
							struct FILE_INFO fl;
							ifstream input(temp[1],ios::binary);
							input.seekg(0,ios::end);
							int size=input.tellg();
							input.seekg(0);
							input.close();
							int pieces;
							//float pieces=(size/CHUNK_SIZE);
							//pieces=ceil(pieces);
							fl.file_sz=size;
							cout<<"Size:"<<size;
							cout<<"\n";
							fl.filename=temp[1];
							fl.sha=file_sha(temp[1]);
							
							vector<string> fdtl;
							fdtl.push_back(temp[1]);
							fdtl.push_back(to_string(newsock));
							fdtl.push_back(temp[2]);
							up_ls.push_back(fdtl);
							
							for(auto a:fl_chunk)
							{
								pieces++;
								cout<<a<<" ";
								(fl.chunk_sha).push_back(a);
							}
							fl.no_chunk=pieces;
							cout<<"\nPieces:"<<pieces;
							cout<<"\n";	
							file_detail.insert({temp[1],fl});
							if(grp_file.find(temp[2])==grp_file.end())
							{
								vector<string> fn;
								fn.push_back(temp[1]);
								grp_file.insert({temp[2],fn});
							}
							else
							{
								grp_file[(temp[2])].push_back(temp[1]);
							}
							if(seeder_list.find(temp[1])==seeder_list.end())
							{
								vector<int> seed;
								seed.push_back(newsock);
								seeder_list.insert({temp[1],seed});
							}
							else
							{
								seeder_list[(temp[1])].push_back(newsock);
							}
							
							fl_chunk.clear();
							fflush(stdout);
							send(newsock,msg3.c_str(),msg3.size(),0);
					}
					else
					{
						fflush(stdout);
						send(newsock,msg2.c_str(),msg2.size(),0);
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="download_file")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Group doesn't exist!!\n";
				if(grp_detail.find(temp[1])!=grp_detail.end())
				{
				string msg2="Only members can download!!\n";
				if(find((grp_detail[temp[1]]).begin(),(grp_detail[temp[1]]).end(),newsock)!=(grp_detail[temp[1]]).end())
				{
					string msg3="No such file is present!!\n";
					if(file_detail.find(temp[2])!=file_detail.end())
					{
						string fldet="";
						string ser="";
						fldet=file_detail[temp[2]].sha+";";
						for(auto a:file_detail[temp[2]].chunk_sha)
						{
							cout<<a<<"\n";
							fldet=fldet+a+",";
						}
						fldet=fldet+";";
						for(auto b:seeder_list[temp[2]])
						{
						fldet=fldet+peer[umap[b]].server_ip+","+to_string(peer[umap[b]].server_port)+"|";
						}
						fflush(stdout);
						send(newsock,fldet.c_str(),fldet.size(),0);
						
					}
					else
					{
						fflush(stdout);
						send(newsock,msg3.c_str(),msg3.size(),0);
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg2.c_str(),msg2.size(),0);
				}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="show_downloads")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="No files avialbles!!\n";
				if(seeder_list.size()>0)
				{
					string k="";
					for(auto a:up_ls)
					{
						//cout<<a[1]<<":"<<newsock;
						if(stoi(a[1])==newsock)
						{
							string status="[C]";
							char slh='/';
							size_t l=a[0].find_last_of(slh);
							string fdst=a[0].substr(l+1,a[0].size());
							k=k+status+"["+a[2]+"]"+"["+fdst+"]"+"\n";
						}
						else
						{
							string status="[D]";
							char slh='/';
							size_t l=a[0].find_last_of(slh);
							string fdst=a[0].substr(l+1,a[0].size());
							k=k+status+"["+a[2]+"]"+"["+fdst+"]"+"\n";
						}
					}
					fflush(stdout);
					send(newsock,k.c_str(),k.size(),0);
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="list_files")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Group doesn't exist!!\n";
				if(grp_detail.find(temp[1])!=grp_detail.end())
				{
					for(auto a:grp_file[temp[1]])
					{
						string k="";
						k=k+a+"\n";
						fflush(stdout);
						send(newsock,k.c_str(),k.size(),0);	
					}
				}
				else
				{
					fflush(stdout);
					send(newsock,msg1.c_str(),msg1.size(),0);
				}
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
		else if(temp[0]=="logout")
		{
			string msg="Not logged in!!\n";
			if(peer[umap[newsock]].login==1)
			{
				string msg1="Logged out!!\n";
				peer[umap[newsock]].login=0;
				fflush(stdout);
				send(newsock,msg1.c_str(),msg1.size(),0);	
			}
			else
			{
				fflush(stdout);
				send(newsock,msg.c_str(),msg.size(),0);
			}
			temp.clear();
		}
	} 
}

int main(int argc,char** argv)
{
	    string inpf="";
	    string outf="";
	    string arg=argv[1];
	    ifstream input(arg,ios::binary);
	    getline(input,inpf);
	    getline(input,outf);
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
	    address.sin_port = htons(stoi(outf));
	    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
	    { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	    } 
	    if (listen(server_fd, 10) < 0) 
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
		pthread_t p1;
		peer[no_peer].ip=inet_ntoa(address.sin_addr);
		peer[no_peer].port=(int) ntohs(address.sin_port);
		peer[no_peer].skt=new_socket;
		umap[new_socket]=no_peer;
		//cout<<new_socket; 
		if(pthread_create(&p1, NULL,communicate,(void*)&new_socket)!= 0) 
		{
			cout<<"Failed to create client thread\n";
		} 
		no_peer++;
	    }
	    close(server_fd);
    	    pthread_exit(NULL);
    	    input.close();
	    return 0;
}
