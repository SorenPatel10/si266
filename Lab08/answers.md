# MIDN 3/C Soren Patel
## Lab08 Intro to pthreads

### Part 1
1. The counter threads do not always run in strictly numerical order; the order changes in repeated runs. This is because the order is determined by the OS and the program does not use any specific methods to order them. The threads run concurrently.
2. After uncommenting this code, recompiling, and rerunning multiple times, the other threads still always printed their thread ID's. 
3. No. One thread blocking does not cause the other threads to block.
4. The main thread is waiting on getchar().

### Part 2
5. There is a race condition with next_uncracked_digest_index because multiple threads can read it at the same time and overwrite each other's values. There is no thread syncrhonization preventing this from occurring. 

### Part 4
6.  | #Threads | Runtime |
    |----------|------------|
    |1         | 1m52.124s       |
    |2         | 1m38.986s       |
    |4         | 0m49.282s       |
    |8         | 0m44.452s       |
    |16        | 0m38.353s       |

7. With 4 logical CPUs, 4 threads seemed to give the most efficiency for the least amount of threads. It was an improvement over 2, which was a large improvement over 1. Using 8 or 16 threads didnt seem to give much more effiency. I think that correlates to the number of cores, perhaps one core runs one thread each. 