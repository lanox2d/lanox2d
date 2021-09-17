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
 * @file        dlopen.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <elf.h>
#include <link.h>
#include <pthread.h>
#include <sys/system_properties.h>
#include <jni.h>
#include <stdio.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the fake dlopen magic
#define LX_FAKE_DLCTX_MAGIC      (0xfaddfadd)

/* g_dl_mutex in linker
 *
 * @see http://androidxref.com/5.0.0_r2/xref/bionic/linker/dlfcn.cpp#32
 */
#define LX_LINKER_MUTEX         "__dl__ZL10g_dl_mutex"

// the linker name
#ifndef __LP64__
#   define LX_LINKER_NAME       "linker"
#else
#   define LX_LINKER_NAME       "linker64"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the dynamic library context type for fake dlopen
typedef struct _lx_fake_dlctx_t
{
    // magic, mark handle for fake dlopen
    lx_uint32_t     magic;

    // the load bias address of the dynamic library
    lx_pointer_t    biasaddr;

    // the .dynsym and .dynstr sections
    lx_pointer_t    dynstr;
    lx_pointer_t    dynsym;
    lx_int_t        dynsym_num;

    // the .symtab and .strtab sections
    lx_pointer_t    strtab;
    lx_pointer_t    symtab;
    lx_int_t        symtab_num;

    // the file data and size
    lx_pointer_t    filedata;
    lx_size_t       filesize;

}lx_fake_dlctx_t, *lx_fake_dlctx_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the jni environment on tls
__thread JNIEnv*        g_tls_jnienv = lx_null;
static JavaVM*          g_jvm = lx_null;
static lx_int_t         g_jversion = JNI_VERSION_1_4;
static pthread_mutex_t* g_linker_mutex = lx_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
extern __attribute((weak)) lx_int_t dl_iterate_phdr(lx_int_t (*)(struct dl_phdr_info*, size_t, lx_pointer_t), lx_pointer_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */

// weak symbol import
lx_void_t __system_property_read_callback(
    prop_info const* info,
    lx_void_t (*callback)(lx_pointer_t cookie, lx_char_t const* name, lx_char_t const* value, uint32_t serial),
    lx_void_t* cookie) __attribute__((weak));

lx_int_t __system_property_get(lx_char_t const* name, lx_char_t* value) __attribute__((weak));

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* Technical note regarding reading system properties.
 *
 * Try to use the new __system_property_read_callback API that appeared in
 * Android O / API level 26 when available. Otherwise use the deprecated
 * __system_property_get function.
 *
 * For more technical details from an NDK maintainer, see:
 * https://bugs.chromium.org/p/chromium/issues/detail?id=392191#c17
 */

// callback used with __system_property_read_callback.
static lx_void_t lx_rt_prop_read_int(lx_pointer_t cookie, lx_char_t const* name, lx_char_t const* value, uint32_t serial)
{
    *(lx_int_t *)cookie = atoi(value);
    (lx_void_t)name;
    (lx_void_t)serial;
}

// read process output
static lx_int_t lx_rt_process_read(lx_char_t const* cmd, lx_char_t* data, lx_size_t maxn)
{
    lx_int_t n = 0;
    FILE*    p = popen(cmd, "r");
    if (p)
    {
        lx_char_t  buf[256] = {0};
        lx_char_t* pos = data;
        lx_char_t* end = data + maxn;
        while (!feof(p))
        {
            if (fgets(buf, sizeof(buf), p))
            {
               lx_int_t len = lx_strlen(buf);
               if (pos + len < end)
               {
                   lx_memcpy(pos, buf, len);
                   pos += len;
                   n   += len;
               }
            }
        }

        *pos = '\0';
        pclose(p);
    }
    return n;
}

// get system property integer
static lx_int_t lx_rt_system_property_get_int(lx_char_t const* name)
{
    // check
    lx_assert_and_check_return_val(name, -1);

    lx_int_t result = 0;
    if (__system_property_read_callback)
    {
        struct prop_info const* info = __system_property_find(name);
        if (info) __system_property_read_callback(info, &lx_rt_prop_read_int, &result);
    }
    else if (__system_property_get)
    {
        lx_char_t value[PROP_VALUE_MAX] = {0};
        if (__system_property_get(name, value) >= 1)
            result = atoi(value);
    }
    else
    {
        lx_char_t cmd[256];
        lx_char_t value[PROP_VALUE_MAX];
        snprintf(cmd, sizeof(cmd), "getprop %s", name);
        if (lx_rt_process_read(cmd, value, sizeof(value)) > 1)
            result = atoi(value);
    }
    return result;
}

static lx_int_t lx_rt_api_level()
{
    static lx_int_t s_api_level = -1;
    if (s_api_level < 0)
        s_api_level = lx_rt_system_property_get_int("ro.build.version.sdk");
    return s_api_level;
}

// find the load bias address from the base address
static lx_pointer_t lx_fake_find_biasaddr_from_baseaddr(lx_pointer_t baseaddr)
{
    // check
    lx_assert_and_check_return_val(baseaddr, lx_null);

    // find load bias from program header
    ElfW(Ehdr)*       ehdr = (ElfW(Ehdr)*)baseaddr;
    ElfW(Phdr) const* dlpi_phdr = (ElfW(Phdr) const*)(baseaddr + ehdr->e_phoff);
    lx_int_t          dlpi_phnum = ehdr->e_phnum;
    uintptr_t         min_vaddr = UINTPTR_MAX;
    for (lx_int_t i = 0; i < dlpi_phnum; i++)
    {
        ElfW(Phdr) const* phdr = &(dlpi_phdr[i]);
        if (PT_LOAD == phdr->p_type)
        {
            if (min_vaddr > phdr->p_vaddr)
                min_vaddr = phdr->p_vaddr;
        }
    }
    return min_vaddr != UINTPTR_MAX? baseaddr - min_vaddr : lx_null;
}

// find the load bias address and real path from the maps
static lx_pointer_t lx_fake_find_biasaddr_from_maps(lx_char_t const* filename, lx_char_t* realpath, lx_size_t realmaxn)
{
    // check
    lx_assert_and_check_return_val(filename && realpath && realmaxn, lx_null);

    // trace
    lx_trace_d("find biasaddr of %s from maps", filename);

    // find it
    lx_char_t    line[512];
    lx_char_t    page_attr[10];
    lx_pointer_t biasaddr = lx_null;
    FILE* fp = fopen("/proc/self/maps", "r");
    if (fp)
    {
        while (fgets(line, sizeof(line), fp))
        {
            if (lx_strstr(line, filename))
            {
                int       pos = 0;
                uintptr_t start = 0;
                uintptr_t offset = 0;
                // 7372a68000-7372bc1000 --xp 000fe000 fd:06 39690571                       /system/lib64/libandroid_runtime.so
                if (3 == sscanf(line, "%"SCNxPTR"-%*"SCNxPTR" %4s %"SCNxPTR" %*x:%*x %*d%n", &start, page_attr, &offset, &pos))
                {
                    // check permission and offset
                    if (page_attr[0] != 'r') continue;
                    if (page_attr[3] != 'p') continue;
                    if (0 != offset) continue;

                    // get load bias address
                    biasaddr = lx_fake_find_biasaddr_from_baseaddr((lx_pointer_t)start);

                    // get real path
                    if (filename[0] == '/')
                        lx_strlcpy(realpath, filename, realmaxn);
                    else if (pos < sizeof(line))
                    {
                        lx_char_t* p = line + pos;
                        lx_char_t* e = p + lx_strlen(p);
                        while (p < e && lx_isspace((lx_int_t)*p)) p++;
                        while (p < e && lx_isspace((lx_int_t)(*(e - 1)))) e--;
                        *e = '\0';
                        if (p < e) lx_strlcpy(realpath, p, realmaxn);
                        else realpath[0] = '\0';
                    }
                    else realpath[0] = '\0';

                    // trace
                    lx_trace_d("realpath: %s, biasaddr: %p found!", realpath, biasaddr);
                }
                break;
            }
        }
        fclose(fp);
    }
    return biasaddr;
}

// the callback of dl_iterate_phdr()
static lx_int_t lx_fake_find_biasaddr_from_linker_cb(struct dl_phdr_info* info, size_t size, lx_pointer_t udata)
{
    // check
    lx_cpointer_t* args = (lx_cpointer_t*)udata;
    lx_check_return_val(args, 1);
    lx_check_return_val(info && info->dlpi_addr && info->dlpi_name && info->dlpi_name[0] != '\0', 0);

    // get filename
    lx_char_t const* filename = lx_null;
    lx_char_t const* filepath = (lx_char_t const*)args[0];
    lx_assert_and_check_return_val(filepath, 1);
    if (filepath[0] == '/')
    {
        lx_char_t const* p = filepath + lx_strlen(filepath);
        while (p >= filepath && *p != '/')
            p--;
        if (p >= filepath && *p == '/') filename = p + 1;
    }

    // find library, we can also get full path of dlpi_name from maps
    lx_pointer_t*    pbiasaddr = (lx_pointer_t*)&args[3];
    lx_char_t*       realpath  = (lx_char_t*)args[1];
    lx_size_t        realmaxn  = (lx_size_t)args[2];
    if ((filepath && lx_strstr(info->dlpi_name, filepath)) ||
        (filename && !lx_strcmp(info->dlpi_name, filename))) // dlpi_name ma ybe not full path, e.g. libart.so
    {
        // save load bias address
        *pbiasaddr = (lx_pointer_t)info->dlpi_addr;

        // get real path
        if (filepath[0] == '/')
            lx_strlcpy(realpath, filepath, realmaxn);
        else if (info->dlpi_name[0] == '/')
            lx_strlcpy(realpath, info->dlpi_name, realmaxn);
        else
        {
            // we only find real path
            if (!lx_fake_find_biasaddr_from_maps(filepath, realpath, realmaxn))
                realpath[0] = '\0';
        }

        // trace
        lx_trace_d("realpath: %s, biasaddr: %p found!", realpath, (lx_pointer_t)info->dlpi_addr);

        // found, stop it
        return 1;
    }
    return 0;
}

// find the load bias address and real path from the maps
static lx_pointer_t lx_fake_find_biasaddr_from_linker(lx_char_t const* filepath, lx_char_t* realpath, lx_size_t realmaxn)
{
    // check
    lx_assert_and_check_return_val(dl_iterate_phdr && filepath && realpath && realmaxn, lx_null);

    // trace
    lx_trace_d("find biasaddr of %s from linker", filepath);

    // find biasaddr
    lx_cpointer_t args[4];
    args[0] = (lx_cpointer_t)filepath;
    args[1] = (lx_cpointer_t)realpath;
    args[2] = (lx_cpointer_t)realmaxn;
    args[3] = lx_null;
    if (g_linker_mutex) pthread_mutex_lock(g_linker_mutex);
    dl_iterate_phdr(lx_fake_find_biasaddr_from_linker_cb, args);
    if (g_linker_mutex) pthread_mutex_unlock(g_linker_mutex);
    return (lx_pointer_t)args[3];
}

// find the load bias address and real path
static lx_pointer_t lx_fake_find_biasaddr(lx_char_t const* filename, lx_char_t* realpath, lx_size_t realmaxn)
{
    lx_assert_and_check_return_val(filename && realpath, lx_null);
    lx_pointer_t biasaddr = lx_null;
    if (dl_iterate_phdr && 0 != lx_strcmp(filename, LX_LINKER_NAME))
        biasaddr = lx_fake_find_biasaddr_from_linker(filename, realpath, realmaxn);
    if (!biasaddr)
        biasaddr = lx_fake_find_biasaddr_from_maps(filename, realpath, realmaxn);
    return biasaddr;
}

// open map file
static lx_pointer_t lx_fake_open_file(lx_char_t const* filepath, lx_size_t* pfilesize)
{
    // check
    lx_assert_and_check_return_val(filepath && pfilesize, lx_null);

    // open it
    lx_int_t     fd = -1;
    lx_pointer_t filedata = lx_null;
    do
    {
        // open file
        fd = open(filepath, O_RDONLY | O_CLOEXEC);
        if (fd < 0 && errno == EINTR)
            fd = open(filepath, O_RDONLY | O_CLOEXEC);
        lx_check_break(fd > 0);

        // get file size
        struct stat st;
        if (0 != fstat(fd, &st) || 0 == st.st_size) break;

        // mmap the file data
        filedata = mmap(lx_null, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        lx_assert_and_check_break(filedata && filedata != MAP_FAILED);

        // save the file size
        if (pfilesize) *pfilesize = (lx_size_t)st.st_size;

    } while (0);

    // close the fd first
    if (fd > 0) close(fd);
    fd = -1;

    // ok?
    return filedata;
}

// get symbol address from the fake dlopen context
static lx_pointer_t lx_fake_dlsym(lx_fake_dlctx_ref_t dlctx, lx_char_t const* symbol)
{
    // check
    lx_assert_and_check_return_val(dlctx && dlctx->filedata && dlctx->filesize && symbol, lx_null);

    // find the symbol address from the .dynsym first
    lx_int_t         i = 0;
    lx_pointer_t     end = dlctx->filedata + dlctx->filesize;
    lx_char_t const* dynstr = (lx_char_t const*)dlctx->dynstr;
    ElfW(Sym)*       dynsym = (ElfW(Sym)*)dlctx->dynsym;
    lx_int_t         dynsym_num = dlctx->dynsym_num;
    if (dynsym && dynstr)
    {
        for (i = 0; i < dynsym_num; i++, dynsym++)
        {
            lx_char_t const* name = dynstr + dynsym->st_name;
            if ((lx_pointer_t)name < end && lx_strcmp(name, symbol) == 0)
            {
                /* NB: sym->st_value is an offset into the section for relocatables,
                 * but a VMA for shared libs or exe files, so we have to subtract the bias
                 */
                lx_pointer_t symboladdr = (lx_pointer_t)(dlctx->biasaddr + dynsym->st_value);
                lx_trace_d("dlsym(%s): found at .dynsym/%p = %p + %x", symbol, symboladdr, dlctx->biasaddr, (lx_int_t)dynsym->st_value);
                return symboladdr;
            }
        }
    }

    // find the symbol address from the .symtab
    lx_char_t const* strtab = (lx_char_t const*)dlctx->strtab;
    ElfW(Sym)*       symtab = (ElfW(Sym)*)dlctx->symtab;
    lx_int_t         symtab_num = dlctx->symtab_num;
    if (symtab && strtab)
    {
        for (i = 0; i < symtab_num; i++, symtab++)
        {
            lx_char_t const* name = strtab + symtab->st_name;
            if ((lx_pointer_t)name < end && lx_strcmp(name, symbol) == 0)
            {
                lx_pointer_t symboladdr = (lx_pointer_t)(dlctx->biasaddr + symtab->st_value);
                lx_trace_d("dlsym(%s): found at .symtab/%p = %p + %x", symbol, symboladdr, dlctx->biasaddr, (lx_int_t)symtab->st_value);
                return symboladdr;
            }
        }
    }
    return lx_null;
}

// close the fake dlopen context
static lx_int_t lx_fake_dlclose(lx_fake_dlctx_ref_t dlctx)
{
    // check
    lx_assert_and_check_return_val(dlctx, -1);

    // clear data
    dlctx->biasaddr   = lx_null;
    dlctx->dynsym     = lx_null;
    dlctx->dynstr     = lx_null;
    dlctx->dynsym_num = 0;
    dlctx->strtab     = lx_null;
    dlctx->symtab     = lx_null;
    dlctx->symtab_num = 0;

    // unmap file data
    if (dlctx->filedata) munmap(dlctx->filedata, dlctx->filesize);
    dlctx->filedata = lx_null;
    dlctx->filesize = 0;

    // lx_free context
    lx_free(dlctx);
    return 0;
}

/* @see https://www.sunmoonblog.com/2019/06/04/fake-dlopen/
 * https://github.com/avs333/Nougat_dlfunctions
 */
static lx_fake_dlctx_ref_t lx_fake_dlopen_impl(lx_char_t const* filename, lx_int_t flag)
{
    // check
    lx_assert_and_check_return_val(filename, lx_null);

    // do open
    lx_bool_t           ok = lx_false;
    lx_char_t           realpath[512];
    lx_fake_dlctx_ref_t dlctx = lx_null;
    do
    {
        // attempt to find the load bias address and real path
        lx_pointer_t biasaddr = lx_fake_find_biasaddr(filename, realpath, sizeof(realpath));
        lx_check_break(biasaddr);

        // init context
        dlctx = lx_malloc0_type(lx_fake_dlctx_t);
        lx_assert_and_check_break(dlctx);

        dlctx->magic    = LX_FAKE_DLCTX_MAGIC;
        dlctx->biasaddr = biasaddr;

        // open file
        dlctx->filedata = lx_fake_open_file(realpath, &dlctx->filesize);
        lx_assert_and_check_break(dlctx->filedata && dlctx->filesize);

        // trace
        lx_trace_d("fake_dlopen: biasaddr: %p, realpath: %s, filesize: %d", biasaddr, realpath, (lx_int_t)dlctx->filesize);

        // get elf
        ElfW(Ehdr)*  elf = (ElfW(Ehdr)*)dlctx->filedata;
        lx_pointer_t end = dlctx->filedata + dlctx->filesize;
        lx_assert_and_check_break((lx_pointer_t)(elf + 1) < end);

        // get .shstrtab section
        lx_pointer_t shoff = dlctx->filedata + elf->e_shoff;
        ElfW(Shdr)*  shstrtab = (ElfW(Shdr)*)(shoff + elf->e_shstrndx * elf->e_shentsize);
        lx_assert_and_check_break((lx_pointer_t)(shstrtab + 1) <= end);

        lx_pointer_t shstr = dlctx->filedata + shstrtab->sh_offset;
        lx_assert_and_check_break(shstr < end);

        // parse elf sections
        lx_int_t  i = 0;
        lx_bool_t broken = lx_false;
        for (i = 0; !broken && i < elf->e_shnum && shoff; i++, shoff += elf->e_shentsize)
        {
            // get section
            ElfW(Shdr)* sh = (ElfW(Shdr)*)shoff;
            lx_assert_and_check_break((lx_pointer_t)(sh + 1) <= end && shstr + sh->sh_name < end);
            lx_assert_and_check_break(dlctx->filedata + sh->sh_offset < end);

            // trace
            lx_trace_d("elf section(%d): type: %d, name: %s", i, sh->sh_type, shstr + sh->sh_name);

            // get .dynsym and .symtab sections
            switch(sh->sh_type)
            {
            case SHT_DYNSYM:
                // get .dynsym
                if (dlctx->dynsym)
                {
                    lx_trace_d("%s: duplicate .dynsym sections", realpath);
                    broken = lx_true;
                    break;
                }
                dlctx->dynsym     = dlctx->filedata + sh->sh_offset;
                dlctx->dynsym_num = (sh->sh_size / sizeof(ElfW(Sym)));
                lx_trace_d(".dynsym: %p %d", dlctx->dynsym, dlctx->dynsym_num);
                break;
            case SHT_SYMTAB:
                // get .symtab
                if (dlctx->symtab)
                {
                    lx_trace_d("%s: duplicate .symtab sections", realpath);
                    broken = lx_true;
                    break;
                }
                dlctx->symtab     = dlctx->filedata + sh->sh_offset;
                dlctx->symtab_num = (sh->sh_size / sizeof(ElfW(Sym)));
                lx_trace_d(".symtab: %p %d", dlctx->symtab, dlctx->symtab_num);
                break;
            case SHT_STRTAB:
                // get .dynstr
                if (!lx_strcmp(shstr + sh->sh_name, ".dynstr"))
                {
                    // .dynstr is guaranteed to be the first STRTAB
                    if (dlctx->dynstr) break;
                    dlctx->dynstr = dlctx->filedata + sh->sh_offset;
                    lx_trace_d(".dynstr: %p", dlctx->dynstr);
                }
                // get .strtab
                else if (!lx_strcmp(shstr + sh->sh_name, ".strtab"))
                {
                    if (dlctx->strtab) break;
                    dlctx->strtab = dlctx->filedata + sh->sh_offset;
                    lx_trace_d(".strtab: %p", dlctx->strtab);
                }
                break;
            default:
                break;
            }
        }
        lx_check_break(!broken && dlctx->dynstr && dlctx->dynsym);

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (dlctx) lx_fake_dlclose(dlctx);
        dlctx = lx_null;
    }
    return dlctx;
}
static lx_void_t lx_linker_init()
{
    static lx_bool_t s_inited = lx_false;
    if (!s_inited)
    {
        // we need linker mutex only for android 5.0 and 5.1
        lx_size_t apilevel = lx_rt_api_level();
        if (apilevel == __ANDROID_API_L__ || apilevel == __ANDROID_API_L_MR1__)
        {
            lx_fake_dlctx_ref_t linker = lx_fake_dlopen_impl(LX_LINKER_NAME, LX_RTLD_NOW);
            lx_trace_d("init linker: %p", linker);
            if (linker)
            {
                g_linker_mutex = (pthread_mutex_t*)lx_fake_dlsym(linker, LX_LINKER_MUTEX);
                lx_trace_d("load g_dl_mutex: %p", g_linker_mutex);
                lx_fake_dlclose(linker);
            }
        }
        s_inited = lx_true;
    }
}
static lx_fake_dlctx_ref_t lx_fake_dlopen(lx_char_t const* filename, lx_int_t flag)
{
    lx_linker_init();
    return lx_fake_dlopen_impl(filename, flag);
}
static lx_void_t lx_jni_clearException(JNIEnv* env, lx_bool_t report)
{
    jthrowable e = report? (*env)->ExceptionOccurred(env) : lx_null;
    (*env)->ExceptionClear(env);
    if (e)
    {
        jclass clazz = (*env)->GetObjectClass(env, e);
        jmethodID printStackTrace_id = (*env)->GetMethodID(env, clazz, "printStackTrace", "()V");
        if (!(*env)->ExceptionCheck(env) && printStackTrace_id)
            (*env)->CallVoidMethod(env, e, printStackTrace_id);
        if ((*env)->ExceptionCheck(env))
            (*env)->ExceptionClear(env);
    }
}
static jobject lx_jni_Class_getDeclaredMethod(JNIEnv* env)
{
    // check
    lx_assert_and_check_return_val(env, lx_null);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0) return lx_null;

    // get unreachable memory info
    jboolean check = lx_false;
    jobject  getDeclaredMethod_method = lx_null;
    do
    {
        // get class
        jclass clazz = (*env)->FindClass(env, "java/lang/Class");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && clazz);

        // get string class
        jclass string_clazz = (*env)->FindClass(env, "java/lang/String");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && string_clazz);

        // get class/array class
        jclass classarray_clazz = (*env)->FindClass(env, "[Ljava/lang/Class;");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && classarray_clazz);

        // get getDeclaredMethod id
        jmethodID getDeclaredMethod_id = (*env)->GetMethodID(env, clazz, "getDeclaredMethod", "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_id);

        // get getDeclaredMethod name
        jstring getDeclaredMethod_name = (*env)->NewStringUTF(env, "getDeclaredMethod");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_name);

        // get getDeclaredMethod args
        jobjectArray getDeclaredMethod_args = (*env)->NewObjectArray(env, 2, clazz, lx_null);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_args);

        (*env)->SetObjectArrayElement(env, getDeclaredMethod_args, 0, string_clazz);
        (*env)->SetObjectArrayElement(env, getDeclaredMethod_args, 1, classarray_clazz);

        // Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
        getDeclaredMethod_method = (jobject)(*env)->CallObjectMethod(env, clazz, getDeclaredMethod_id, getDeclaredMethod_name, getDeclaredMethod_args);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_method);

    } while (0);

    // exception? clear it
    if (check)
    {
        getDeclaredMethod_method = lx_null;
        lx_jni_clearException(env, lx_true);
    }
    return (jstring)(*env)->PopLocalFrame(env, getDeclaredMethod_method);
}

/* load library via system call
 *
 * @see http://weishu.me/2018/06/07/lx_free-reflection-above-android-p/
 * https://github.com/tiann/FreeReflection/blob/c995ef100f39c2eb2d7c344384ca06e8c13b9a4c/library/src/main/java/me/weishu/reflection/Reflection.java#L23-L34
 *
 * System.load(libraryPath)
 *
 * @code
    Method forName = Class.class.getDeclaredMethod("forName", String.class);
    Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
    Class<?> systemClass = (Class<?>)forName.invoke(null, "java.lang.System");
    Method load = (Method)getDeclaredMethod.invoke(systemClass, "load", new Class[]{String.class});
    load.invoke(systemClass, libraryPath);
 * @endcode
 *
 * System.loadLibrary(libraryName)
 *
 * @code
    Method forName = Class.class.getDeclaredMethod("forName", String.class);
    Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
    Class<?> systemClass = (Class<?>)forName.invoke(null, "java.lang.System");
    Method loadLibrary = (Method)getDeclaredMethod.invoke(systemClass, "loadLibrary", new Class[]{String.class});
    loadLibrary.invoke(systemClass, libraryName);
 * @endcode
 */
static lx_bool_t lx_jni_System_load_or_loadLibrary_from_sys(JNIEnv* env, lx_char_t const* loadName, lx_char_t const* libraryPath)
{
    // check
    lx_assert_and_check_return_val(env && loadName && libraryPath, lx_false);

    // push
    if ((*env)->PushLocalFrame(env, 20) < 0) return lx_false;

    // do load
    jboolean check = lx_false;
    do
    {
        // get getDeclaredMethod method
        jobject getDeclaredMethod_method = lx_jni_Class_getDeclaredMethod(env);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_method);

        // get class
        jclass clazz = (*env)->FindClass(env, "java/lang/Class");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && clazz);

        // get object class
        jclass object_clazz = (*env)->FindClass(env, "java/lang/Object");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && object_clazz);

        // get string class
        jclass string_clazz = (*env)->FindClass(env, "java/lang/String");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && string_clazz);

        // get system class
        jclass system_clazz = (*env)->FindClass(env, "java/lang/System");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && system_clazz);

        // get method class
        jclass method_clazz = (*env)->FindClass(env, "java/lang/reflect/Method");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && method_clazz);

        // get getDeclaredMethod_method.invoke id
        jmethodID invoke_id = (*env)->GetMethodID(env, method_clazz, "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_id);

        // get load name
        jstring load_name = (*env)->NewStringUTF(env, loadName);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_name);

        // get invoke args
        jobjectArray invoke_args = (*env)->NewObjectArray(env, 2, object_clazz, lx_null);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_args);

        // get load args
        jobjectArray load_args = (*env)->NewObjectArray(env, 1, clazz, string_clazz);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_args);

        (*env)->SetObjectArrayElement(env, invoke_args, 0, load_name);
        (*env)->SetObjectArrayElement(env, invoke_args, 1, load_args);

        // Method load = (Method)getDeclaredMethod.invoke(systemClass, "load", new Class[]{String.class});
        jobject load_method = (jobject)(*env)->CallObjectMethod(env, getDeclaredMethod_method, invoke_id, system_clazz, invoke_args);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_method);

        // load.invoke(systemClass, libraryPath)
        jstring libraryPath_jstr = (*env)->NewStringUTF(env, libraryPath);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && libraryPath_jstr);

        invoke_args = (*env)->NewObjectArray(env, 1, object_clazz, libraryPath_jstr);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_args);

        (*env)->CallObjectMethod(env, load_method, invoke_id, system_clazz, invoke_args);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)));

    } while (0);

    // exception? clear it
    if (check) lx_jni_clearException(env, lx_true);
    (*env)->PopLocalFrame(env, lx_null);
    return !check;
}
static lx_bool_t lx_jni_System_load_or_loadLibrary_from_app(JNIEnv* env, lx_char_t const* loadName, lx_char_t const* libraryPath)
{
    // check
    lx_assert_and_check_return_val(env && loadName && libraryPath, lx_false);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0) return lx_false;

    // do load
    jboolean check = lx_false;
    do
    {
        // get system class
        jclass system_clazz = (*env)->FindClass(env, "java/lang/System");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && system_clazz);

        // get load/loadLibrary id
        jmethodID load_id = (*env)->GetStaticMethodID(env, system_clazz, loadName, "(Ljava/lang/String;)V");
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_id);

        // get library path
        jstring libraryPath_jstr = (*env)->NewStringUTF(env, libraryPath);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && libraryPath_jstr);

        // load library
        (*env)->CallStaticVoidMethod(env, system_clazz, load_id, libraryPath_jstr);
        lx_assert_and_check_break(!(check = (*env)->ExceptionCheck(env)));

    } while (0);

    // exception? clear it
    if (check) lx_jni_clearException(env, lx_true);
    (*env)->PopLocalFrame(env, lx_null);
    return !check;
}

// System.load(libraryPath)
static lx_bool_t lx_jni_System_load(JNIEnv* env, lx_char_t const* libraryPath)
{
    lx_trace_d("load: %s", libraryPath);
    return lx_jni_System_load_or_loadLibrary_from_app(env, "load", libraryPath) ||
           lx_jni_System_load_or_loadLibrary_from_sys(env, "load", libraryPath);
}

// System.loadLibrary(libraryName)
static lx_bool_t lx_jni_System_loadLibrary(JNIEnv* env, lx_char_t const* libraryName)
{
    lx_trace_d("loadLibrary: %s", libraryName);
    return lx_jni_System_load_or_loadLibrary_from_app(env, "loadLibrary", libraryName) ||
           lx_jni_System_load_or_loadLibrary_from_sys(env, "loadLibrary", libraryName);
}

/* get the current jni environment
 *
 * @see frameworks/base/core/jni/include/android_runtime/AndroidRuntime.h
 *
 * static AndroidRuntime* runtime = AndroidRuntime::getRuntime();
 * static JavaVM* getJavaVM() { return mJavaVM; }
 * static JNIEnv* getJNIEnv();
 */
static JNIEnv* lx_jni_getenv()
{
    if (g_jvm)
    {
        JNIEnv* env = lx_null;
        if (JNI_OK == (*g_jvm)->GetEnv(g_jvm, (lx_pointer_t*)&env, g_jversion))
            return env;
    }
    if (!g_tls_jnienv)
    {
        lx_fake_dlctx_ref_t dlctx = lx_fake_dlopen("libandroid_runtime.so", LX_RTLD_NOW);
        if (dlctx)
        {
            typedef lx_pointer_t (*getJNIEnv_t)();
            getJNIEnv_t getJNIEnv = (getJNIEnv_t)lx_fake_dlsym(dlctx, "_ZN7android14AndroidRuntime9getJNIEnvEv");
            if (getJNIEnv)
                g_tls_jnienv = getJNIEnv();
            lx_fake_dlclose(dlctx);
        }

        // trace
        lx_trace_d("get jnienv: %p", g_tls_jnienv);
    }
    return g_tls_jnienv;
}

lx_void_t lx_jni_javavm_set(JavaVM* jvm, lx_int_t jversion)
{
    g_jvm = jvm;
    g_jversion = jversion;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_dlimage_ref_t lx_dlopen(lx_char_t const* filename, lx_int_t flag)
{
    // check
    lx_assert_and_check_return_val(filename, lx_null);

    // attempt to use original dlopen to load it fist
    // TODO we disable the original dlopen now, load /data/xxx.so may be returned an invalid address
    lx_pointer_t handle = lx_null;//dlopen(filename, flag == LX_RTLD_LAZY? RTLD_LAZY : RTLD_NOW);

    // uses the fake dlopen to load it from maps directly
    if (!handle) handle = (lx_pointer_t)lx_fake_dlopen(filename, flag);

    // uses the fake dlopen to load it from maps directly
    if (!handle)
    {
        // load it via system call
        JNIEnv* env = lx_jni_getenv();
        if (env && (((lx_strstr(filename, "/") || lx_strstr(filename, ".so")) && lx_jni_System_load(env, filename)) || lx_jni_System_loadLibrary(env, filename)))
            handle = (lx_pointer_t)lx_fake_dlopen(filename, flag);
    }
    return (lx_dlimage_ref_t)handle;
}
lx_pointer_t lx_dlsym(lx_dlimage_ref_t handle, lx_char_t const* symbol)
{
    // check
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)handle;
    lx_assert_and_check_return_val(dlctx && symbol, lx_null);

    // do dlsym
    return (dlctx->magic == LX_FAKE_DLCTX_MAGIC)? lx_fake_dlsym(dlctx, symbol) : dlsym((lx_handle_t)handle, symbol);
}
lx_int_t lx_dlclose(lx_dlimage_ref_t handle)
{
    // check
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)handle;
    lx_assert_and_check_return_val(dlctx, -1);

    // do dlclose
    return (dlctx->magic == LX_FAKE_DLCTX_MAGIC)? lx_fake_dlclose(dlctx) : dlclose((lx_handle_t)handle);
}






