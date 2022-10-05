#ifndef UTILS_H
#define UTILS_H
#define _GNU_SOURCE

#include "utils/logging/logging.h"
#include "utils/mutex.h"

#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.h>

#define VAR_UINT8 1
#define VAR_INT8 2
#define VAR_UINT16 3
#define VAR_INT16 4
#define VAR_INT 5
#define VAR_INT64 6
#define VAR_STRING 7
#define VAR_PSTRING 8
#define VAR_FLOAT 9
#define VAR_HEX 10
#define VAR_ARRAY 16
#define VAR_ARRAY_UINT8 (VAR_ARRAY + VAR_UINT8)
#define VAR_ARRAY_INT8 (VAR_ARRAY + VAR_INT8)
#define VAR_ARRAY_UINT16 (VAR_ARRAY + VAR_UINT16)
#define VAR_ARRAY_INT16 (VAR_ARRAY + VAR_INT16)
#define VAR_ARRAY_INT (VAR_ARRAY + VAR_INT)
#define VAR_ARRAY_INT64 (VAR_ARRAY + VAR_INT64)
#define VAR_ARRAY_FLOAT (VAR_ARRAY + VAR_FLOAT)
#define VAR_ARRAY_HEX (VAR_ARRAY + VAR_HEX)
#define VAR_ARRAY_STRING (VAR_ARRAY + VAR_STRING)
#define VAR_ARRAY_PSTRING (VAR_ARRAY + VAR_PSTRING)
#define VAR_AARRAY 32
#define VAR_AARRAY_UINT8 (VAR_AARRAY + VAR_UINT8)
#define VAR_AARRAY_INT8 (VAR_AARRAY + VAR_INT8)
#define VAR_AARRAY_UINT16 (VAR_AARRAY + VAR_UINT16)
#define VAR_AARRAY_INT16 (VAR_AARRAY + VAR_INT16)
#define VAR_AARRAY_INT (VAR_AARRAY + VAR_INT)
#define VAR_AARRAY_INT64 (VAR_AARRAY + VAR_INT64)
#define VAR_AARRAY_FLOAT (VAR_AARRAY + VAR_FLOAT)
#define VAR_AARRAY_HEX (VAR_AARRAY + VAR_HEX)
#define VAR_AARRAY_STRING (VAR_AARRAY + VAR_STRING)
#define VAR_AARRAY_PSTRING (VAR_AARRAY + VAR_PSTRING)
#define VAR_FUNCTION 48
#define VAR_FUNCTION_INT (VAR_FUNCTION + VAR_INT)
#define VAR_FUNCTION_INT64 (VAR_FUNCTION + VAR_INT64)
#define VAR_FUNCTION_STRING (VAR_FUNCTION + VAR_STRING)

typedef int (*get_data_int)(int p);
typedef int64_t (*get_data_int64)(int p);
typedef char *(*get_data_string)(int p, char *dest, int max_len);

typedef struct struct_symbols {
    const char *name;
    int type;
    void *addr;
    float multiplier; // multiply the value of the variable
    int len;
    int skip;
} _symbols;

int split(char **rv, char *s, int lrv, char sep);
int map_int(char *s, char **v);
int map_intd(char *s, char **v, int dv);
int map_float(char *s, int mul);
int check_strs(char *s, char **v, int dv);
void *mymalloc(int a, char *f, int l);
void *myrealloc(void *p, int a, char *f, int l);
void myfree(void *x, char *f, int l);
char *header_parameter(char **arg, int i);
char *get_current_timestamp();
char *strip(char *s);
int split(char **rv, char *s, int lrv, char sep);
void set_signal_handler(char *argv0);
int becomeDaemon();
int end_of_header(char *buf);
char *readfile(char *fn, char *ctype, int *len);
int get_json_state(char *buf, int len);
int get_json_bandwidth(char *buf, int len);
void process_file(void *sock, char *s, int len, char *ctype);
int closefile(char *mem, int len);

pthread_t start_new_thread(char *name);
pthread_t get_tid();
void set_thread_prio(pthread_t tid, int prio);

int add_new_lock(void **arr, int count, int size, SMutex *mutex);
void join_thread();
void add_join_thread(pthread_t t);
int init_utils(char *arg0);
void _hexdump(char *desc, void *addr, int len);
uint32_t crc_32(const uint8_t *data, int datalen);
void _dump_packets(char *message, unsigned char *b, int len, int packet_offset);
int buffer_to_ts(uint8_t *dest, int dstsize, uint8_t *src, int srclen, char *cc,
                 int pid);
void write_buf_to_file(char *file, uint8_t *buf, int len);
void mkdir_recursive(const char *path);
void sleep_msec(uint32_t msec);
int get_random(unsigned char *dest, int len);

#define dump_packets(message, b, len, packet_offset)                           \
    if (DEFAULT_LOG & opts.debug)                                              \
    _dump_packets(message, b, len, packet_offset)
#define hexdump(message, b, len)                                               \
    if (DEFAULT_LOG & opts.debug)                                              \
    _hexdump(message, b, len)

#define malloc1(a) mymalloc(a, __FILE__, __LINE__)
#define free1(a) myfree(a, __FILE__, __LINE__)
#define realloc1(a, b) myrealloc(a, b, __FILE__, __LINE__)

#define strlcatf(buf, size, ptr, fmt...)                                       \
    do {                                                                       \
        int __r = snprintf((buf) + ptr, (size)-ptr, fmt);                      \
        ptr += __r;                                                            \
        if (ptr >= (size)) {                                                   \
            LOG("%s buffer size too small (%d)", __FUNCTION__, size);          \
            ptr = (size)-1;                                                    \
        }                                                                      \
    } while (0)

#define strcatf(buf, ptr, fmt, ...)                                            \
    strlcatf(buf, sizeof(buf) - 1, ptr, fmt, ##__VA_ARGS__)

#define SAFE_STRCPY(a, b)                                                      \
    {                                                                          \
        int x = sizeof(a);                                                     \
        if (x < 10)                                                            \
            LOG("sizeof %d is too small at %s:%d", x, __FILE__, __LINE__);     \
        strncpy(a, b, x - 1);                                                  \
        a[x - 1] = 0;                                                          \
    }

typedef ssize_t (*mywritev)(int fd, const struct iovec *io, int len);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define PID_FROM_TS(b) (((b)[1] & 0x1F) * 256 + (b)[2])

#ifdef TESTING
#define writev(a, b, c) _writev(a, b, c)
#endif

#ifdef UTILS_C
char *loglevels[] = {"general",   "http", "socketworks", "stream", "adapter",
                     "satipc",    "pmt",  "tables",      "dvbapi", "lock",
                     "netceiver", "ca",   "axe",         "socket", "utils",
                     "dmx",       "ssdp", "dvb",         NULL};
mywritev _writev = writev;
#else
extern char *loglevels[];
extern mywritev _writev;
#endif

#endif
