#ifndef NANVIX_UTILS_H_
#define NANVIX_UTILS_H_

#define MIN(x, y) \
    ({ __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _x < _y ? _x : _y; })

#endif
