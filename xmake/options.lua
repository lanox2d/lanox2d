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
    set_default("png")
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
    set_values("sdl", "glut", "glfw", "fbdev", "android")
    set_description("Set renderer window")
    on_check(function (option)
        if not option:value() then
            if is_plat("macosx", "linux", "windows") then
                option:set_value("glfw")
            elseif is_plat("android") then
                option:set_value("android")
            end
        end
    end)
    after_check(function (option)
        local value = option:value()
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
            if window == "sdl" or window == "fbdev" then
                option:set_value("bitmap")
            elseif window == "glut" or window == "glfw" or window == "android" then
                option:set_value("opengl")
            end
        end
    end)
    after_check(function (option)
        if option:value() then
            option:set("configvar", "LX_CONFIG_DEVICE_HAVE_" .. option:value():upper(), 1)
        end
    end)

-- opengl version, e.g. 1.1, 3.3, es2.0, ...
option("openglver")
    set_showmenu(true)
    set_description("Set opengl version")
    set_values("3.3", "2.1", "es2.0", "1.1")
    add_deps("device", "window")
    on_check(function (option)
        if not option:value() then
            local device = option:dep("device"):value()
            local window = option:dep("window"):value()
            if device == "opengl" then
                if is_plat("macosx") then
                    option:set_value(window == "glfw" and "3.3" or "2.1")
                elseif is_plat("linux") then
                    option:set_value("2.1")
                elseif is_plat("windows", "mingw") then
                    option:set_value("1.1")
                elseif is_plat("android", "iphoneos") then
                    option:set_value("es2.0")
                end
            end
        end
    end)
    after_check(function (option)
        local value = option:value()
        if value then
            if value:startswith("es") then
                option:set("configvar", "LX_CONFIG_OPENGL_ES", 1)
                value = value:sub(3)
            end
            option:set("configvar", "LX_CONFIG_OPENGL_VERSION", tonumber((value:gsub("%.", ""))))
        end
    end)
option_end()
