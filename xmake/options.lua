-- tests and examples options
option("tests",    {showmenu = true, default = false, description = "Enable tests"})
option("examples", {showmenu = true, default = true, description = "Enable exmaples"})

-- internal auto-detection options
option("wchar",    {ctypes = "wchar_t", configvar = {"LX_CONFIG_TYPE_HAVE_WCHAR", 1}})

-- enable small compilation mode, it will disable all optional packages and modules
option("small",    {showmenu = true, default = true, configvar = {"LX_CONFIG_SMALL", 1}, description = "Enable small mode and disable all optional modules"})

-- pixfmt option
option("pixfmt")
    set_showmenu(true)
    set_default("rgb565,xrgb8888,argb8888,rgbx8888,rgba8888")
    set_description("Enable pixel formats")
    after_check(function (option)
        local value = option:value()
        if value then
            for _, name in ipairs(value:split(',')) do
                option:set("configvar", "LX_CONFIG_PIXFMT_HAVE_" .. name:upper(), 1)
            end
        end
    end)

-- bitmap option
option("bitmap")
    set_showmenu(true)
    set_default("bmp")
    set_values("bmp", "jpg", "png")
    set_description("Enable bitmap formats")
    after_check(function (option)
        local value = option:value()
        if value then
            for _, name in ipairs(value:split(',')) do
                option:set("configvar", "LX_CONFIG_BITMAP_HAVE_" .. name:upper(), 1)
            end
        end
    end)

-- window option
option("window")
    set_showmenu(true)
    set_default("sdl")
    set_values("sdl", "glut", "glfw")
    set_description("Set renderer window")
    after_check(function (option)
        local value = option:value()
        if is_plat("macosx") then
            if value == "glut" then
                option:add("frameworks", "GLUT", "OpenGL")
            end
            option:add("cxflags", "-Wno-error=deprecated-declarations")
        end
        if value then
            option:set("configvar", "LX_CONFIG_WINDOW_HAVE_" .. value:upper(), 1)
        end
    end)

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
            elseif window == "glut" or window == "glfw" then
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
