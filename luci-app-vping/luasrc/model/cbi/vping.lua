local fs = require "nixio.fs"
local util = require "luci.util"

local function read_proc_ip()
    local content = fs.readfile("/proc/proc_ip") or ""
    return content:match("IPv4:%s*([0-9%.]+)") or ""
end

m = SimpleForm("vping", translate("Virtual Ping"))
m.reset = false
m.submit = true

m.description = translate("Configure the virtual ping IP address directly through /proc/proc_ip.")

local current_ip = read_proc_ip()

local current = m:field(DummyValue, "_current", translate("Current IPv4"))
current.rawhtml = true
current.default = "<code>" .. util.pcdata(current_ip ~= "" and current_ip or translate("not available")) .. "</code>"

local ip = m:field(Value, "ipaddr", translate("IPv4 address"))
ip.datatype = "ip4addr"
ip.rmempty = false
ip.default = current_ip

local result = m:field(DummyValue, "_result", translate("Last operation"))
result.rawhtml = true
result.default = "<em>" .. util.pcdata(translate("No changes applied yet.")) .. "</em>"

function m.handle(self, state, data)
    if state == FORM_VALID and data.ipaddr then
        local ok = fs.writefile("/proc/proc_ip", data.ipaddr .. "\n")

        if ok then
            local applied = read_proc_ip()
            current.default = "<code>" .. util.pcdata(applied ~= "" and applied or data.ipaddr) .. "</code>"
            result.default = "<span style=\"color:green\">" ..
                util.pcdata(translate("Value written to /proc/proc_ip")) ..
                "</span>"
        else
            self.errmessage = translate("Unable to write to /proc/proc_ip. Make sure the kernel module is loaded.")
            result.default = "<span style=\"color:red\">" ..
                util.pcdata(translate("Write failed")) ..
                "</span>"
        end
    end

    return true
end

return m
