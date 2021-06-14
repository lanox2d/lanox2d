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
#include <jni.h>
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
#include <stdio.h>
#include <string.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the fake dlopen magic
#define LX_FAKE_DLCTX_MAGIC      (0xfaddfadd)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the dynamic library context type for fake dlopen
typedef struct lx_fake_dlctx_t_ {
    // magic, mark handle for fake dlopen
    lx_uint32_t     magic;

    // the base address of the dynamic library
    lx_pointer_t    baseaddr;

    // the bias address
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
__thread JNIEnv* g_tls_jnienv = lx_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// find the base address and real path from the maps
static lx_pointer_t lx_fake_find_maps(lx_char_t const* filename, lx_char_t* realpath, lx_size_t realmaxn) {
    lx_assert_and_check_return_val(filename && realpath && realmaxn, lx_null);

    // find it
    lx_char_t    line[512];
    lx_char_t    page_attr[10];
    lx_pointer_t baseaddr = lx_null;
    FILE* fp = fopen("/proc/self/maps", "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, filename)) {
                int       pos = 0;
                uintptr_t start = 0;
                uintptr_t offset = 0;
                // 7372a68000-7372bc1000 --xp 000fe000 fd:06 39690571                       /system/lib64/libandroid_runtime.so
                if (3 == sscanf(line, "%"SCNxPTR"-%*"SCNxPTR" %4s %"SCNxPTR" %*x:%*x %*d%n", &start, page_attr, &offset, &pos)) {
                    // check permission and offset
                    if (page_attr[0] != 'r') continue;
                    if (page_attr[3] != 'p') continue;
                    if (0 != offset) continue;

                    // get base address
                    baseaddr = (lx_pointer_t)start;

                    // get real path
                    if (filename[0] == '/') {
                        strlcpy(realpath, filename, realmaxn);
                    } else if (pos < sizeof(line)) {
                        lx_char_t* p = line + pos;
                        lx_char_t* e = p + strlen(p);
                        while (p < e && lx_isspace((lx_int_t)*p)) p++;
                        while (p < e && lx_isspace((lx_int_t)(*(e - 1)))) e--;
                        *e = '\0';
                        if (p < e) strlcpy(realpath, p, realmaxn);
                        else realpath[0] = '\0';
                    }
                    else {
                        realpath[0] = '\0';
                    }
                }
                break;
            }
        }
        fclose(fp);
    }
    return baseaddr;
}

// open map file
static lx_pointer_t lx_fake_open_file(lx_char_t const* filepath, lx_size_t* pfilesize) {
    lx_assert_and_check_return_val(filepath && pfilesize, lx_null);

    // open it
    lx_int_t     fd = -1;
    lx_pointer_t filedata = lx_null;
    do {
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
static lx_pointer_t lx_fake_dlsym(lx_fake_dlctx_ref_t dlctx, lx_char_t const* symbol) {
    lx_assert_and_check_return_val(dlctx && dlctx->filedata && dlctx->filesize && symbol, lx_null);

    // find the symbol address from the .dynsym first
    lx_int_t         i = 0;
    lx_pointer_t     end = dlctx->filedata + dlctx->filesize;
    lx_char_t const* dynstr = (lx_char_t const*)dlctx->dynstr;
    ElfW(Sym)*       dynsym = (ElfW(Sym)*)dlctx->dynsym;
    lx_int_t         dynsym_num = dlctx->dynsym_num;
    if (dynsym && dynstr) {
        for (i = 0; i < dynsym_num; i++, dynsym++) {
            lx_char_t const* name = dynstr + dynsym->st_name;
            if ((lx_pointer_t)name < end && strcmp(name, symbol) == 0 &&
                    dlctx->baseaddr + dynsym->st_value > dlctx->biasaddr) {
                /* NB: sym->st_value is an offset into the section for relocatables,
                 * but a VMA for shared libs or exe files, so we have to subtract the bias
                 */
                lx_pointer_t symboladdr = (lx_pointer_t)(dlctx->baseaddr + dynsym->st_value - dlctx->biasaddr);
                lx_trace_d("dlsym(%s): found at .dynsym/%p = %p + %x - %p", symbol, symboladdr, dlctx->baseaddr, (lx_int_t)dynsym->st_value, dlctx->biasaddr);
                return symboladdr;
            }
        }
    }

    // find the symbol address from the .symtab
    lx_char_t const* strtab = (lx_char_t const*)dlctx->strtab;
    ElfW(Sym)*       symtab = (ElfW(Sym)*)dlctx->symtab;
    lx_int_t         symtab_num = dlctx->symtab_num;
    if (symtab && strtab) {
        for (i = 0; i < symtab_num; i++, symtab++) {
            lx_char_t const* name = strtab + symtab->st_name;
            if ((lx_pointer_t)name < end && strcmp(name, symbol) == 0 &&
                    dlctx->baseaddr + symtab->st_value > dlctx->biasaddr) {
                lx_pointer_t symboladdr = (lx_pointer_t)(dlctx->baseaddr + symtab->st_value - dlctx->biasaddr);
                lx_trace_d("dlsym(%s): found at .symtab/%p = %p + %x - %p", symbol, symboladdr, dlctx->baseaddr, (lx_int_t)symtab->st_value, dlctx->biasaddr);
                return symboladdr;
            }
        }
    }
    return lx_null;
}

// close the fake dlopen context
static lx_int_t lx_fake_dlclose(lx_fake_dlctx_ref_t dlctx) {
    lx_assert_and_check_return_val(dlctx, -1);

    // clear data
    dlctx->baseaddr   = lx_null;
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

    // free context
    lx_free(dlctx);
    return 0;
}

/* @see https://www.sunmoonblog.com/2019/06/04/fake-dlopen/
 * https://github.com/avs333/Nougat_dlfunctions
 */
static lx_fake_dlctx_ref_t lx_fake_dlopen(lx_char_t const* filename, lx_int_t flag) {
    lx_assert_and_check_return_val(filename, lx_null);

    // do open
    lx_bool_t           ok = lx_false;
    lx_char_t           realpath[512];
    lx_fake_dlctx_ref_t dlctx = lx_null;
    do {
        // attempt to find the base address and real path from the maps first
        lx_pointer_t baseaddr = lx_fake_find_maps(filename, realpath, sizeof(realpath));
        lx_check_break(baseaddr);

        // init context
        dlctx = lx_malloc0_type(lx_fake_dlctx_t);
        lx_assert_and_check_break(dlctx);

        dlctx->magic    = LX_FAKE_DLCTX_MAGIC;
        dlctx->baseaddr = baseaddr;

        // open file
        dlctx->filedata = lx_fake_open_file(realpath, &dlctx->filesize);
        lx_assert_and_check_break(dlctx->filedata && dlctx->filesize);

        // trace
        lx_trace_d("fake_dlopen: baseaddr: %p, realpath: %s, filesize: %d", baseaddr, realpath, (lx_int_t)dlctx->filesize);

        // get elf
        ElfW(Ehdr)*  elf = (ElfW(Ehdr)*)dlctx->filedata;
        lx_pointer_t end = dlctx->filedata + dlctx->filesize;
        lx_assert_and_check_break((lx_pointer_t)(elf + 1) < end);

        // get load bias from the program header
        lx_int_t     i = 0;
        lx_bool_t    bias_found = lx_false;
        lx_pointer_t phoff = dlctx->filedata + elf->e_phoff;
        for (i = 0; i < elf->e_phnum && phoff; i++, phoff += elf->e_phentsize) {
            ElfW(Phdr)* ph = (ElfW(Phdr)*)phoff;
            lx_assert_and_check_break((lx_pointer_t)(ph + 1) <= end);

            if ((PT_LOAD == ph->p_type) && (0 == ph->p_offset)) {
                dlctx->biasaddr = (lx_pointer_t)ph->p_vaddr;
                bias_found = lx_true;
                break;
            }
        }
        lx_assert_and_check_break(bias_found);

        // get .shstrtab section
        lx_pointer_t shoff = dlctx->filedata + elf->e_shoff;
        ElfW(Shdr)*  shstrtab = (ElfW(Shdr)*)(shoff + elf->e_shstrndx * elf->e_shentsize);
        lx_assert_and_check_break((lx_pointer_t)(shstrtab + 1) <= end);

        lx_pointer_t shstr = dlctx->filedata + shstrtab->sh_offset;
        lx_assert_and_check_break(shstr < end);

        // parse elf sections
        lx_bool_t    broken = lx_false;
        for (i = 0; !broken && i < elf->e_shnum && shoff; i++, shoff += elf->e_shentsize) {
            // get section
            ElfW(Shdr)* sh = (ElfW(Shdr)*)shoff;
            lx_assert_and_check_break((lx_pointer_t)(sh + 1) <= end && shstr + sh->sh_name < end);
            lx_assert_and_check_break(dlctx->filedata + sh->sh_offset < end);

            // trace
            lx_trace_d("elf section(%d): type: %d, name: %s", i, sh->sh_type, shstr + sh->sh_name);

            // get .dynsym and .symtab sections
            switch(sh->sh_type) {
            case SHT_DYNSYM:
                // get .dynsym
                if (dlctx->dynsym) {
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
                if (dlctx->symtab) {
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
                if (!strcmp(shstr + sh->sh_name, ".dynstr")) {
                    // .dynstr is guaranteed to be the first STRTAB
                    if (dlctx->dynstr) break;
                    dlctx->dynstr = dlctx->filedata + sh->sh_offset;
                    lx_trace_d(".dynstr: %p", dlctx->dynstr);
                } else if (!strcmp(shstr + sh->sh_name, ".strtab")) {
                    // get .strtab
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

    if (!ok && dlctx) {
        lx_fake_dlclose(dlctx);
        dlctx = lx_null;
    }
    return dlctx;
}

static lx_void_t lx_jni_clearException(JNIEnv* env, lx_bool_t report) {
    jthrowable e = report? (*env)->ExceptionOccurred(env) : lx_null;
    (*env)->ExceptionClear(env);
    if (e) {
        jclass clazz = (*env)->GetObjectClass(env, e);
        jmethodID printStackTrace_id = (*env)->GetMethodID(env, clazz, "printStackTrace", "()V");
        if (!(*env)->ExceptionCheck(env) && printStackTrace_id)
            (*env)->CallVoidMethod(env, e, printStackTrace_id);
        if ((*env)->ExceptionCheck(env))
            (*env)->ExceptionClear(env);
    }
}

static jobject lx_jni_Class_getDeclaredMethod(JNIEnv* env) {
    lx_assert_and_check_return_val(env, lx_null);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0) return lx_null;

    // get unreachable memory info
    jboolean check = lx_false;
    jobject  getDeclaredMethod_method = lx_null;
    do {
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
    if (check) {
        getDeclaredMethod_method = lx_null;
        lx_jni_clearException(env, lx_true);
    }
    return (jstring)(*env)->PopLocalFrame(env, getDeclaredMethod_method);
}

/* load library via system call
 *
 * @see http://weishu.me/2018/06/07/free-reflection-above-android-p/
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
static lx_bool_t lx_jni_System_load_or_loadLibrary_from_sys(JNIEnv* env, lx_char_t const* loadName, lx_char_t const* libraryPath) {
    lx_assert_and_check_return_val(env && loadName && libraryPath, lx_false);

    // push
    if ((*env)->PushLocalFrame(env, 20) < 0) return lx_false;

    // do load
    jboolean check = lx_false;
    do {
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

static lx_bool_t lx_jni_System_load_or_loadLibrary_from_app(JNIEnv* env, lx_char_t const* loadName, lx_char_t const* libraryPath) {
    lx_assert_and_check_return_val(env && loadName && libraryPath, lx_false);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0) return lx_false;

    // do load
    jboolean check = lx_false;
    do {
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
static lx_bool_t lx_jni_System_load(JNIEnv* env, lx_char_t const* libraryPath) {
    lx_trace_d("load: %s", libraryPath);
    return lx_jni_System_load_or_loadLibrary_from_app(env, "load", libraryPath) ||
           lx_jni_System_load_or_loadLibrary_from_sys(env, "load", libraryPath);
}

// System.loadLibrary(libraryName)
static lx_bool_t lx_jni_System_loadLibrary(JNIEnv* env, lx_char_t const* libraryName) {
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
static JNIEnv* lx_jni_getenv() {
    if (!g_tls_jnienv) {
        lx_fake_dlctx_ref_t dlctx = lx_fake_dlopen("libandroid_runtime.so", LX_RTLD_NOW);
        if (dlctx) {
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_dlimage_ref_t lx_dlopen(lx_char_t const* filename, lx_int_t flag) {
    lx_assert_and_check_return_val(filename, lx_null);

    // attempt to use original dlopen to load it fist
    // TODO we disable the original dlopen now, load /data/xxx.so may be returned an invalid address
    lx_pointer_t handle = lx_null;//dlopen(filename, flag == LX_RTLD_LAZY? RTLD_LAZY : RTLD_NOW);

    // uses the fake dlopen to load it from maps directly
    if (!handle) handle = (lx_pointer_t)lx_fake_dlopen(filename, flag);

    // uses the fake dlopen to load it from maps directly
    if (!handle) {
        // load it via system call
        JNIEnv* env = lx_jni_getenv();
        if (env && (((strstr(filename, "/") || strstr(filename, ".so")) && lx_jni_System_load(env, filename)) || lx_jni_System_loadLibrary(env, filename)))
            handle = (lx_pointer_t)lx_fake_dlopen(filename, flag);
    }
    return (lx_dlimage_ref_t)handle;
}

lx_pointer_t lx_dlsym(lx_dlimage_ref_t dlimage, lx_char_t const* symbol) {
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)dlimage;
    lx_assert_and_check_return_val(dlctx && symbol, lx_null);

    // do dlsym
    return (dlctx->magic == LX_FAKE_DLCTX_MAGIC)? lx_fake_dlsym(dlctx, symbol) : dlsym((lx_pointer_t)dlimage, symbol);
}

lx_int_t lx_dlclose(lx_dlimage_ref_t dlimage) {
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)dlimage;
    lx_assert_and_check_return_val(dlctx, -1);

    // do dlclose
    return (dlctx->magic == LX_FAKE_DLCTX_MAGIC)? lx_fake_dlclose(dlctx) : dlclose((lx_pointer_t)dlimage);
}

