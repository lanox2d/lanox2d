target("lanox2d")

    -- set basic configuration
    set_kind("$(kind)")

    -- export all symbols for windows/dll
    if is_plat("windows") and is_kind("shared") then
        add_rules("utils.symbols.export_all")
    end

    -- set the auto-generated config.h
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)")
    add_configfiles("lanox2d.config.h.in")

    -- add include directories
    add_includedirs("..", {public = true})
    add_includedirs("$(buildir)/$(plat)/$(arch)/$(mode)", {public = true})

    -- add the header files for installing
    add_headerfiles("../(lanox2d/*.h)")
    add_headerfiles("../(lanox2d/base/**.h)|libc/arch/**.h")
    add_headerfiles("../(lanox2d/prefix/**.h)")
    add_headerfiles("../(lanox2d/core/**.h)|private/**.h|pixmap/*.h|device/**.h|bitmap/*.h")
    add_headerfiles("../(lanox2d/platform/**.h)|windows/*.h")
    add_headerfiles("$(buildir)/$(plat)/$(arch)/$(mode)/lanox2d.config.h", {prefixdir = "lanox2d"})

    -- add source files
    add_files("*.c")
    add_files("base/**.c|libc/arch/**.c|platform/**.c")
    add_files("base/platform/*.c")
    add_files("core/**.c|pixmap/*.c|device/**.c|tess/**.c")
    add_files("platform/**.c|windows/*.c")

    -- add options
    add_options("small", "wchar", "window", "device")

    -- check interfaces
    check_interfaces()

    -- add packages
    if is_config("window", "sdl") then
        add_packages("libsdl")
        set_configvar("LX_CONFIG_WINDOW_HAVE_SDL", 1)
    end
    if is_config("window", "glut") then
        if is_plat("macosx") then
            add_frameworks("GLUT", "OpenGL")
            add_cxflags("-Wno-error=deprecated-declarations")
        else
            add_packages("freeglut")
        end
        set_configvar("LX_CONFIG_WINDOW_HAVE_GLUT", 1)
    end
    if is_config("device", "skia") then
        add_packages("skia")
    end

    -- add devices
    if is_config("device", "bitmap") then
        add_files("core/device/bitmap/**.c")
    end
    if is_config("device", "opengl") then
        add_files("core/device/opengl/**.c")
        add_files("core/tess/**.c")
    end

    -- add bitmaps
    local bitmap = get_config("bitmap")
    if bitmap then
        for _, name in ipairs(bitmap:split(',')) do
            set_configvar("LX_CONFIG_BITMAP_HAVE_" .. name:upper(), 1)
        end
    end
    if not bitmap or not bitmap:find("bmp") then
        del_files("core/bitmap/bmp/*.c")
    end

    -- add pixmaps
    local pixfmt = get_config("pixfmt")
    if pixfmt then
        for _, name in ipairs(pixfmt:split(',')) do
            set_configvar("LX_CONFIG_PIXFMT_HAVE_" .. name:upper(), 1)
        end
    end
