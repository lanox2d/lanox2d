-- build iphoneos app only for test
task("ipa_build")
    set_menu {usage = "xmake ipad_build [options]", description = "build iphoneos ipa.", options = {
        {'t', "target_minver", "kv", nil, "Set the target minver."}
    }}
    on_run(function ()
        import("core.base.option")
        local target_minver = option.get("target_minver")
        if target_minver then
            os.exec("xmake f -p iphoneos -m releasedbg -c --target_minver=%s", target_minver)
        else
            os.exec("xmake f -p iphoneos -m releasedbg -c")
        end
        if option.get("verbose") then
            os.exec("xmake package -v Example")
        else
            os.exec("xmake package Example")
        end
    end)

task("ipa_install")
    set_menu {usage = "xmake ipa_install [options]", description = "install iphoneos ipa.", options = {}}
    on_run(function ()
        os.exec("ideviceinstaller -i build/iphoneos/arm64/releasedbg/Example.ipa")
    end)

task("ipa_test")
    set_menu {usage = "xmake ipa_test [options]", description = "install and test iphoneos ipa.", options = {
        {'t', "target_minver", "kv", nil, "Set the target minver."}
    }}
    on_run(function ()
        import("core.base.task")
        import("core.base.option")
        task.run("ipa_build", {target_minver = option.get("target_minver")})
        task.run("ipa_install")
        os.exec("idevicesyslog")
    end)

task("pod_build")
    set_menu {usage = "xmake pod_build [options]", description = "build pod libraries.", options = {
        {'t', "target_minver", "kv", nil, "Set the target minver."},
        {'m', "mode", "kv", nil, "Set the complation mode."},
        {'d', "device", "kv", "metal", "Set the renderer device."},
        {'p', "plat", "kv", nil, "Set the complation platform."}
    }}
    on_run(function ()
        import("core.base.option")
        import("core.project.config")
        local plat = option.get("plat")
        local outputdir = path.join(config.buildir(), plat, "install")
        local mode = option.get("mode") or "releasedbg"
        local archs = plat == "iphoneos" and {"armv7", "arm64", "x86_64"} or {"x86_64"}
        os.tryrm(outputdir)
        for _, arch in ipairs(archs) do
            local argv = {"f", "-y", "-c", "-p", plat, "-m", mode, "-a", arch}
            if option.get("verbose") then
                table.insert(argv, "-v")
            end
            local target_minver = option.get("target_minver")
            if target_minver then
                table.insert(argv, " --target_minver=" .. target_minver)
            end
            local device = option.get("device")
            if device then
                table.insert(argv, "--device=" .. device)
            end
            local packagedir = path.join(outputdir, arch, "packages")
            os.execv(os.programfile(), argv)
            os.execv(os.programfile())
            os.execv(os.programfile(), {"install", "-o", path.join(outputdir, arch), "lanox2d"})
            os.execv(os.programfile(), {"require", "--export", "--packagedir=" .. packagedir})
            for _, libfile in ipairs(os.files(path.join(packagedir, "**.a"))) do
                os.cp(libfile, path.join(outputdir, arch, "lib"))
            end
        end
    end)
task_end()

if is_plat("iphoneos", "macosx") then
    includes("Lanox2d", "Example")
end
