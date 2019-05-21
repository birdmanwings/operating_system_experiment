#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define PN 3   // 消费者数目
#define CN 4   // 生产者数目
#define M 10   // 缓冲数目

int in = 0;   // 生产者放置产品的位置
int out = 0;  // 消费者取产品的位置

char *buff[M]; // 缓冲初始化为0， 开始时没有产品

sem_t empty; // 同步信号量， 当满了时阻止生产者放产品
sem_t full;   // 同步信号量， 当没产品时阻止消费者消费
pthread_mutex_t mutex; // 互斥信号量， 一次只有一个线程访问缓冲

/* 生产者方法 */ 
void *product(){
   FILE *fp=NULL;
   char item[255];

   fp=fopen("test.txt","r"); //从文件中读取数据
   fscanf(fp, "%s", item);
   while(1){
      // 用sleep的数量可以调节生产和消费的速度，便于观察
      sleep(1);

      sem_wait(&empty);
      pthread_mutex_lock(&mutex);
     
      in = in % M;
      buff[in] = item;  
      ++in;
      printf("product an apple\n"); 
      pthread_mutex_unlock(&mutex);
      sem_post(&full);  
   }
}

/* 消费者方法 */
void *consumer(){
   while(1){
      // 用sleep的数量可以调节生产和消费的速度，便于观察
      sleep(1);
   //sleep(1);
     
      sem_wait(&full);
      pthread_mutex_lock(&mutex);
     
      out = out % M;
      printf("%s\n", buff[out]);
      buff[out] = '\0';
      ++out;

      pthread_mutex_unlock(&mutex);
      sem_post(&empty);
   }
}

int main(){
   pthread_t id1[PN];
   pthread_t id2[CN];
   int i;
   int ret1[PN];
   int ret2[CN];

   // 初始化同步信号量
   int ini1 = sem_init(&empty, 0, M); 
   int ini2 = sem_init(&full, 0, 0);  
   if(ini1 && ini2 != 0){
      printf("sem init failed \n");
      exit(1);
   } 
   //初始化互斥信号量 
   int ini3 = pthread_mutex_init(&mutex, NULL);
   if(ini3 != 0){
      printf("mutex init failed \n");
      exit(1);
   } 
   // 创建PN个生产者线程
   for(i = 0; i < PN; i++){
      ret1[i] = pthread_create(&id1[i], NULL, product, (void *)(&i));
      if(ret1[i] != 0){
       printf("product%d creation failed \n", i);
       exit(1);
      }
   }
   //创建CN个消费者线程
   for(i = 0; i < CN; i++){
      ret2[i] = pthread_create(&id2[i], NULL, consumer, NULL);
      if(ret2[i] != 0){
       printf("consumer%d creation failed \n", i);
       exit(1);
      }
   }

   for(i = 0; i < PN; i++){
      pthread_join(id1[i],NULL);
   }

   for(i=0;i<CN;i++){
      pthread_join(id2[i],NULL);
   }

   exit(0); 
}