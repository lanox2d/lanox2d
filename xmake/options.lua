-- tests and examples options
option("tests",    {showmenu = true, default = false, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

-- enable small compilation mode, it will disable all optional packages and modules
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1}, description = "Enable small mode and disable all optional modules"})

-- pixfmt option
option("pixfmt",   {showmenu = true, default = "rgb565,xrgb8888,argb8888", description = "Enable pixel formats"})

-- bitmap option
option("bitmap",   {showmenu = true, default = "bmp", description = "Enable bitmap formats"})

-- window option, TODO glut, x11, glfw, ..
option("window",   {showmenu = true, default = "sdl", description = "Set render window", values = {"sdl", "glut"}})

-- internal auto-detection options
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

