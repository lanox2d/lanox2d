target("test_jni")
    set_kind("shared")
    add_deps("lanox2d")
    add_files("**.c")
    add_defines("LX_NOMAIN_ENTRY")
    if is_config("device", "vulkan") then
        add_syslinks("android")
    end
    after_build("android", function (target)
        local symsdir = path.join("$(buildir)", "$(plat)", "syms", "$(mode)", "$(arch)")
        os.trycp(target:symbolfile(), path.join(symsdir, "libtest_jni.so"))
    end)

