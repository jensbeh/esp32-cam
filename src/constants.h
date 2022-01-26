#include "Arduino.h"

// Pins
static int FLASHLIGHT_PIN = 4;
static int MOTION_PIN = 2; // <------ define your data/GPIO pin e.g. GPIO2 = 2

// Paths
static String STREAM_PATH = "/stream";

// Controls
static String CAM_CONTROLS_PATH = "camControls/";
static String UPDATE_CAMERA_PATH = "updateCamera";
static String FACTORY_RESET_ESP_PATH = "factoryResetESP";
static String RESET_WIFI_PATH = "resetWiFi";
static String RESET_CAM_VALUES_PATH = "resetCamValues";

static String FRAMESIZE_PATH = "framesize="; // 0,3,4,5,6,7,8,9,10
static String QUALITY_PATH = "quality="; //10 - 63
static String BRIGHTNESS_PATH = "brightness="; // -2  to 2
static String CONTRAST_PATH = "contrast="; // -2 to 2
static String SATURATION_PATH = "saturation="; // -2 to 2
static String SPECIAL_EFFECT_PATH = "specialEffect="; // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
static String WHITEBALANCE_STATE_PATH = "autoWhiteBalance="; // 0 = disable , 1 = enable
static String AUTOWB_GAIN_PATH = "autoWbGain="; // 0 = disable , 1 = enable
static String WB_MODE_PATH = "wbMode="; // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
static String EXPOSURE_CTRL_STATE_PATH = "exposureCtrl="; // 0 = disable , 1 = enable
static String AEC_VALUE_PATH = "aecValue="; // 0 - 1200; only if Exposure Control is off
static String AEC2_PATH = "aec2="; // 0 = disable , 1 = enable
static String AE_LEVEL_PATH = "aeLevel="; // -2 to 2
static String AGC_CTRL_STATE_PATH = "agcCtrl="; // 0 = disable , 1 = enable
static String AGC_GAIN_PATH = "agcGain="; // 0 - 30; only if Agc Control is off
static String GAINCEILING_PATH = "gainceiling="; // 0 to 6; only if Agc Control is on
static String BPC_PATH = "bpc="; // 0 = disable , 1 = enable
static String WPC_PATH = "wpc="; // 0 = disable , 1 = enable
static String RAW_GMA_PATH = "rawGma="; // 0 = disable , 1 = enable
static String LENC_PATH = "lenc="; // 0 = disable , 1 = enable
static String HMIRROR_PATH = "hmirror="; // 0 = disable , 1 = enable
static String VFLIP_PATH = "vflip="; // 0 = disable , 1 = enable
static String COLORBAR_PATH = "colorbar="; // 0 = disable , 1 = enable
static String FLASHLIGHT_PATH = "flashlight="; // 0 = disable , 1 = enable

// Notifications
static String CAM_NOTIFICATION_PATH = "camNotification/";
static String MOTION_DETECTED_PATH = "motionDetection";