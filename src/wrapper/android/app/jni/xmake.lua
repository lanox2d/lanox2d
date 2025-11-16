target("test_jni")
    set_kind("shared")
    add_deps("lanox2d")
    add_files("**.c")
    add_defines("LX_NOMAIN_ENTRY")
    if is_config("device", "vulkan") then
        add_syslinks("android")
    end
    add_packages("vulkan-validationlayers")
    after_build("android", function (target)
        local symsdir = path.join("$(builddir)", "$(plat)", "syms", "$(mode)", "$(arch)")
        os.trycp(target:symbolfile(), path.join(symsdir, "libtest_jni.so"))
        local validationlayers = target:pkg("vulkan-validationlayers")
        if validationlayers then
            local arch = target:arch()
            os.vcp(path.join(validationlayers:installdir(), "lib", arch, "*.so"), path.join(os.scriptdir(), "..", "libs", arch))
        end
    end)

