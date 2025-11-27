/**
 * \file            ringbuff.c
 * \brief           Ring buffer manager
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of ring buffer library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.2.0
 */
#include "utils/ringbuff.h"

/* --- Buffer unique part starts --- */
/* Prefix for all buffer functions and typedefs */
#define BUF_PREF(x)                     ring ## x
/* --- Buffer unique part ends --- */

/* Memory set and copy functions */
#define BUF_MEMSET                      memset
#define BUF_MEMCPY                      memcpy
#define BUF_IS_VALID(b)                 ((b) != NULL && (b)->buff != NULL && (b)->size > 0)
#define BUF_MIN(x, y)                   ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y)                   ((x) > (y) ? (x) : (y))

uint8_t
BUF_PREF(buff_init)(BUF_PREF(buff_t)* buff, void* buffdata, size_t size) {
    if (buff == NULL || buffdata == NULL || size == 0) {
        return 0;
    }

    BUF_MEMSET(buff, 0x00, sizeof(*buff));

    buff->size = size;
    buff->buff = buffdata;

    return 1;
}

void
BUF_PREF(buff_free)(BUF_PREF(buff_t)* buff) {
    if (BUF_IS_VALID(buff)) {
        buff->buff = NULL;
    }
}

size_t
BUF_PREF(buff_write)(BUF_PREF(buff_t)* buff, const void* data, size_t btw) {
    size_t tocopy, free;
    const uint8_t* d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btw == 0) {
        return 0;
    }

    free = BUF_PREF(buff_get_free)(buff);
    btw = BUF_MIN(free, btw);
    if (btw == 0) {
        return 0;
    }

    tocopy = BUF_MIN(buff->size - buff->w, btw);
    BUF_MEMCPY(&buff->buff[buff->w], d, tocopy);
    buff->w += tocopy;
    btw -= tocopy;

    if (btw > 0) {
        BUF_MEMCPY(buff->buff, &d[tocopy], btw);
        buff->w = btw;
    }

    if (buff->w >= buff->size) {
        buff->w = 0;
    }
    return tocopy + btw;
}

size_t
BUF_PREF(buff_read)(BUF_PREF(buff_t)* buff, void* data, size_t btr) {
    size_t tocopy, full;
    uint8_t *d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btr == 0) {
        return 0;
    }

    full = BUF_PREF(buff_get_full)(buff);
    btr = BUF_MIN(full, btr);
    if (btr == 0) {
        return 0;
    }

    tocopy = BUF_MIN(buff->size - buff->r, btr);
    BUF_MEMCPY(d, &buff->buff[buff->r], tocopy);
    buff->r += tocopy;
    btr -= tocopy;

    if (btr > 0) {
        BUF_MEMCPY(&d[tocopy], buff->buff, btr);
        buff->r = btr;
    }

    if (buff->r >= buff->size) {
        buff->r = 0;
    }
    return tocopy + btr;
}

size_t
BUF_PREF(buff_peek)(BUF_PREF(buff_t)* buff, size_t skip_count, void* data, size_t btp) {
    size_t full, tocopy, r;
    uint8_t *d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btp == 0) {
        return 0;
    }

    r = buff->r;

    full = BUF_PREF(buff_get_full)(buff);

    if (skip_count >= full) {
        return 0;
    }
    r += skip_count;
    full -= skip_count;
    if (r >= buff->size) {
        r -= buff->size;
    }

    btp = BUF_MIN(full, btp);
    if (btp == 0) {
        return 0;
    }

    tocopy = BUF_MIN(buff->size - r, btp);
    BUF_MEMCPY(d, &buff->buff[r], tocopy);
    btp -= tocopy;

    if (btp > 0) {
        BUF_MEMCPY(&d[tocopy], buff->buff, btp);
    }
    return tocopy + btp;
}

size_t
BUF_PREF(buff_get_free)(BUF_PREF(buff_t)* buff) {
    size_t size, w, r;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    w = buff->w;
    r = buff->r;
    if (w == r) {
        size = buff->size;
    } else if (r > w) {
        size = r - w;
    } else {
        size = buff->size - (w - r);
    }

    return size - 1;
}

size_t
BUF_PREF(buff_get_full)(BUF_PREF(buff_t)* buff) {
    size_t w, r, size;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    w = buff->w;
    r = buff->r;
    if (w == r) {
        size = 0;
    } else if (w > r) {
        size = w - r;
    } else {
        size = buff->size - (r - w);
    }
    return size;
}

void
BUF_PREF(buff_reset)(BUF_PREF(buff_t)* buff) {
    if (BUF_IS_VALID(buff)) {
        buff->w = 0;
        buff->r = 0;
    }
}

void *
BUF_PREF(buff_get_linear_block_read_address)(BUF_PREF(buff_t)* buff) {
    if (!BUF_IS_VALID(buff)) {
        return NULL;
    }
    return &buff->buff[buff->r];
}

size_t
BUF_PREF(buff_get_linear_block_read_length)(BUF_PREF(buff_t)* buff) {
    size_t w, r, len;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    w = buff->w;
    r = buff->r;
    if (w > r) {
        len = w - r;
    } else if (r > w) {
        len = buff->size - r;
    } else {
        len = 0;
    }
    return len;
}

size_t
BUF_PREF(buff_skip)(BUF_PREF(buff_t)* buff, size_t len) {
    size_t full;

    if (!BUF_IS_VALID(buff) || len == 0) {
        return 0;
    }

    full = BUF_PREF(buff_get_full)(buff);
    len = BUF_MIN(len, full);
    buff->r += len;
    if (buff->r >= buff->size) {
        buff->r -= buff->size;
    }
    return len;
}

void *
BUF_PREF(buff_get_linear_block_write_address)(BUF_PREF(buff_t)* buff) {
    if (!BUF_IS_VALID(buff)) {
        return NULL;
    }
    return &buff->buff[buff->w];
}

size_t
BUF_PREF(buff_get_linear_block_write_length)(BUF_PREF(buff_t)* buff) {
    size_t w, r, len;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    w = buff->w;
    r = buff->r;
    if (w >= r) {
        len = buff->size - w;
        if (r == 0) {
            --len;
        }
    } else {
        len = r - w - 1;
    }
    return len;
}

size_t
BUF_PREF(buff_advance)(BUF_PREF(buff_t)* buff, size_t len) {
    size_t free;

    if (!BUF_IS_VALID(buff) || len == 0) {
        return 0;
    }

    free = BUF_PREF(buff_get_free)(buff);
    len = BUF_MIN(len, free);
    buff->w += len;
    if (buff->w >= buff->size) {
        buff->w -= buff->size;
    }
    return len;
}

/**
 * \brief           Tìm chuỗi pattern trong buffer mà không làm thay đổi con trỏ đọc
 * \param[in]       buff: Buffer handle
 * \param[in]       pattern: Chuỗi cần tìm
 * \param[in]       pat_len: Độ dài chuỗi cần tìm
 * \return          >=0 là vị trí bắt đầu pattern trong buffer (tính từ r),
 *                  -1 nếu không tìm thấy
 */
int
BUF_PREF(buff_find)(BUF_PREF(buff_t)* buff, const void* pattern, size_t pat_len) {
    const uint8_t* pat = pattern;
    size_t full, i, j;

    if (!BUF_IS_VALID(buff) || pattern == NULL || pat_len == 0) {
        return -1;
    }

    full = BUF_PREF(buff_get_full)(buff);
    if (full < pat_len) {
        return -1;
    }

    for (i = 0; i <= full - pat_len; ++i) {
        int matched = 1;

        for (j = 0; j < pat_len; ++j) {
            size_t idx = buff->r + i + j;
            if (idx >= buff->size) {
                idx -= buff->size;
            }
            if (buff->buff[idx] != pat[j]) {
                matched = 0;
                break;
            }
        }

        if (matched) {
            return (int)i;   /* vị trí tương đối từ r */
        }
    }

    return -1;
}
