1️⃣ printf / fprintf
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);

// Example:
printf("Hello, %s\n", name);
fprintf(stderr, "Error code: %d\n", code);

✅ fprintf lets you print to any FILE* (stdout, stderr, files).

2️⃣ scanf / sscanf / fscanf
int scanf(const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);

// Example:
int x; scanf("%d", &x);
char buffer[100]; sscanf(str, "%s %d", name, &age);

✅ sscanf parses a string, scanf reads from stdin, fscanf reads from a file.

3️⃣ fgets / fputs
char *fgets(char *s, int size, FILE *stream);
int fputs(const char *s, FILE *stream);

// Example:
char line[256];
fgets(line, sizeof(line), fp);
fputs(line, stdout);

✅ Always provide buffer size to avoid overflow.

4️⃣ strcmp / strncmp / strcasecmp
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

// Example:
if (strncmp(str, prefix, strlen(prefix)) == 0) { ... }

✅ Compares strings; returns 0 if equal.

5️⃣ strcpy / strncpy / strcat / strncat
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);

// Example:
char buf[100];
strncpy(buf, "Hello", sizeof(buf)-1);
buf[sizeof(buf)-1] = '\0';  // ensure null termination

✅ strncpy can prevent buffer overflows but may not null-terminate automatically.

6️⃣ strlen / strchr / strstr
size_t strlen(const char *s);
char *strchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);

// Example:
size_t len = strlen(str);
char *p = strchr(str, 'a');     // first occurrence of 'a'
char *q = strstr(str, "abc");   // substring
7️⃣ malloc / free / calloc / realloc
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

// Example:
int *arr = malloc(10 * sizeof(int));
if (!arr) exit(1);
free(arr);

✅ Always free memory you allocate.

8️⃣ fopen / fclose / fread / fwrite
FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

// Example:
FILE *fp = fopen("data.txt", "r");
fclose(fp);

✅ Mode: "r", "w", "a", "r+", etc.

9️⃣ fork / exec / wait
pid_t fork(void);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
int execlp(const char *file, const char *arg0, ..., NULL);

// Example:
pid_t pid = fork();
if (pid == 0) execlp("ls", "ls", "-l", NULL);
else wait(NULL);

✅ fork duplicates process; exec replaces it; wait collects child exit status.

🔟 pipe / dup2 / close
int pipe(int fd[2]);
int dup2(int oldfd, int newfd);
int close(int fd);

// Example: redirect stdout to pipe
int fd[2]; pipe(fd);
dup2(fd[1], STDOUT_FILENO);
close(fd[0]); close(fd[1]);

✅ Always close unused ends to prevent deadlocks.

1️⃣1️⃣ getpid / getppid
pid_t getpid(void);   // returns process ID
pid_t getppid(void);  // returns parent process ID

// Example:
printf("My PID: %d\n", getpid());
1️⃣2️⃣ perror / strerror
void perror(const char *s);        // prints s + system error
char *strerror(int errnum);        // returns error string

// Example:
FILE *fp = fopen("file.txt","r");
if (!fp) perror("fopen failed");
1️⃣3️⃣ Signal/Exit Macros
WIFEXITED(status)   // true if child exited normally
WEXITSTATUS(status) // exit code
WIFSIGNALED(status) // true if killed by signal
WTERMSIG(status)    // signal number

1️⃣4️⃣ Notes / Tips

fgets() reads up to n-1 characters, always provide buffer size.

strncpy() may not null-terminate, fix with buf[n-1]='\0'.

fork() creates two copies of process, parent & child.

exec() never returns on success.

Always check return values: malloc, fopen, fork, pipe, dup2, exec.