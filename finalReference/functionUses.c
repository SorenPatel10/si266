1️⃣ sigaction / signals
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

✅ Reliable signal handling (better than signal())
✅ Use sig_atomic_t for shared variables

2️⃣ kill / alarm / pause / sigsuspend
int kill(pid_t pid, int sig);
unsigned int alarm(unsigned int seconds);
int pause(void);
int sigsuspend(const sigset_t *mask);

✅ Used for process communication + timing

3️⃣ pthread_create / join / detach
int pthread_create(pthread_t *thread, ...);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);

✅ join = wait for thread
✅ detach = auto cleanup (used in servers)

4️⃣ pthread_mutex
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);
pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);

✅ Prevent race conditions

5️⃣ semaphores
sem_init(&sem, 0, value);
sem_wait(&sem);
sem_post(&sem);

✅ Controls access count (not just 1 like mutex)

6️⃣ condition variables
pthread_cond_wait(&cond, &mutex);
pthread_cond_signal(&cond);

✅ Always use with mutex
✅ Use WHILE loop (not if!)

7️⃣ socket API
int socket(int domain, int type, int protocol);
int connect(int sockfd, ...);
int bind(int sockfd, ...);
int listen(int sockfd, int backlog);
int accept(int sockfd, ...);

✅ Client: socket → connect
✅ Server: socket → bind → listen → accept

8️⃣ getaddrinfo / freeaddrinfo
int getaddrinfo(...);
void freeaddrinfo(...);

✅ Modern DNS resolution (don’t use gethostbyname)

9️⃣ read / write
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

✅ May return partial reads/writes → handle carefully

🔟 htons / ntohs
uint16_t htons(uint16_t hostshort);

✅ Convert host ↔ network byte order
