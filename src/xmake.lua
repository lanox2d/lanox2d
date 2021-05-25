includes("check_cfuncs.lua")

-- tests and examples options
option("tests",    {showmenu = true, default = false, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

-- enable small compilation mode, it will disable all optional packages and modules
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1}, description = "Enable small mode and disable all optional modules"})

-- window option, TODO glut, x11, glfw, ..
option("window",   {showmenu = true, default = "sdl", description = "Set render window", values = {"sdl"}})

-- internal auto-detection options
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

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
end

-- libsdl package
if is_config("window", "sdl") then
    add_requires("libsdl")
end

-- all projects
includes("lanox2d", "tests", "examples")
