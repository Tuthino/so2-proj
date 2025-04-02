# so2-proj
### Author
Marek Tutka 276314

### Problem overview
Dining philosophers problem.
Wiki page: https://en.wikipedia.org/wiki/Dining_philosophers_problem

In original problem philosphers have 2 possible states:
- Thinking
- Eating
However the philosopher can eat only when it has 2 forks available and can only use forks that are in front of him.  
left fork = fork[philospher_id]  
right fork = fork[(philosopher_id+1) % philosopher_num]  

### My solution
#### Deadlocks
To prevent deadlocks, where every philospher takes one fork and noone can start eating, because it waits for the second fork,
I've implemented a semophore class that acts as a waiter. 
Waiter allows to eat NUM_PHILOSOPHERS – 1 persons, so there is always situation where at least one person can eat.


#### Race condition
To prevent race conditions, forks and printing are mutexes and they are guarded by unique_lock.lock().

#### Starving
To help with starving prevention in the branch [prevent-starving] there is a simple FIFO queue added,
to make sure that philosophers are served in the order that they requested for eating.

### How to run the program
There is a makefile that compiles the program using c++17.
To compile:
```make```

To run the program:
`./main`