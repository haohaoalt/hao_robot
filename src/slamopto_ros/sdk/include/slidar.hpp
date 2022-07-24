#ifndef _Slidar_MR_HPP_
#define _Slidar_MR_HPP_
#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <iomanip>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include "str_split.hpp"

#define MAX_DATA_LEN 3000 
#define MAX_TCP_LEN 1460
#define BUFF_LEN 2000
#define DATA_NUM 811
#define SHMNO 1
#define SEMNO 2
using namespace std;

struct Shm_dis
{
  pthread_mutexattr_t mutexattr;
  pthread_mutex_t mutex;
  float dis[DATA_NUM];
};

struct Data
{
  uint32_t head;
  uint32_t data_len;
  uint32_t data;
  uint32_t reserved;
};

class Slidar_M
{

private:
  std::string tcp_ip;
  int tcp_port;
  uint16_t ui16_tcp_port;
  struct sockaddr_in server_addr;
  int sockfd;
  Data d_4005;
  char pdis_mem[5 * DATA_NUM];

public:
  Slidar_M(std::string tcp_ip,int tcp_port);
  ~Slidar_M();
  int read_dis(float (&dis_in)[DATA_NUM],float (&intensities)[DATA_NUM]);
  int stop();
};

Slidar_M::Slidar_M(std::string tcp_ip,int tcp_port):tcp_ip(tcp_ip),tcp_port(tcp_port)
{

  this->d_4005.head = htonl(0x1234 << 16 | 0x4005);
  this->d_4005.data_len = htonl(0x4);
  this->d_4005.data = htonl(0xffffffff);
  this->d_4005.reserved = htonl(0x67);

  this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
  if (this->sockfd == -1)
  {
    printf("create socket error!\n");
    exit(EXIT_FAILURE);
  }

  bzero(&this->server_addr, sizeof(struct sockaddr_in));
  this->server_addr.sin_family = AF_INET;

  this->server_addr.sin_addr.s_addr = inet_addr(tcp_ip.c_str());
 
  this->server_addr.sin_port = htons((uint16_t)(tcp_port));


  if (connect(this->sockfd, (struct sockaddr *)(&this->server_addr), sizeof(struct sockaddr)) == -1)
  {
    printf("connect error!\n");
    exit(EXIT_FAILURE);
  }
  send(this->sockfd, &this->d_4005, sizeof(this->d_4005), 0);
}

Slidar_M::~Slidar_M()
{
  

  close(this->sockfd);
}


int Slidar_M::read_dis(float (&dis_in)[DATA_NUM],float (&intensities)[DATA_NUM])
{
  char *p_end = NULL;
  char *intensity_end = NULL;

  char buffer[BUFF_LEN];
  static int read_nbyte;
  static string all_data;
  static string tmp_data;
  static string p_data_sring;
  static string i_data_string;

  static string one_data;

  read_nbyte = 0;
  all_data.clear();
  tmp_data.clear();
  p_data_sring.clear();
  i_data_string.clear();
  one_data.clear();


  memset((void *)buffer, 0, BUFF_LEN);
  read_nbyte = recv(this->sockfd, buffer, BUFF_LEN, 0);

  if (MAX_TCP_LEN == read_nbyte && 2 == buffer[0])
  {
    
    tmp_data = buffer;
    all_data.append(tmp_data);

    for(int i=0;i<4;i++)
    {
      memset((void *)buffer, 0, BUFF_LEN);
      read_nbyte = recv(this->sockfd, buffer, BUFF_LEN, 0);
      tmp_data = buffer;
      all_data.append(tmp_data);
    }


    p_data_sring.append(all_data, 110, 4054);
    i_data_string.append(all_data,4206,2432);
    for (int i = 0; i < DATA_NUM; i++)
    {
      one_data = p_data_sring.substr(i * 5, 4);
      dis_in[i] = (float)strtoul(one_data.c_str(), &p_end, 16) / 1000.0;
      one_data = i_data_string.substr(i * 3, 2);
      intensities[i] = (float)strtoul(one_data.c_str(), &intensity_end, 16) / 1000.0;
    }
      return 1;
  }
  return 0;
}


#endif
