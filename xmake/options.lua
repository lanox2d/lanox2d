-- tests and examples options
option("tests",    {showmenu = true, default = false, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

-- internal auto-detection options
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

-- enable small compilation mode, it will disable all optional packages and modules
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1}, description = "Enable small mode and disable all optional modules"})

-- pixfmt option
option("pixfmt",   {showmenu = true, default = "rgb565,xrgb8888,argb8888,rgbx8888,rgba8888", description = "Enable pixel formats"})

-- bitmap option
option("bitmap",   {showmenu = true, default = "bmp", description = "Enable bitmap formats"})

-- window option
option("window",   {showmenu = true, default = "sdl", description = "Set renderer window", values = {"sdl", "glut"}})

-- device option
option("device")
    set_showmenu(true)
    set_description("Set renderer device")
    set_values("bitmap", "opengl", "skia")
    add_deps("window")
    on_check(function (option)
        if not option:value() then
            local window = option:dep("window"):value()
            if window == "sdl" then
                option:set_value("bitmap")
            elseif window == "glut" then
                option:set_value("opengl")
            end
        end
    end)
    after_check(function (option)
        if option:value() then
            option:set("configvar", "LX_CONFIG_DEVICE_HAVE_" .. option:value():upper(), 1)
        end
    end)
option_end()
