/*
 * Copyright (c) 2011, Oracle and/or its affiliates.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * Tests for fallback implementations of string handling routines
 * provided in os/ subdirectory for some platforms.
 */

/* Test relies on assert() */
#undef NDEBUG

#include <dix-config.h>

#include <assert.h>
#include "os.h"
#include "tests-common.h"

/* Ensure we're testing our functions, even on platforms with libc versions */
#include <string.h>
#undef strndup
#define strndup my_strndup
char *strndup(const char *str, size_t n);

#include "../os/strndup.c"

static void
strndup_checks(void)
{
    const char *sample = "0123456789abcdef";
    char *allofit;

    char *firsthalf = strndup(sample, 8);
    char *secondhalf = strndup(sample + 8, 8);

    assert(firsthalf);
    assert(secondhalf);
    assert(strcmp(firsthalf, "01234567") == 0);
    assert(strcmp(secondhalf, "89abcdef") == 0);

    free(firsthalf);
    free(secondhalf);

    allofit = strndup(sample, 20);
    assert(allofit);
    assert(strcmp(allofit, sample) == 0);
    free(allofit);
}

const testfunc_t*
string_test(void)
{
    static const testfunc_t testfuncs[] = {
        strndup_checks,
        NULL,
    };

    return testfuncs;
}
