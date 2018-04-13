#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
int queue[50];
int num_readers[50];
int f=0;
int r=0;
int ct=1;
sem_t check_wr;
sem_t binary_sem[50];
sem_t wr[50];
//int queue[40];
//memset(queue, 0 , 40*sizeof(queue[0]));
//int num_readers[40];
//memset(num_readers, 0, 40*sizeof(num_readers[0]))
int get_front(){
  return f;
}
int get_rear(){
  return r;
}
void enqueue(){
  if (r==49)printf("Queue is full");
  else{
    int num=(rand()+20)%200;
    r++;
    queue[r]=num;
    printf("Enqueued element %d at index %d",num,r);
  }
}
void dequeue(){
  if (r-f==0)printf("Queue is empty");
  else{
      int num=queue[f];
      f++;
      printf("Dequeued element %d from index %d",num,f);
  }
}

void *reader(void *arg)
{
  int temp=((int)arg);
  int index=(rand())%50;
  sem_wait(&binary_sem[index]);
  num_readers[index]++;
  
  if(num_readers[index]==1)
  {
    sem_wait(&wr[index]);
  }

  sem_post(&binary_sem[index]);
  if (ct==1){
    dequeue();
    ct=0;
  }else{
    printf("queue read by the reader%d is %d at index %d\n",f,queue[index],index);
  }
  sleep(1);
  sem_wait(&binary_sem[index]);
  num_readers[index]--;
  if(num_readers[index]==0)
  {
    sem_post(&wr[index]);
  }
  sem_post(&binary_sem[index]);
}
void *writer(void *arg)
{
  int num=(rand()+20)%200;
  int index=(rand())%50;
  sem_wait(&check_wr);
  int temp=((int)arg);
  sem_wait(&wr[index]);
  queue[index]=num;
  //printf("queue writen by the writer%d is %d at index %d\n",f,queue[index],index);
  enqueue();
  sleep(1);
  sem_post(&wr[index]);
  sem_post(&check_wr);
}



int main()
{
  memset(queue, 0 , 40*sizeof(queue[0]));
  memset(num_readers, 0, 40*sizeof(num_readers[0]));
  int b; 
  pthread_t rtid[25];
  pthread_t wtid[25];
  sem_init(&check_wr, 0, 1);
  for(int j=0; j<50; j++)
  {
    sem_init(&binary_sem[j],0,1);
    sem_init(&wr[j],0,1);
  }

  for(int i=0;i<25;i++)
  {
    pthread_create(&wtid[i],NULL,writer,(void *)i);
    sleep(1);
    pthread_create(&rtid[i],NULL,reader,(void *)i);
    //sleep(1);
  }


  for(int i=0;i<25;i++)
  {
    pthread_join(wtid[i],NULL);
    pthread_join(rtid[i],NULL);
  }

  return 0;
}
