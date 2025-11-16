target("lanox2d_jni")
    set_kind("shared")
    add_deps("lanox2d")
    add_files("*.c")
    if is_config("device", "vulkan") then
        add_syslinks("android")
    end
    after_build("android", function (target)
        local symsdir = path.join("$(builddir)", "$(plat)", "syms", "$(mode)", "$(arch)")
        os.trycp(target:symbolfile(), path.join(symsdir, "liblanox2d_jni.so"))
    end)

