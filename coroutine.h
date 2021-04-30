#ifndef __COROUTINE_H__
#define __COROUTINE_H__
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

#endif