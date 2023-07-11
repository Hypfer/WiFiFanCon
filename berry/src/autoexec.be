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
var configuredPWMs = tasmota.cmd("pwm")

if configuredPWMs.has("PWM")
    configuredFans = configuredPWMs["PWM"].size()
end

if configuredFans < 1
    print("No fans configured")
else
    print(str(configuredFans) + " fan(s) configured")
    
    for i: 1..configuredFans
        tasmota.add_driver(Fan(i))
    end
end

