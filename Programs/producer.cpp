#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/mman.h>
#include <unistd.h>
#include <string>

using std::cout; using std::endl; using std::string;

int const maxBufferHold = 2;

int main(){
    //creating semaphore
    sem_t *my_semaphore = sem_open("/betweenProcess", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if(my_semaphore == SEM_FAILED){
        cout << "file 1 sem failed" << endl;
        return 1;
    }

    //creating shared memory
    char getSize[2];
    int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1){
        cout << "share memory not opened properly producer" << endl;
        return 1;
    }

    if( ftruncate(shm_fd, sizeof(getSize)) == -1){
        cout << "share memory size not created properly producer" << endl;
    }

    void* sharedArray = mmap(NULL, sizeof(getSize), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(sharedArray == MAP_FAILED){
        cout << "share memory not properly created producer" << endl;
    }

    char *addChar = static_cast<char*>(sharedArray);  //pointer to shared memory

    //creating shared memory to data total
    int getIntSize;
    int shm_fd2 = shm_open("/my_shared_counter", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd2 == -1){
        cout << "share counter not opened properly peroducer" << endl;
        return 1;
    }

    if( ftruncate(shm_fd2, sizeof(getIntSize)) == -1){
        cout << "share memory size not created properly producer" << endl;
    }

    void* sharedCounter = mmap(NULL, sizeof(getIntSize), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    if(sharedCounter == MAP_FAILED){
        cout << "share counter not properly created peroducer" << endl;
    }

    int *getCounter = static_cast<int*>(sharedCounter);  //pointer to shared counter
 
    //producing information

    string toProduce = "This is the string to produce";

    for (int i = 0; i < toProduce.size(); ++i){
        sem_wait(my_semaphore);
            int counter = *getCounter;
        sem_post(my_semaphore);

        while(counter == maxBufferHold) {
            cout  << "Full" << endl; std::this_thread::sleep_for(std::chrono::seconds(2));

            sem_wait(my_semaphore);
            counter = *getCounter;
            sem_post(my_semaphore);
        }


        sem_wait(my_semaphore);
            addChar[(i % 2)] = toProduce[i];
            *getCounter += 1;
        sem_post(my_semaphore);
    }


    //send termination character
    sem_wait(my_semaphore);
            int counter = *getCounter;
    sem_post(my_semaphore);

    while(counter == maxBufferHold) {
            cout  << "Full" << endl; std::this_thread::sleep_for(std::chrono::seconds(2));

            sem_wait(my_semaphore);
            counter = *getCounter;
            sem_post(my_semaphore);
    }

    sem_wait(my_semaphore);
        addChar[(toProduce.size()%2)] = '\0';
        cout << "done sending" << endl;
        *getCounter += 1;
    sem_post(my_semaphore);

    //waits for signal that consumer is done, then lets consumer close connections before closing shared memory
    sem_wait(my_semaphore);
            counter = *getCounter;
    sem_post(my_semaphore);

    while(counter != -1) {
            std::this_thread::sleep_for(std::chrono::seconds(2));

            sem_wait(my_semaphore);
            counter = *getCounter;
            sem_post(my_semaphore);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //closing and unlinking semaphore 

    sem_close(my_semaphore);
    sem_unlink("/betweenProcess");

    //closing shared memory
    if (munmap(sharedArray, sizeof(getSize)) == -1) {
        cout  << "Shared memory not closed properly 1 producer" << endl;
    }
    if (munmap(sharedCounter, sizeof(getIntSize)) == -1) {
        cout  << "Shared memory not closed properly 2 producer" << endl;
    }
    if (close(shm_fd) == -1) {
        cout  << "Shared memory not closed properly 3 producer" << endl;
    }
    if (close(shm_fd2) == -1) {
        cout  << "Shared memory not closed properly 4 producer" << endl;
    }

    //unlinking shared memory

    if(shm_unlink("/my_shared_memory") == -1){
        cout  << "Shared memory not unlinked properly 1 producer" << endl;
    }
    if(shm_unlink("/my_shared_counter") == -1){
        cout  << "Shared memory not unlinked properly 1 producer" << endl;
    }

    //telling user processes are done
    cout << "processes are finished " << endl;

    return 0;

}