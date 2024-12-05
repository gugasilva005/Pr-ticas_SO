#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <random>
#include <ctime>
#include <semaphore.h>

using namespace std;

#define N 5  
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define RIGHT (phnum + 1) % 5
#define LEFT (phnum + 4) % 5

int phil[N];
int times = 200;

// Mutex para sincronizar as mensagens no terminal
pthread_mutex_t print_mutex;

class Monitor {
    int state[N];
    sem_t mutex;
    sem_t s[N];

    public:
        Monitor() {
            sem_init(&mutex, 0, 1);
            for (int i = 0; i < N; i++) {
                sem_init(&s[i], 0, 0);
                state[i] = THINKING;
            }
        }
        ~Monitor() {
            sem_destroy(&mutex);
            for(int i = 0; i < N; i++) sem_destroy(&s[i]);
        }

        void test(int phnum) {
            if (state[phnum] == HUNGRY and state[LEFT] != EATING and state[RIGHT] != EATING) {
                state[phnum] = EATING;
                pthread_mutex_lock(&print_mutex);
                cout << "DEBUG: Filosofo " << phnum + 1 << " pode comer" << endl;
                pthread_mutex_unlock(&print_mutex);
                sem_post(&s[phnum]);
            }
        }
        
        void takeFork(int phnum) {
            sem_wait(&mutex)        ;
            
            state[phnum] = HUNGRY;

            // Sincronizacao para mensagens
            pthread_mutex_lock(&print_mutex);
            cout << "Filosofo " << phnum + 1 << " esta com fome" << endl;
            pthread_mutex_unlock(&print_mutex);

            test(phnum);

            sem_post(&mutex);
            cout << "DEBUG: Liberando semaforo para filosofo " << phnum + 1 << endl;
            sem_wait(&s[phnum]);

            state[phnum] = EATING;

            pthread_mutex_lock(&print_mutex);
            cout << "Filosofo " << phnum + 1 << " pegou os garfos e esta comendo" << endl;
            pthread_mutex_unlock(&print_mutex);

            sleep(1);
        }

        void putFork(int phnum) {
            sem_wait(&mutex);

            state[phnum] = THINKING;

            pthread_mutex_lock(&print_mutex);
            cout << "Filosofo " << phnum + 1 << " soltou os garfos e esta pensando" << endl;
            pthread_mutex_unlock(&print_mutex);

            test(RIGHT);
            test(LEFT);

            cout << "DEBUG: Estado atual dos Filosofos: ";
            for (int i = 0; i < N; i++) cout << state[i] << " ";
            cout << endl;

            sem_post(&mutex);
            cout << "DEBUG: Liberando semaforo para filosofo " << phnum + 1 << endl;

            sleep(1);
        }
};

Monitor phil_object;

double random_wait() {
    static thread_local mt19937 generator(time(nullptr));
    uniform_real_distribution<double> distribution(0.5, 1.0);
    return distribution(generator);
}

void *philosopher(void *arg) {
    int c = 0;

    while(c < times) {
        int j = *(int*)arg;

        sleep(random_wait());

        phil_object.takeFork(j);

        sleep(random_wait());

        phil_object.putFork(j);

        c++;
    }
    
    return nullptr;
}

int main() {
    pthread_t thread_id[N];
    pthread_attr_t attr;

    pthread_mutex_init(&print_mutex, nullptr);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < N; i++) {
        phil[i] = i;
    }

    for (int j = 0; j < N; j++) {
        pthread_create(&thread_id[j], &attr, philosopher, &phil[j]);

        pthread_mutex_lock(&print_mutex);
        cout << "Filosofo " << j + 1 << " esta pensando" << endl;
        pthread_mutex_unlock(&print_mutex);

        sleep(1);
    }

    for (int k = 0; k < N; k++) {
        pthread_join(thread_id[k], nullptr);
    }

    pthread_attr_destroy(&attr);

    pthread_mutex_destroy(&print_mutex);

    pthread_exit(nullptr);

    return 0;
}