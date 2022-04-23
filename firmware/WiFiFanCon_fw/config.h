//defaults
char mqtt_server[80] = "example.tld";

char mqtt_username[24] = "";
char mqtt_password[24] = "";


// fanTacho
const int tachoPin1                             = GPIO_NUM_16;
const int tachoPin2                             = GPIO_NUM_18;
const int tachoPin3                             = GPIO_NUM_21;
const int tachoPin4                             = GPIO_NUM_23;
const int tachoUpdateCycle                     = 1000; // how often tacho speed shall be determined, in milliseconds
const int numberOfInterruptsInOneSingleRotation = 2; 


// fanPWM
const int pwmPin1              = GPIO_NUM_17;
const int pwmPin2              = GPIO_NUM_19;
const int pwmPin3              = GPIO_NUM_22;
const int pwmPin4              = GPIO_NUM_25;

const int pwmFreq              = 25000; //25khz according to the spec
const int pwmResolution        = 8;

// initial pwm fan speed on startup in %
const int initialDutyCycle     = 40;
