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
    add_options("small", "wchar", "window", "device", "bitmap", "pixfmt", "openglver")

    -- check interfaces
    check_interfaces()

    -- add packages
    add_packages("libsdl", "glut", "glfw", "skia", "libpng", "libjpeg-turbo")

    -- add devices
    if is_config("device", "bitmap") then
        add_files("core/device/bitmap/**.c")
    end
    if is_config("device", "opengl") then
        add_files("core/device/opengl/**.c|program/*.c")
        add_files("core/tess/**.c")
        local openglver = get_config("openglver")
        if openglver then
            local opengles = false
            if openglver:startswith("es") then
                openglver = openglver:sub(3)
                opengles = true
            end
            openglver = tonumber((openglver:gsub("%.", "")))
            if openglver > 30 then
                add_files("core/device/opengl/program/shaders/*_33.vs")
                add_files("core/device/opengl/program/shaders/*_33.fs")
            elseif openglver >= 20 then
                if opengles then
                    add_files("core/device/opengl/program/shaders/*_es20.vs")
                    add_files("core/device/opengl/program/shaders/*_es20.fs")
                else
                    add_files("core/device/opengl/program/shaders/*_21.vs")
                    add_files("core/device/opengl/program/shaders/*_21.fs")
                end
            end
            if openglver >= 20 then
                add_files("core/device/opengl/program/*.c")
            end
            add_rules("utils.bin2c", {extensions = {".vs", ".fs"}})
        end
    end
    if is_config("device", "skia") then
        set_languages("c++14")
        add_files("core/device/skia/**.cc")
    end

    -- add bitmaps
    local bitmap = get_config("bitmap")
    if not bitmap or not bitmap:find("bmp") then
        del_files("core/bitmap/bmp/*.c")
    end
    if not bitmap or not bitmap:find("png") then
        del_files("core/bitmap/png/*.c")
    end
    if not bitmap or not bitmap:find("jpg") then
        del_files("core/bitmap/jpg/*.c")
    end
