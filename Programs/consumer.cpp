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

    //waiting for producer to create semaphore and shared memory
    std::this_thread::sleep_for(std::chrono::seconds(3));

    //opening semaphore
    sem_t *my_semaphore = sem_open("/betweenProcess", 0);
    if(my_semaphore == SEM_FAILED){
        cout << "file 2 sem failed" << endl;
        return 1;
    }

    //opening shared memory array
    char getSize[2];
    int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1){
        cout << "share memory not opened properly counsumer" << endl;
        return 1;
    }

    void* sharedArray = mmap(NULL, sizeof(getSize), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(sharedArray == MAP_FAILED){
        cout << "share memory not properly created counsumer" << endl;
    }

    char *getChar = static_cast<char*>(sharedArray);  //pointer to shared memory

    //opening shared memory to data total
    int getIntSize;
    int shm_fd2 = shm_open("/my_shared_counter", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd2 == -1){
        cout << "share counter not opened properly counsumer" << endl;
        return 1;
    }

    void* sharedCounter = mmap(NULL, sizeof(getIntSize), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    if(sharedCounter == MAP_FAILED){
        cout << "share counter not properly created counsumer" << endl;
    }

    int *getCounter = static_cast<int*>(sharedCounter);  //pointer to shared counter

    //getting information
    int i = 0;
    string result;

    while (true){
        sem_wait(my_semaphore);
            int counter = *getCounter;
        sem_post(my_semaphore);

        while(counter == 0) {
            cout  << "empty" << endl; std::this_thread::sleep_for(std::chrono::seconds(2));

            sem_wait(my_semaphore);
            counter = *getCounter;
            sem_post(my_semaphore);
        }

        sem_wait(my_semaphore);

        if(getChar[(i%2)] == '\0'){  //when it gets this character, message is done, and will send signal to close connections
            cout << result << endl;
            *getCounter = -1;
            sem_post(my_semaphore);
            break;
        }

        result += getChar[(i%2)];
        ++i;
        *getCounter += -1;

        sem_post(my_semaphore);
    }


    //closing semaphore
    sem_close(my_semaphore);

    //closing shared memory
    if (munmap(sharedArray, sizeof(getSize)) == -1) {
        cout  << "Shared memory not closed properly 1 consumer" << endl;
    }
    if (munmap(sharedCounter, sizeof(getIntSize)) == -1) {
        cout  << "Shared memory not closed properly 2 consumer" << endl;
    }
    if (close(shm_fd) == -1) {
        cout  << "Shared memory not closed properly 3 consumer" << endl;
    }
    if (close(shm_fd2) == -1) {
        cout  << "Shared memory not closed properly 4 consumer" << endl;
    }

    return 0;

}