import webserver
import string

var updateInterval = "*/30 * * * * *"
var hctSpeedGetterID = "speed_mqtt_getter"

class Fan : Driver
    var id
    var lastRPM
    
    var web_argName
    var web_speedInputElemID
    var web_speedInfoElemID
    var web_rpmInfoElemID
    var web_lastSpeed
    var web_lastRPM
    
    var hct_fanSpeed
    var hct_fanRPM
    
    def init(id)
        print("Init Fan " + str(id))
        
        self.id = id
        
        self.lastRPM = 0
       
        # reset
        tasmota.cmd("counter" + str(self.id) + " 0");
        
        tasmota.add_cron(updateInterval, /-> self.update_rpm(), "RPM_UPDATE_FAN_" + str(self.id))
        
        
        self.web_argName = 'm_fan_' + str(self.id) + 'speed_req'
        self.web_speedInputElemID = 'fan_speed_input_' + str(self.id)
        self.web_speedInfoElemID = 'fan_speed_info_' + str(self.id)
        self.web_rpmInfoElemID = 'fan_rpm_info_' + str(self.id)
        self.web_lastSpeed = self.get_speed()
        self.web_lastRPM = self.lastRPM
        
        
        self.setup_mqtt()
    end
    
    def setup_mqtt()
        var _hct = global.hct
        var systemId = string.tolower(tasmota.cmd("DeviceName")["DeviceName"])
        systemId = string.replace(systemId, " ", "_")
        
        if _hct == nil
            # No HCT => Nothing to do
            
            return
        end
        
        def speed_getter()
            return self.get_speed()
        end
        
        def rpm_getter()
            return self.lastRPM
        end
    
        self.hct_fanSpeed = _hct.Number(
            "Fan " + str(self.id),
            0..1000,                
            'slider',                # Input type
            nil,                     # Step size
            nil,                     # Unit of measure
            systemId + "_" + str(self.id) + "_speed",     # Entity ID
            'mdi:fan',
            [
                _hct.CallbackIn(
                    /value->self.set_speed(value)
                ),
                _hct.CallbackOut(updateInterval, speed_getter, nil, hctSpeedGetterID, nil, _hct.tools_be.callbacks.Cron)
            ]
        )
        
        self.hct_fanRPM = _hct.Sensor(   
            "Fan " + str(self.id) + " RPM",    
            'rpm',
            nil,
            systemId + "_" + str(self.id) + "_rpm",
            'mdi:speedometer',
            [
                _hct.CallbackOut(updateInterval, rpm_getter, nil, nil, nil, _hct.tools_be.callbacks.Cron)
            ]
        )
    end
    
    def get_speed()
        return tasmota.cmd("pwm")['PWM']['PWM' + str(self.id)]
    end
    
    def set_speed(value)
        tasmota.cmd("pwm" + str(self.id) +" " + str(value))
        
        if self.hct_fanSpeed != nil
            try
              self.hct_fanSpeed.registry.get(hctSpeedGetterID).function()
            except .. as e,m
              # intentional
            end
        end
    end
    
    def update_rpm()
        var currentRPM = tasmota.cmd("counter" + str(self.id))['Counter' + str(self.id)]; 
        # 2 counts per rotation. Assuming a 30s update interval we can take the value as-is
        
        # reset
        tasmota.cmd("counter" + str(self.id) + " 0");
        
        self.lastRPM = currentRPM
        
        return currentRPM
    end
    
    def web_add_main_button()        
        var currentSpeed = str(self.get_speed())      
    
        webserver.content_send(
            "<div>"
            "<h3>Fan " + str(self.id) + "</h3>"
            '<input id="' + self.web_speedInputElemID + '" type="range" min="0" max="1000" value="' 
            + currentSpeed + '"'
            ' onmouseup="la(\'&' + self.web_argName + '=\' + this.value)"'
            ' oninput="document.getElementById(\'' + self.web_speedInfoElemID + '\').innerText = this.value">'
            "<p>"
            'LVL: <span id="' + self.web_speedInfoElemID + '">' + currentSpeed + "</span>/1000 "
            'RPM: <span id="' + self.web_rpmInfoElemID + '">' + str(self.lastRPM) + "</span>"
            "</p>"
            "</div>"
        )
    end

    
    def web_sensor()
        var currentSpeed = 0
        
        if webserver.has_arg(self.web_argName)
          var requestedSpeed = webserver.arg(self.web_argName)
          
          self.set_speed(requestedSpeed)
        end
        
        # After(!) any set_speed calls
        currentSpeed = str(self.get_speed())
        
        
        if self.web_lastSpeed != currentSpeed
            self.web_lastSpeed = currentSpeed
            
            webserver.content_send(
                '<style onload="'
                'document.getElementById(\'' + self.web_speedInputElemID + '\').value = ' + str(currentSpeed) + ';'
                'document.getElementById(\'' + self.web_speedInfoElemID + '\').innerText = ' + str(currentSpeed) + ';'
                '"></style>'
            )
        end
        
        if self.web_lastRPM != self.lastRPM
            self.web_lastRPM = self.lastRPM
            
            webserver.content_send(
                '<style onload="'
                'document.getElementById(\'' + self.web_rpmInfoElemID + '\').innerText = ' + str(self.lastRPM) + ';'
                '"></style>'
            )
        end
    end
end

return Fan