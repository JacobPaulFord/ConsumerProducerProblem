#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

using std::cout; using std::endl;

int main(){
    sem_t *my_semaphore = sem_open("/betweenProcess", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if(my_semaphore == SEM_FAILED){
        cout << "file 1 sem failed" << endl;
        return 1;
    }

    sem_wait(my_semaphore);

    cout << "in file 1 start" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << "In file 1 end" << endl;

    sem_post(my_semaphore);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    sem_wait(my_semaphore);

    cout << "in file 1 start" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    cout << "In file 1 end" << endl;

    sem_post(my_semaphore);

    sem_close(my_semaphore);
    sem_unlink("/betweenProcess");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;

}