# Hash Hash Hash

This code implements mutex to prevent race condition of modifying hash table in multi-core scenarios.

## Building

The code uses pthread_mutex_t in pthread library to implement lock.

## Running

Use "./hash-table-tester" with options "-t" as number of threads and "-s" as number of entries per thread.
e.g. ./hash-table-tester -t 8 -s 50000

## First Implementation

In v1, I have a single lock using pthread_mutex_t. Whenever a thread or a process calls add_entry function, the code first tries to obtain the lock. If it fails, the function returns immediately. If it succeeds, the thread or process unlocks the lock at the end of the function.

The v1 approach makes sure only one thread/process is modifying the hash table, preventing race condition.

### Performance

./hash-table-tester -t 8 -s 50000 gives the base case in 8.479 secs and v1 in 15.663 secs. 

./hash-table-tester -t 4 -s 100000 gives the base case in 10.536 secs and v1 in 18.585 secs. 

v1 is slower than the base case because the v1 implementation makes parallelism useless since only one thread/process is allowed to obtain the lock and modify the hash table.

With half of the threads, the performance of v1 doesn't change significantly since it doesn't use parallelism, and the total entries it needs to process is still 400,000.

## Second Implementation

So in v2, I add a lock in the hash_table_entry struct. When a thread/process calls add_entry, the code first tries to obtain the lock under the specific hash table entry that the thread/process wants to modify. If it fails, the function returns immediately. If it succeeds, the thread or process unlocks the lock at the end of the function.

The v2 approach makes sure only one thread/process is modifying a certain hash table entry, preventing race condition while allowing modifying different entries in parallel.

### Performance

Run the tester such that the base hash table completes in 1-2 seconds.
Report the relative speedup with number of threads equal to the number of
physical cores on your machine (at least 4). Note again that the amount of work
(`-t` times `-s`) should remain constant. Report the speedup relative to the
base hash table implementation. Explain the difference between this
implementation and your first, which respect why you get a performance increase.

./hash-table-tester -t 8 -s 50000 gives the base case in 8.479 secs and v2 in 3.916 secs. 

./hash-table-tester -t 4 -s 100000 gives the base case in 10.536 secs and v1 in 5.076 secs. 

v2 is faster than the base case because the v2 implementation have locks for each hash table entry, ensuring the correct parallelism and preventing race condition from happening.

With half of the threads, the performance of v2 improves since more threads results in a better performance in parallel.

## Cleaning up

Run "make clean" to clean up all binary files.
