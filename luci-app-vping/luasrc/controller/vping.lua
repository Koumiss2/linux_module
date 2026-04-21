module("luci.controller.vping", package.seeall)

function index()
    if not nixio.fs.access("/proc/proc_ip") then
        return
    end

    entry({"admin", "network", "vping"}, cbi("vping"), _("Virtual Ping"), 90)
end
