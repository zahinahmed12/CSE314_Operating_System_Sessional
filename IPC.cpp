#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>
#include<stdlib.h>
#include<unistd.h>

using namespace std;

#define number_of_cycles 10
#define number_of_servicemen 3
#define number_of_counters 2


int paid = 0, taking_service=0, x=0;
sem_t next_cycle, force, pay, depart;
sem_t arr_of_sem[number_of_servicemen];
pthread_mutex_t mutex, leave;


void trying_to_sleep()
{
    usleep(((rand()%7)+1)*1000);
}

void* go_to_servicing(void* arg){

    //pthread_mutex_lock(&mutex);
    //pthread_mutex_unlock(&mutex);
    sem_wait(&force);
    sem_wait(&next_cycle);
    sem_wait(&arr_of_sem[0]);

    sem_post(&next_cycle);

    sem_wait(&depart);
    taking_service++;
    if(taking_service==1)
    {
        pthread_mutex_lock(&leave);
    }
    sem_post(&depart);

    for(int i=0;i<number_of_servicemen;i++)
    {
        printf("%s started taking service from serviceman %d\n",(char*)arg,i+1);
        //sleep(3);
        trying_to_sleep();
        printf("%s finished taking service from serviceman %d\n",(char*)arg,i+1);

        if(i+1<number_of_servicemen)
        {
            sem_wait(&arr_of_sem[i+1]);
        }
        sem_post(&arr_of_sem[i]);

        if(i+1==number_of_servicemen)
        {
            sem_wait(&depart);
            taking_service--;
            if(taking_service==0)
            {
                pthread_mutex_unlock(&leave);
            }
            sem_post(&depart);
            //sem_post(&force);
        }
    }
    sem_post(&force);

    //printf("%d ta ase\n\n",taking_service);

    sem_wait(&pay);
    printf("%s started paying the service bill\n",(char*)arg);

    sem_wait(&next_cycle);
    pthread_mutex_lock(&mutex);
    ++paid;
    if(paid==1)
    {
        sem_wait(&arr_of_sem[0]);
    }
    pthread_mutex_unlock(&mutex);
    //sleep(2);
    trying_to_sleep();
    printf("%s finished paying the service bill\n",(char*)arg);

    sem_post(&next_cycle);
    sem_post(&pay);

    pthread_mutex_lock(&leave);
    printf("%s has departed\n",(char*)arg);
    pthread_mutex_unlock(&leave);

    pthread_mutex_lock(&mutex);
    paid--;
    if(paid==0)
    {
        sem_post(&arr_of_sem[0]);
    }
    pthread_mutex_unlock(&mutex);

   // pthread_exit((void*)strcat((char*)arg\n"));

}


int main(int argc, char* argv[])
{
    srand(time(0));
    int res;

    /*if(number_of_cycles>number_of_servicemen)
    {
        x=number_of_servicemen;
    }
    else
    {
        x=3;
    }*/

    for(int i=0;i<number_of_servicemen;i++)
    {
        int x;
        x=sem_init(&arr_of_sem[i],0,1);
        if(x !=0)
        {
            printf("Failed\n");
        }
    }

    res = sem_init(&pay,0,number_of_counters);
    if(res != 0){
        printf("Failed\n");
    }
    res = sem_init(&next_cycle,0,1);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_init(&depart,0,1);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_init(&force,0,number_of_servicemen);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&mutex,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&leave,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    pthread_t cycles[number_of_cycles];
    for(int i = 0; i < number_of_cycles; i++){
        char *id = new char[4];
        strcpy(id, to_string(i+1).c_str());

        res = pthread_create(&cycles[i],NULL,go_to_servicing,(void *)id);

        if(res != 0){
            printf("Thread creation failed\n");
        }
    }

    for(int i = 0; i < number_of_cycles; i++){
        void *result;
        pthread_join(cycles[i],&result);
        //printf("%s",(char*)result);
    }

    for(int i=0;i<number_of_servicemen;i++)
    {
        int x;
        x=sem_destroy(&arr_of_sem[i]);
        if(x !=0)
        {
            printf("Failed\n");
        }
    }

    res = sem_destroy(&pay);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&next_cycle);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&depart);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&force);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&mutex);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&leave);
    if(res != 0){
        printf("Failed\n");
    }

    return 0;
}
