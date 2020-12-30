#include <bits/stdc++.h>
using namespace std;
int tracker=1;
struct PEER
{
	int port;
	string server_ip;
	int server_port;
	string username;
	string pass;
	string ip;
	int skt;
	int login;
	vector<string> gid;
};
typedef struct PEER PEER;

struct FILE_INFO
{
	string filename;
	int file_sz;
	string sha;
	int no_chunk;
	vector<string> chunk_sha;
};
typedef struct FILE_INFO FILE_INFO;
