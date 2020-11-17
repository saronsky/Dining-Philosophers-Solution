/*
 * Simon Aronsky
 * 11 / 16 / 2020
 * Program 3C
 * Dining Philosophers
 *
 * This program uses a variety of different methods to prevent neighboring threads (philos) from eating at the same time
 */

#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <unistd.h>   // sleep( )
#include <sys/time.h> // gettimeofday
#include <iostream>   // cout

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;


/*
 * This class uses conditional mutex's (monitor) to ensure every philosopher is fed when they are able to
 */
class Table2 {
public:
    Table2() {
        // initialize by yourself
        pthread_mutex_init(&lock, NULL);
        for (int i = 0; i < PHILOSOPHERS; i++) {
            pthread_cond_init(&self[i], NULL);
            state[i] = THINKING;
        }
    }

    void pickup(int i) {
        // implement by yourself by referring to the textbook.
        pthread_mutex_lock(&lock);
        state[i] = HUNGRY;   //philosopher must be hungry to enable pickup
        test(i); //checks to see if philo able to eat yet
        if (state[i] != EATING) { //if unable to eat, wait for singal
            pthread_cond_wait(&self[i], &lock);
        }
        pthread_mutex_unlock(&lock);
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown(int i) {
        // implement by yourself by referring to the textbook
        pthread_mutex_lock(&lock);
        state[i] = THINKING;
        //after eating it checks the philos neighbors for a possible wake up
        test((i - 1) % PHILOSOPHERS);
        test((i + 1) % PHILOSOPHERS);
        pthread_mutex_unlock(&lock);
        cout << "philosopher[" << i << "] put down chopsticks" << endl;
    }

private:
    enum {
        THINKING, HUNGRY, EATING
    } state[PHILOSOPHERS];
    pthread_mutex_t lock;
    pthread_cond_t self[PHILOSOPHERS];

    void test(int i) {
        // implement by yourself
        //checks to ensure current philo is hungry, and neighbors arent eating
        if (state[(i - 1) % PHILOSOPHERS] != EATING && state[i] == HUNGRY && state[(i + 1) % PHILOSOPHERS] != EATING) {
            //if able to eat, change state and wake up philo
            state[i] = EATING;
            pthread_cond_signal(&self[i]);
        }
    }
};

/*
 * This class uses simple mutex's to lock the entire table when a philo is eating
 */
class Table1 {
public:
    Table1() {
        // initialize the mutex lock
        pthread_mutex_init(&lock, NULL);
    }

    void pickup(int i) {
        // lock by yourself
        pthread_mutex_lock(&lock); //locks the entire table upon pickup
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown(int i) {
        cout << "philosopher[" << i << "] put down chopsticks" << endl;
        // unlock by yourself
        pthread_mutex_unlock(&lock); //unlocks the entire table upon putdown
    }

private:
    // define a mutex lock
    pthread_mutex_t lock;
};

class Table0 {
public:
    void pickup(int i) {
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown(int i) {
        cout << "philosopher[" << i << "] put down chopsticks" << endl;
    }
};

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void *philosopher(void *arg) {
    int id = *(int *) arg;

    for (int i = 0; i < MEALS; i++) {
        switch (table_id) {
            case 0:
                table0.pickup(id);
                sleep(1);
                table0.putdown(id);
                break;
            case 1:
                table1.pickup(id);
                sleep(1);
                table1.putdown(id);
                break;
            case 2:
                table2.pickup(id);
                sleep(1);
                table2.putdown(id);
                break;
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t threads[PHILOSOPHERS];
    pthread_attr_t attr;
    int id[PHILOSOPHERS];
    table_id = atoi(argv[1]);

    pthread_attr_init(&attr);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < PHILOSOPHERS; i++) {
        id[i] = i;
        pthread_create(&threads[i], &attr, philosopher, (void *) &id[i]);
    }

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_join(threads[i], NULL);
    gettimeofday(&end_time, NULL);

    sleep(1);
    cout << "time = " << (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec)
         << endl;

    return 0;
}
