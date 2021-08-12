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
        {'t', "target_minver", "kv", nil, "Set the target minver."},
        {'m', "mode", "kv", nil, "Set the complation mode."}
    }}
    on_run(function ()
        import("core.base.option")
        import("core.project.config")
        local outputdir = path.join(config.buildir(), "iphoneos")
        local packagedir = path.join(config.buildir(), "packages")
        local mode = option.get("mode") or "releasedbg"
        local configs = "-c -y -m " .. mode
        local target_minver = option.get("target_minver")
        if target_minver then
            configs = configs .. " --target_minver=" .. target_minver
        end
        local argv = {"m", "package", "-p", "iphoneos", "-f", configs, "-a", "armv7,arm64,x86_64"}
        if option.get("verbose") then
            table.insert(argv, "-v")
        end
        os.tryrm(packagedir)
        os.execv(os.programfile(), argv)
        os.execv(os.programfile(), {"require", "--export", "--packagedir=" .. packagedir})
    end)
task_end()

if is_plat("iphoneos") then
    includes("Lanox2d", "Example")
end
