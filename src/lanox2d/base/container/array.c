/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        array.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "array.h"
#include "iterator.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default grow number and maximum size
#ifdef LX_CONFIG_SMALL
#   define LX_ARRAY_GROW             (128)
#   define LX_ARRAY_MAXN             (1 << 16)
#else
#   define LX_ARRAY_GROW             (256)
#   define LX_ARRAY_MAXN             (1 << 30)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the array type
typedef struct lx_array_t_ {
    lx_iterator_base_t      base;
    lx_byte_t*              data;
    lx_size_t               size;
    lx_size_t               grow;
    lx_size_t               maxn;
    lx_size_t               itemsize;
    lx_element_free_t       itemfree;
}lx_array_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_size_t lx_array_iterator_head(lx_iterator_ref_t iterator) {
    return 0;
}

static lx_size_t lx_array_iterator_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    lx_array_t* array = (lx_array_t*)iterator->container;
    return array->size;
}

static lx_size_t lx_array_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_array_t* array = (lx_array_t*)iterator->container;
    lx_assert_and_check_return_val(itor < array->size, array->size);
    return itor + 1;
}

static lx_size_t lx_array_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_array_t* array = (lx_array_t*)iterator->container;
    lx_assert_and_check_return_val(itor && itor <= array->size, 0);
    return itor - 1;
}

static lx_pointer_t lx_array_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    return lx_array_item((lx_array_ref_t)iterator->container, itor);
}

static lx_void_t lx_array_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_array_iterator_head,
        lx_array_iterator_tail,
        lx_array_iterator_prev,
        lx_array_iterator_next,
        lx_array_iterator_item
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_array_ref_t lx_array_init(lx_size_t grow, lx_size_t itemsize, lx_element_free_t itemfree) {

    // check
    lx_assert_and_check_return_val(itemsize, lx_null);

    lx_bool_t   ok = lx_false;
    lx_array_t* array = lx_null;
    do {
        // using the default grow
        if (!grow) grow = LX_ARRAY_GROW;

        // init array
        array = lx_malloc0_type(lx_array_t);
        lx_assert_and_check_break(array);

        array->size             = 0;
        array->maxn             = 0;
        array->grow             = grow;
        array->itemsize         = itemsize;
        array->itemfree         = itemfree;
        array->base.iterator_of = lx_array_iterator_of;

        // ok
        ok = lx_true;
    } while (0);

    if (!ok && array) {
        lx_array_exit((lx_array_ref_t)array);
        array = lx_null;
    }
    return (lx_array_ref_t)array;
}

lx_void_t lx_array_exit(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    if (array) {
        if (array->data) {
            lx_array_clear(self);
            lx_free(array->data);
            array->data = lx_null;
        }
        lx_free(array);
    }
}

lx_pointer_t lx_array_data(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    return array? array->data : lx_null;
}

lx_size_t lx_array_size(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    return array? array->size : 0;
}

lx_pointer_t lx_array_head(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    if (array && array->data && array->size) {
        return array->data;
    }
    return lx_null;
}

lx_pointer_t lx_array_last(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    if (array && array->data && array->size) {
        return array->data + (array->size - 1) * array->itemsize;
    }
    return lx_null;
}

lx_pointer_t lx_array_item(lx_array_ref_t self, lx_size_t index) {
    lx_array_t* array = (lx_array_t*)self;
    if (array && array->data && index < array->size) {
        return array->data + index * array->itemsize;
    }
    return lx_null;
}

lx_bool_t lx_array_resize(lx_array_ref_t self, lx_size_t size) {
    lx_array_t* array = (lx_array_t*)self;
    lx_assert_and_check_return_val(array, lx_false);

    // free items if the array is decreased
    lx_size_t  itemsize = array->itemsize;
    lx_byte_t* arraydata = array->data;
    lx_size_t  arraysize = array->size;
    if (size < arraysize) {
        lx_assert(arraydata);
        if (array->itemfree) {
            lx_size_t  i;
            for (i = size; i < arraysize; i++) {
                array->itemfree(arraydata + i * itemsize);
            }
        }
    }

    // resize buffer
    if (size > array->maxn) {
        lx_size_t maxn = lx_align4(size + array->grow);
        lx_assert_and_check_return_val(maxn < LX_ARRAY_MAXN, lx_false);

        if (arraydata) {
            array->data = (lx_byte_t*)lx_ralloc(arraydata, maxn * itemsize);
        } else {
            array->data = (lx_byte_t*)lx_malloc(maxn * itemsize);
        }
        lx_assert_and_check_return_val(array->data, lx_false);
        lx_assert_and_check_return_val(!(((lx_size_t)(array->data)) & 3), lx_false);

        lx_memset(array->data + arraysize * itemsize, 0, (maxn - array->maxn) * itemsize);
        array->maxn = maxn;
    }
    array->size = size;
    return lx_true;
}

lx_void_t lx_array_clear(lx_array_ref_t self) {
    lx_array_t* array = (lx_array_t*)self;
    if (array && array->data) {
        if (array->itemfree) {
            lx_size_t  i;
            lx_byte_t* data = array->data;
            lx_size_t  size = array->size;
            lx_size_t  itemsize = array->itemsize;
            for (i = 0; i < size; i++) {
                array->itemfree(data + i * itemsize);
            }
        }
        array->size = 0;
    }
}

lx_void_t lx_array_copy(lx_array_ref_t self, lx_array_ref_t copied) {
    lx_array_t* array = (lx_array_t*)self;
    lx_array_t* array_copied = (lx_array_t*)copied;
    lx_assert_and_check_return(array && array_copied);
    lx_assert_and_check_return(array->itemfree == array_copied->itemfree);
    lx_assert_and_check_return(array->itemsize == array_copied->itemsize);

    // the copied array is empty? clear it directly
    if (!array_copied->size) {
        lx_array_clear(self);
        return ;
    }

    // ensure enough space
    if (array->size < array_copied->size) {
        if (!lx_array_resize(self, array_copied->size)) {
            return ;
        }
    }
    lx_assert_and_check_return(array->data && array_copied->data && array->size >= array_copied->size);

    // copy data and size
    if (array_copied->data != array->data) {
        lx_memcpy(array->data, array_copied->data, array_copied->size * array_copied->itemsize);
    }
    array->size = array_copied->size;
}

lx_void_t lx_array_insert(lx_array_ref_t self, lx_size_t index, lx_cpointer_t data) {
    lx_array_t* array = (lx_array_t*)self;
    if (array && lx_array_resize(self, array->size + 1)) {
        lx_byte_t* arraydata = array->data;
        lx_size_t  itemsize  = array->itemsize;
        lx_size_t  oldsize   = array->size - 1;
        lx_assert_and_check_return(arraydata && index < array->size);
        if (index < oldsize) {
            lx_memmov(arraydata + (index + 1) * itemsize, arraydata + index * itemsize, (oldsize - index) * itemsize);
        }
        lx_pointer_t item = arraydata + index * itemsize;
        if (itemsize == sizeof(lx_pointer_t)) {
            *((lx_pointer_t*)item) = *((lx_pointer_t*)data);
        } else {
            lx_memcpy(item, data, itemsize);
        }
    }
}

lx_void_t lx_array_insert_tail(lx_array_ref_t self, lx_cpointer_t data) {
    lx_array_insert(self, lx_array_size(self), data);
}

lx_void_t lx_array_replace(lx_array_ref_t self, lx_size_t index, lx_cpointer_t data) {
    lx_array_t* array = (lx_array_t*)self;
    if (array) {
        lx_pointer_t item = lx_array_item(self, index);
        lx_assert_and_check_return(item);
        if (array->itemfree) {
            array->itemfree(item);
        }
        if (array->itemsize == sizeof(lx_pointer_t)) {
            *((lx_pointer_t*)item) = *((lx_pointer_t*)data);
        } else {
            lx_memcpy(item, data, array->itemsize);
        }
    }
}

lx_void_t lx_array_replace_head(lx_array_ref_t self, lx_cpointer_t data) {
    lx_size_t size = lx_array_size(self);
    if (size) {
        lx_array_replace(self, 0, data);
    }
}

lx_void_t lx_array_replace_last(lx_array_ref_t self, lx_cpointer_t data) {
    lx_size_t size = lx_array_size(self);
    if (size) {
        lx_array_replace(self, size - 1, data);
    }
}

lx_void_t lx_array_remove_last(lx_array_ref_t self) {
    lx_size_t size = lx_array_size(self);
    if (size) {
        lx_array_resize(self, size - 1);
    }
}

