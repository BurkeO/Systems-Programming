#include "option_handler.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;



#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
 
 
/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
     
public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive(int);
};
 
tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}
 
/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    cout<<"Connected\n";
    return true;
}
 
/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
string tcp_client::receive(int size=512)
{
    char buffer[size];
	memset(buffer, 0, size);
    string reply;
     
    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
     
    reply = buffer;
    return reply;
}






void usage()
{
	cout << "usage: ./knock -h host -p port [-H] [-w] [-f file]" << endl;
	exit(-1);
}

/*bool isValidBasicInput(int argc, char* argv[])
{
	string argv1 = argv[1];
	if (argv1 == "-?" || argv1 == "-H" || argv1 == "help")
	{
		::usage();
	}
	string argv2 = argv[2];
	string argv3 = argv[3];
	string argv4 = argv[4];
	if (argc < 5)
	{
		return false;
	}
	if (argv1 != "host" && argv1 != "-h")
	{
		return false;
	}
	if (argv3 != "port" && argv3 != "-p")
	{
		return false;
	}
	try
	{
		int portNumber = stoi(argv4);
		if (portNumber < 1 || portNumber > 65535)
		{
			return false;
		}
		return true;
	}
	catch (std::invalid_argument)
	{
		return false;
	}
}

bool isValidExtraInput(char* argv[])
{
	string argv5 = argv[5];
	string argv7 = argv[7];
	if (argv5 != "web" && argv5 != "-w")
	{
		return false;
	}
	if (argv7 != "file" && argv7 != "-f")
	{
		return false;
	}
	return true;
}*/

void add_options(OptionHandler::Handler & h) {

 	try 
	{
    	h.add_option('H', "help",   OptionHandler::NONE, false); 
	}
  	catch (const std::exception & e) 
	{
    	std::cerr << e.what() << std::endl; 
	}
	try 
	{
    	h.add_option('?', "help",   OptionHandler::NONE, false); 
	}
  	catch (const std::exception & e) 
	{
    	std::cerr << e.what() << std::endl; 
	}
	try 
	{
		h.add_option('h', "host",  OptionHandler::REQUIRED, false); 
	}
	catch (const std::exception & e) 
	{
		std::cerr << e.what() << std::endl; 
	}
	try
	{
		h.add_option('p', "port",  OptionHandler::REQUIRED, false); 
	}
	catch (const std::exception & e) 
	{
		std::cerr << e.what() << std::endl;
	}
	h.add_option('w', "web",   OptionHandler::OPTIONAL, false);
	h.add_option('f', "file", OptionHandler::OPTIONAL, false);
}

int main(int argc, char* argv[])
{
	OptionHandler::Handler h = OptionHandler::Handler(argc, argv);
	add_options(h);
	if (h.get_option("help") == true)
	{
		::usage();
	}
	int portNumber;
	string hostValue;
	//h.add_option('w', "web",   OptionHandler::OPTIONAL, false);
	//h.add_option('f', "file", OptionHandler::OPTIONAL, false);
	
	hostValue = h.get_argument("host");
	cout << hostValue << endl;
	string portString = h.get_argument("port");
	cout << portString << endl;
	try
	{
		portNumber= stoi(portString);
	}
	catch (std::invalid_argument)
	{
		cout << "Invalid Argument" << endl;
		exit(-1);
	}
	tcp_client client;
	client.conn(hostValue, portNumber);
	///////////////////////////////////////////////////////////////////////////
	//if (h.get_option("web") == true)
	//{
		string requestString = h.get_argument("web");
		//client.send_data("GET /" + hostValue + "HTTP/1.1\r\n\r\n");
		//client.send_data("GET /" + requestString + " HTTP/1.1\r\n\r\nHost: " + hostValue + "\r\n\r\n");
		client.send_data("GET /" + requestString + " \r\n\r\n");
		//client.send_data("GET /  HTTP/1.1\r\n\r\nHost: " + hostValue + "\r\n\r\n");
		cout << hostValue << endl;
		cout << requestString << endl;
		
	//}
	if (h.get_option("file") == true)
	{
		std::ofstream outfile(h.get_argument("file"));
		//outfile << "----------------------------\n\n" << endl;
		outfile << client.receive(1024);
		//outfile << "\n\n----------------------------\n\n" << endl;
	}
	else
	{
		cout << client.receive(1024);
	}
	return(0);
}


