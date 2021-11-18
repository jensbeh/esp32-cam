#include "Arduino.h"

// Paths
static String STREAM_PATH = "/stream";
static String CAPTURE_PATH = "/capture";
static String TEST_PATH = "/test";

// Controls
static String CAM_CONTROLS_PATH = "camControls/";
static String QUALITY_PATH = "quality=";
static String BRIGHTNESS_PATH = "brightness=";
static String CONTRAST_PATH = "contrast="; // currently here
static String SATURATION_PATH = "saturation=";
static String SHARPNESS_PATH = "sharpness=";
static String DENOISE_PATH = "denoise=";
static String SPECIAL_EFFECT_PATH = "specialEffect=";
static String WHITEBAL_PATH = "whitebal=";
static String AWB_GAIN_PATH = "awbGain=";
static String WB_MODE_PATH = "wbMode=";
static String EXPOSURE_CTRL_PATH = "exposureCtrl=";
static String AEC2_PATH = "aec2=";
static String AE_LEVEL_PATH = "aeLevel=";
static String AEC_VALUE_PATH = "aecValue=";
static String GAIN_CTRL_PATH = "gainCtrl=";
static String AGC_GAIN_PATH = "agcGain=";
static String GAINCEILING_PATH = "gainceiling=";
static String BPC_PATH = "bpc=";
static String WPC_PATH = "wpc=";
static String RAW_GMA_PATH = "rawGma=";
static String LENC_PATH = "lenc=";
static String HMIRROR_PATH = "hmirror=";
static String VFLIP_PATH = "vflip=";
static String DCW_PATH = "dcw=";
static String COLORBAR_PATH = "colorbar=";