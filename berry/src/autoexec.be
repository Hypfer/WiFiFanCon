import Fan
var _hct = global.hct

print("Starting WiFiFanCon Tasmota Berry based Firmware");
print("")

if _hct == nil
    print("HCT not installed. MQTT features will be unavailable")
    print("Installation instructions: https://github.com/fmtr/hct")
    print("")
else
    _hct.Button(        
        'Restart Tasmota',
        nil,
        'mdi:restart',
        _hct.CallbackIn(/value->tasmota.cmd("restart 1"))
    )
end

var configuredFans = 0
var configuredCounters = tasmota.cmd("counter")

if configuredCounters.has("Counter1")
    configuredFans = configuredCounters.size()
end

if configuredFans < 1
    print("No fans configured")
else
    print(str(configuredFans) + " fan(s) configured")
    
    for i: 1..tasmota.cmd("counter").size()
        tasmota.add_driver(Fan(i))
    end
end

