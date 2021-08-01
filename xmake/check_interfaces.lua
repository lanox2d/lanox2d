includes("check_cfuncs.lua")

-- get function name
--
-- sigsetjmp
-- sigsetjmp((void*)0, 0)
-- sigsetjmp{sigsetjmp((void*)0, 0);}
-- sigsetjmp{int a = 0; sigsetjmp((void*)a, a);}
--
function get_function_name(func)
    local name, code = string.match(func, "(.+){(.+)}")
    if code == nil then
        local pos = func:find("%(")
        if pos then
            name = func:sub(1, pos - 1)
        else
            name = func
        end
    end
    return name:trim()
end

-- check c functions in the given module
function check_module_cfuncs(module, includes, ...)
    for _, func in ipairs({...}) do
        local funcname = get_function_name(func)
        configvar_check_cfuncs(("LX_CONFIG_%s_HAVE_%s"):format(module:upper(), funcname:upper()), func,
            {name = module .. "_" .. funcname,
             includes = includes,
             defines = "_GNU_SOURCE=1",
             warnings = "error",
             languages = stdc})
    end
end

-- check interfaces
function check_interfaces()
    -- add the interfaces for libm
    check_module_cfuncs("libm", "math.h",
        "sincos",
        "sincosf",
        "log2",
        "log2f",
        "sqrt",
        "sqrtf",
        "acos",
        "acosf",
        "asin",
        "asinf",
        "pow",
        "powf",
        "fmod",
        "fmodf",
        "tan",
        "tanf",
        "atan",
        "atanf",
        "atan2",
        "atan2f",
        "cos",
        "cosf",
        "sin",
        "sinf",
        "exp",
        "expf")

    -- add the interfaces for libc
    check_module_cfuncs("libc", {"string.h", "stdlib.h"},
        "strtol",
        "strnlen",
        "strlcpy",
        "strncasecmp")

    -- add the interfaces for posix
    if not is_plat("windows") then
        check_module_cfuncs("posix", "dlfcn.h",  "dlopen")
        check_module_cfuncs("posix", "unistd.h", "getpagesize", "sysconf")
    end
end
