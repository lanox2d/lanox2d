-- project settings
set_project("lanox2d")
set_xmakever("2.5.3")
set_version("1.0.1", {build = "%Y%m%d%H%M"})
set_warnings("all", "error")
set_languages("c99")

-- complation modes
add_rules("mode.release", "mode.debug", "mode.asan", "mode.minsizerel", "mode.releasedbg")
if is_mode("debug") then
    add_defines("LX_DEBUG")
end
if is_mode("releasedbg", "release", "minsizerel") then
    if is_plat("android") then
        add_shflags("-fuse-ld=lld")
        add_cxflags("-fPIC")
        add_shflags("-Wl,--build-id")
    elseif not is_plat("windows", "mingw") then
        add_cxflags("-flto")
        add_shflags("-flto")
        add_ldflags("-flto")
    end
end

-- platform settings
if is_plat("windows") then
    if is_mode("debug") then
        set_runtimes("MTd")
    else
        set_runtimes("MT")
    end
elseif is_plat("android") then
    add_syslinks("m", "c")
elseif is_plat("mingw", "msys", "cygwin") then
    add_syslinks("pthread", "m")
else
    add_syslinks("pthread", "dl", "m", "c")
end

includes("xmake", "src")
