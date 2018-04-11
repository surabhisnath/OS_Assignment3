#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

sem_t mutex[40], writeblock[40];
sem_t writer_check;
int data[40];
//memset(data, 0 , 40*sizeof(data[0]));
int read_count[40];
//memset(read_count, 0, 40*sizeof(read_count[0]));


void *reader(void *arg)
{
  int f;
  f = ((int)arg);
  int index=(rand())%40;
  sem_wait(&mutex[index]);
  read_count[index]++;
  
  if(read_count[index]==1)
  {
    sem_wait(&writeblock[index]);
  }

  sem_post(&mutex[index]);
  
  printf("Data read by the reader%d is %d at index %d\n",f,data[index],index);
  



  sleep(1);
  
  sem_wait(&mutex[index]);
  
  read_count[index]--;
  
  if(read_count[index]==0)
  {
    sem_post(&writeblock[index]);
  }

  sem_post(&mutex[index]);
}


void *writer(void *arg)
{

  sem_wait(&writer_check);
  int f;
  f = ((int) arg);
  int index=(rand())%40;
  int num=(rand()+10)%100;
  sem_wait(&writeblock[index]);
  data[index]=num;
  printf("Data writen by the writer%d is %d at index %d\n",f,data[index],index);
  
  sleep(1);
  sem_post(&writeblock[index]);
  sem_post(&writer_check);
}



int main()
{
  int i,b; 
  pthread_t rtid[20],wtid[20];
  memset(data, 0 , 40*sizeof(data[0]));
  memset(read_count, 0, 40*sizeof(read_count[0]));
  sem_init(&writer_check, 0, 1);

  for(int j=0; j<40; j++)
  {

    sem_init(&mutex[j],0,1);
    sem_init(&writeblock[j],0,1);
 
  }

  for(i=0;i<20;i++)
  {
    pthread_create(&wtid[i],NULL,writer,(void *)i);
    sleep(1);
    pthread_create(&rtid[i],NULL,reader,(void *)i);
    //sleep(1);
  }


  for(i=0;i<20;i++)
  {
    pthread_join(wtid[i],NULL);
    pthread_join(rtid[i],NULL);
  }

  return 0;
}