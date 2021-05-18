-- tests and examples options
option("tests",    {showmenu = true, default = false, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

-- enable small compilation mode, it will disable all optional packages and modules
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1}, description = "Enable small mode and disable all optional modules"})

-- window option, TODO glut, x11, glfw, ..
local window = "sdl"
option("window", {showmenu = true, default = window, description = "Set render window", values = {"sdl"}})

-- internal auto-detection options
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

-- libsdl package
if is_config("window", "sdl") then
    add_requires("libsdl")
    if has_package("libsdl") then
        set_configvar("LX_CONFIG_WINDOW_HAVE_SDL", 1)
    end
end

-- all projects
includes("lanox2d", "tests", "examples")
