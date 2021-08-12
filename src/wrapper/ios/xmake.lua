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
    set_menu {usage = "xmake pod_build [options]", description = "build iphoneos libraries for pod.", options = {
        {'t', "target_minver", "kv", nil, "Set the target minver."}
    }}
    on_run(function ()
        import("core.base.option")
        import("core.project.config")
        local outputdir = path.join(config.buildir(), "iphoneos")
        -- build Lanox2d.framework for each archs
        for _, arch in ipairs({"armv7", "arm64", "x86_64"}) do
            local argv = {"f", "-p", "iphoneos", "-a", arch, "-m", "releasedbg", "-c", "-y"}
            if option.get("verbose") then
                table.insert(argv, "-v")
            end
            local target_minver = option.get("target_minver")
            if target_minver then
                table.insert(argv, "--target_minver=" .. target_minver)
            end
            os.execv(os.programfile(), argv)

            local argv = {"-r", "Lanox2d"}
            if option.get("verbose") then
                table.insert(argv, "-v")
            end
            os.execv(os.programfile(), argv)
            os.execv(os.programfile(), {"install", "-o", path.join(outputdir, arch, "releasedbg", "install"), "lanox2d"})
        end
        -- generate universal Lanox2d.framework
        local lipoargs = ""
        for _, arch in ipairs({"armv7", "arm64", "x86_64"}) do
            local targetfile = path.join(outputdir, arch, "releasedbg", "Lanox2d.framework", "Versions", "A", "Lanox2d")
            lipoargs = format("%s -arch %s %s", lipoargs, arch, targetfile)
        end
        local libraryfile = os.tmpfile()
        lipoargs = format("-create %s -output %s", lipoargs, libraryfile)
        os.execv(os.programfile(), {"l", "lipo", lipoargs})
        os.tryrm(path.join(outputdir, "universal/releasedbg/Lanox2d.framework"))
        os.vmv(path.join(outputdir, "arm64/releasedbg/Lanox2d.framework"), path.join(outputdir, "universal/releasedbg/Lanox2d.framework"))
        os.tryrm(path.join(outputdir, "armv7/releasedbg/Lanox2d.framework"))
        os.tryrm(path.join(outputdir, "x86_64/releasedbg/Lanox2d.framework"))
        os.vcp(libraryfile, path.join(outputdir, "universal/releasedbg/Lanox2d.framework/Versions/A/Lanox2d"))
        os.vcp(path.join(outputdir, "arm64", "releasedbg", "install", "include", "lanox2d"), path.join(outputdir, "universal/releasedbg/Lanox2d.framework/Headers/core/lanox2d"))
    end)
task_end()

if is_plat("iphoneos") then
    includes("Lanox2d", "Example")
end
