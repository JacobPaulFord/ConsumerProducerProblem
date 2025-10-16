#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

using std::cout; using std::endl;

int main(){
    std::this_thread::sleep_for(std::chrono::seconds(2));
    sem_t *my_semaphore = sem_open("/betweenProcess", 0);
    if(my_semaphore == SEM_FAILED){
        cout << "file 2 sem failed" << endl;
        return 1;
    }

    sem_wait(my_semaphore);

    cout << "in file 2 start" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << "In file 2 end" << endl;

    sem_post(my_semaphore);
    
    sem_close(my_semaphore);

    return 0;

}