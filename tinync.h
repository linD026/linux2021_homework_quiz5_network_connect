#ifndef __TINYNC_H_
#define __TINYNC_H_
/**
 * T1:
 * nc -l 127.0.0.1 1234 < /etc/lsb-release
 * T2:
 * ./tinync 127.0.0.1 1234 < tinync.c
 *
 * T1 expect:
 * tinync.c content
 *
 * T2 expect:
 * /etc/lsb-release content
 */

#include <stddef.h>

/* coroutine status values */
enum {
  CR_BLOCKED = 0,
  CR_FINISHED = 1,
  CR_YEILD = 2,
};

/* Helper macros to generate unique labels */
#define __cr_line3(name, line) _cr_##name##line
#define __cr_line2(name, line) __cr_line3(name, line)
#define __cr_line(name) __cr_line2(name, __LINE__)

struct cr {
  void *label;
  int status;
  void *local; /* private local storage */
};

#define cr_init()                                                              \
  { .label = NULL, .status = CR_BLOCKED }

#define cr_begin()                                                             \
  do {                                                                         \
    if ((o)->status == CR_FINISHED)                                            \
      return;                                                                  \
    if ((o)->label)                                                            \
      goto *(o)->label;                                                        \
  } while (0)

#define cr_label(stat)                                                         \
  do {                                                                         \
    (o)->status = (stat);                                                      \
    __cr_line(label) : (o)->label = &&__cr_line(label);                        \
  } while (0)

#define cr_end(final)                                                          \
  do {                                                                         \
    final;                                                                     \
    cr_label(CR_FINISHED);                                                     \
  } while (0)

#define cr_status(o) (o)->status

#define cr_wait(cond, final)                                                   \
  do {                                                                         \
    cr_label(CR_BLOCKED);                                                      \
    if (!(cond)) {                                                             \
      final;                                                                   \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define cr_exit(stat)                                                          \
  do {                                                                         \
    cr_label(stat);                                                            \
    return;                                                                    \
  } while (0)

#define cr_restart(final)                                                      \
  do {                                                                         \
    final;                                                                     \
    (o)->label = NULL;                                                         \
    (o)->status = CR_BLOCKED;                                                  \
    return;                                                                    \
  } while (0)

#define cr_yield(final)                                                        \
  do {                                                                         \
    cr_label(CR_BLOCKED);                                                      \
    final;                                                                     \
    return;                                                                    \
  } while (0)

#define cr_function(func_name, ...)                                            \
  void cr_##func_name(struct cr *o, __VA_ARGS__)
#define cr_call(func_name, o, ...) cr_##func_name(&(o), __VA_ARGS__)

/////////////////////////////////

#define cr_queue(T, size)                                                      \
  struct {                                                                     \
    T buf[size];                                                               \
    size_t r, w;                                                               \
  }
#define cr_queue_init()                                                        \
  { .r = 0, .w = 0 }
#define cr_queue_len(q) (sizeof((q)->buf) / sizeof((q)->buf[0]))
#define cr_queue_cap(q) ((q)->w - (q)->r)
#define cr_queue_empty(q) ((q)->w == (q)->r)
#define cr_queue_full(q) (cr_queue_cap(q) == cr_queue_len(q))

#define cr_queue_push(q, el)                                                   \
  (!cr_queue_full(q) && ((q)->buf[(q)->w++ % cr_queue_len(q)] = (el), 1))
#define cr_queue_pop(q)                                                        \
  (cr_queue_empty(q) ? NULL : &(q)->buf[(q)->r++ % cr_queue_len(q)])

/* Wrap system calls and other functions that return -1 and set errno */
#define cr_sys(o, call)                                                        \
  cr_wait(o, (errno = 0) || !(((call) == -1) &&                                \
                              (errno == EAGAIN || errno == EWOULDBLOCK ||      \
                               errno == EINPROGRESS || errno == EINTR)))

#endif