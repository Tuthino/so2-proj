#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <queue>
#include <condition_variable>

using namespace std;

// Semaphore acting as waiter
class Semaphore {
    public:
        Semaphore(size_t count = 0) : count(count),next_ticket(0), now_serving(0) {}

        void wait(){
            unique_lock<mutex> lock(mtx);
            size_t ticket = next_ticket++;
            waiting_queue.push(ticket);
            cv.wait(lock, [this,ticket] { return count > 0 && waiting_queue.front() == ticket; });
            waiting_queue.pop();
            --count;
        }

        void signal() {
            unique_lock<mutex> lock(mtx);
            ++count;
            cv.notify_all();
        }

    private:
        mutex mtx;
        condition_variable cv;
        size_t count, next_ticket, now_serving;
        queue<size_t> waiting_queue;
}; 

int random_duration(int min_ms, int max_ms) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min_ms, max_ms);
    return dist(gen);
}


void philosopher(int id, mutex* forks, Semaphore& waiter, int num_philosophers, mutex& print_mtx) {
    while(true) {
        // Thinking phase
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " is thinking." << endl;
        }

        // TODO: Add a random sleep time to simulate thinking
        this_thread::sleep_for(chrono::milliseconds(random_duration(500,2000))); 

        // #### Want to eat / hungry phase ####
        waiter.wait();
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " is hungry. Asked for waiter for permission" << endl;
        }

        // Determine left and right forks ids
        int left_fork = id;
        int right_fork = (id + 1) % num_philosophers;

        // Picking up and locking the forks
        forks[left_fork].lock();
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " picked up left fork." << endl;
        }
        forks[right_fork].lock();
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " picked up right fork." << endl;
        }

        // #### Eating phase ####
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " is eating." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(random_duration(500,2000))); 

        // Put down the forks after eating
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " finished eating." << endl; 
        }
        forks[right_fork].unlock();
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " put down right fork." << endl;
        }
        forks[left_fork].unlock();
        {
            lock_guard<mutex> lock(print_mtx);
            cout << "Philosopher " << id << " put down left fork." << endl;
        }
        
        waiter.signal();

    }
}


//TODO: Get an input here instead of hardcoded value

int main(){
    const size_t NUM_PHILOSOPHERS = 5;
    mutex forks[NUM_PHILOSOPHERS];
    Semaphore waiter(NUM_PHILOSOPHERS - 1); // Initialize semaphore with NUM_PHILOSOPHERS - 1
    // so we prevent situation where all philosophers pick up their left fork and wait for the right one
    // The fork picking up and putting down is done in reverse order to avoid deadlock

    mutex print_mtx; // print mutex to synchronize the output

    // Create philosopher threads
    vector<thread> philosophers;
    for (size_t i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i, forks, ref(waiter), NUM_PHILOSOPHERS, ref(print_mtx));
    }

    for (auto& philosopher : philosophers) {
        philosopher.join();
    }
    // Because the philosophers are in an infinite loop, we never actually reach here, but .join() prevents
    // the main thread from exiting before the philosopher threads
    return 0;   
}