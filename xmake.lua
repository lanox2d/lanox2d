-- project settings
set_project("lanox2d")
set_xmakever("2.7.3")
set_version("1.0.1", {build = "%Y%m%d%H%M"})
set_warnings("all", "error")
set_languages("c99")
set_policy("package.requires_lock", true)

-- complation modes
add_rules("mode.release", "mode.debug", "mode.asan", "mode.minsizerel", "mode.releasedbg")
if is_mode("debug", "asan") then
    add_defines("LX_DEBUG")
end
if is_mode("releasedbg", "release", "minsizerel") then
    if not is_plat("windows", "mingw") then
        set_policy("build.optimization.lto", true)
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
    if is_arch("arm64-v8a") then -- for r20b, lld is default linker for r22b
        add_shflags("-fuse-ld=lld")
    end
    add_cxflags("-fPIC")
    add_shflags("-Wl,--build-id")
    add_syslinks("m", "c")
elseif is_plat("mingw", "msys", "cygwin") then
    add_syslinks("pthread", "m")
elseif is_plat("iphoneos") then
    add_cxflags("-fembed-bitcode-marker")
    add_mxflags("-fembed-bitcode-marker")
else
    add_syslinks("pthread", "dl", "m", "c")
end

-- toolchain settings
--[[
if is_plat("android") then
    add_requires("ndk")
    set_toolchains("@ndk", {sdkver = "21"})
end]]

includes("xmake", "src")
