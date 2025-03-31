#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <condition_variable>

using namespace std;

// Semaphore acting as waiter
class Semaphore {
    public:
        Semaphore(int count = 0) : count(count) {}

        void wait(){
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] { return count > 0; });
            --count;
        }

        void signal() {
            unique_lock<mutex> lock(mtx);
            ++count;
            cv.notify_one();
        }

    private:
        mutex mtx;
        condition_variable cv;
        int count;
}; 



void philosopher(int id, mutex* forks, Semaphore& waiter, int num_philosophers) {
    while(true) {
        // Thinking phase
        cout << "Philosopher " << id << " is thinking." << endl;

        // TODO: Add a random sleep time to simulate thinking
        this_thread::sleep_for(chrono::milliseconds(1000));

        // #### Want to eat / hungry phase ####
        waiter.wait();
        cout << "Philosopher " << id << " is hungry. Asked for waiter for permission" << endl;

        // Determine left and right forks ids
        int left_fork = id;
        int right_fork = (id + 1) % num_philosophers;

        // Picking up and locking the forks
        forks[left_fork].lock();
        cout << "Philosopher " << id << " picked up left fork." << endl;
        forks[right_fork].lock();
        cout << "Philosopher " << id << " picked up right fork." << endl;

        // #### Eating phase ####
        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000)); // TODO: add a random sleep time 

        // Put down the forks after eating
        cout << "Philosopher " << id << " finished eating." << endl; 
        forks[right_fork].unlock();
        cout << "Philosopher " << id << " put down right fork." << endl;
        forks[left_fork].unlock();
        cout << "Philosopher " << id << " put down left fork." << endl;
        


        waiter.signal();

    }
}


//TODO: Get an input here instead of hardcoded value

int main(){
    const int NUM_PHILOSOPHERS = 5;
    mutex forks[NUM_PHILOSOPHERS];
    Semaphore waiter(NUM_PHILOSOPHERS - 1); // Initialize semaphore with NUM_PHILOSOPHERS - 1
    // so we prevent situation where all philosophers pick up their left fork and wait for the right one
    // The fork picking up and putting down is done in reverse order to avoid deadlock

    // Create philosopher threads
    vector<thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i, forks, ref(waiter), NUM_PHILOSOPHERS);
    }

    for (auto& philosopher : philosophers) {
        philosopher.join();
    }

    return 0;
    // main thread here
}