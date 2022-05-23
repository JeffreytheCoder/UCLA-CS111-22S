# Hash Hash Hash

This code implements mutex to prevent race condition of modifying hash table in multi-core scenarios.

## Building

The code uses pthread_mutex_t in pthread library to implement lock.

## Running

Show an example run of your (completed) program on using the `-t` and `-s` flags
of a run where the base hash table completes in between 1-2 seconds.



## First Implementation

In v1, I have a single lock using pthread_mutex_t. Whenever a thread or a process calls add_entry function, the code first tries to obtain the lock. If it fails, the function returns immediately. If it succeeds, the thread or process unlocks the lock at the end of the function.

The v1 approach makes sure only one thread/process is modifying the hash table, preventing race condition.

### Performance

Run the tester such that the base hash table completes in 1-2 seconds.
Report the relative speedup (or slow down) with a low number of threads and a
high number of threads. Note that the amount of work (`-t` times `-s`) should
remain constant. Explain any differences between the two.

## Second Implementation

The v1 approach makes parallelism useless since only one thread/process is allowed to obtain the lock and modify the hash table.

So in v2, I add a lock in the hash_table_entry struct. When a thread/process calls add_entry, the code first tries to obtain the lock under the specific hash table entry that the thread/process wants to modify. If it fails, the function returns immediately. If it succeeds, the thread or process unlocks the lock at the end of the function.

The v2 approach makes sure only one thread/process is modifying a certain hash table entry, preventing race condition while allowing modifying different entries in parallel.

### Performance

Run the tester such that the base hash table completes in 1-2 seconds.
Report the relative speedup with number of threads equal to the number of
physical cores on your machine (at least 4). Note again that the amount of work
(`-t` times `-s`) should remain constant. Report the speedup relative to the
base hash table implementation. Explain the difference between this
implementation and your first, which respect why you get a performance increase.

## Cleaning up

Run "make clean" to clean up all binary files.
