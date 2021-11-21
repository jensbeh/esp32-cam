#include "OV2640.h"
#include "esp_camera.h"

camera_config_t cam_config;

void setupCam() {
    cam_config.ledc_channel = LEDC_CHANNEL_0;
    cam_config.ledc_timer = LEDC_TIMER_0;
    cam_config.pin_d0 = Y2_GPIO_NUM;
    cam_config.pin_d1 = Y3_GPIO_NUM;
    cam_config.pin_d2 = Y4_GPIO_NUM;
    cam_config.pin_d3 = Y5_GPIO_NUM;
    cam_config.pin_d4 = Y6_GPIO_NUM;
    cam_config.pin_d5 = Y7_GPIO_NUM;
    cam_config.pin_d6 = Y8_GPIO_NUM;
    cam_config.pin_d7 = Y9_GPIO_NUM;
    cam_config.pin_xclk = XCLK_GPIO_NUM;
    cam_config.pin_pclk = PCLK_GPIO_NUM;
    cam_config.pin_vsync = VSYNC_GPIO_NUM;
    cam_config.pin_href = HREF_GPIO_NUM;
    cam_config.pin_sscb_sda = SIOD_GPIO_NUM;
    cam_config.pin_sscb_scl = SIOC_GPIO_NUM;
    cam_config.pin_pwdn = PWDN_GPIO_NUM;
    cam_config.pin_reset = RESET_GPIO_NUM;
    cam_config.xclk_freq_hz = 20000000;
    cam_config.pixel_format = PIXFORMAT_JPEG;


    // cam has PSRAM so we can set these ones
    cam_config.frame_size = FRAMESIZE_VGA;
    cam_config.jpeg_quality = 10;
    cam_config.fb_count = 2;
}

void OV2640::makeFrameBuffer() {
    if (fb)
        //return the frame buffer back to the driver for reuse
        esp_camera_fb_return(fb);

    fb = esp_camera_fb_get();
}

uint8_t *OV2640::getFrameBuffer() {
    checkForEmptyFB();

    return fb->buf;
}

void OV2640::checkForEmptyFB() {
    // take a frame if fb is null
    if (!fb)
        makeFrameBuffer();
}

int OV2640::getWidth() {
    checkForEmptyFB();
    return fb->width;
}

int OV2640::getHeight() {
    checkForEmptyFB();
    return fb->height;
}

size_t OV2640::getSize() {
    checkForEmptyFB();

    return fb->len;
}

void OV2640::setPixelFormat(pixformat_t format) {
    // image format can be one of the following options
    switch (format) {
        case PIXFORMAT_YUV422:
        case PIXFORMAT_GRAYSCALE:
        case PIXFORMAT_RGB565:
        case PIXFORMAT_JPEG:
            cam_config.pixel_format = format;
            break;
        default:
            cam_config.pixel_format = PIXFORMAT_RGB565;
            break;
    }
}
pixformat_t OV2640::getPixelFormat() {
    return cam_config.pixel_format;
}

// set FrameSize
void OV2640::setFrameSize(framesize_t size) {
    /*
    FRAMESIZE_UXGA (1600 x 1200) -> framesize=10
    FRAMESIZE_SXGA (1280 x 1024) -> framesize=9
    FRAMESIZE_XGA (1024 x 768) -> framesize=8
    FRAMESIZE_SVGA (800 x 600) -> framesize=7
    FRAMESIZE_VGA (640 x 480) -> framesize=6
    FRAMESIZE_CIF (352 x 288) -> framesize=5
    FRAMESIZE_QVGA (320 x 240) -> framesize=4
    FRAMESIZE_HQVGA (240 x 176) -> framesize=3
    FRAMESIZE_QQVGA (160 x 120) -> framesize=0
    */
    cam_config.frame_size = size;
}
framesize_t OV2640::getFrameSize() {
    return cam_config.frame_size;
}

void OV2640::setQuality(uint8_t newQuality) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, newQuality); //10 - 63
}
uint8_t OV2640::getQuality() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.quality;
}

// set brightness
void OV2640::setBrightness(int8_t newBrightness) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_brightness(s, newBrightness); // -2 to 2
}
int8_t OV2640::getBrightness() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.brightness;
}

// set contrast
void OV2640::setContrast(int8_t newContrast) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_contrast(s, newContrast); // -2 to 2
}
int8_t OV2640::getContrast() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.contrast;
}

// set saturation
void OV2640::setSaturation(int8_t newSaturation) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_saturation(s, newSaturation); // -2 to 2
}
int8_t OV2640::getSaturation() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.saturation;
}

// set a special effect
void OV2640::setSpecialEffect(uint8_t newSpecialEffect) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_special_effect(s, newSpecialEffect); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
}
uint8_t OV2640::getSpecialEffect() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.special_effect;
}

// set white balance state
void OV2640::setAutoWhiteBalanceState(int autoWhiteBalanceState) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_whitebal(s, autoWhiteBalanceState); // 0 = disable , 1 = enable
}

// set auto white balance gain state, when enabled WbMode disabled and vice versa 
void OV2640::setAutoWbGainState(uint8_t autoWbGainState) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_awb_gain(s, autoWbGainState); // 0 = disable , 1 = enable
}
uint8_t OV2640::getAwbGainSate() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.awb_gain;
}

// set white balance mode
void OV2640::setWbMode(uint8_t newWbMode) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_wb_mode(s, newWbMode); // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
}
uint8_t OV2640::getWbMode() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.wb_mode;
}

// set exposure control; if disabled then AecValue can be set
void OV2640::setExposureCtrlState(int exposureCtrlState) { // = AEC SENSOR
    sensor_t * s = esp_camera_sensor_get();
    s->set_exposure_ctrl(s, exposureCtrlState); // 0 = disable , 1 = enable
}

// set Exposure Value
void OV2640::setAecValue(uint16_t newAecValue) { // = Exposure Value
    sensor_t * s = esp_camera_sensor_get();
    s->set_aec_value(s, newAecValue); // 0 - 1200
}
uint16_t OV2640::getAecValue() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.aec_value;
}

// set Aec DSP aka Aec2
void OV2640::setAec2(uint8_t aec2) { //= AEC DSP
    sensor_t * s = esp_camera_sensor_get();
    s->set_aec2(s, aec2); // 0 = disable , 1 = enable
}
uint8_t OV2640::getAec2() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.aec2;
}

// set AeLevel
void OV2640::setAeLevel(int8_t newAeLevel) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_ae_level(s, newAeLevel); // -2 to 2
}
int8_t OV2640::getAeLevel() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.ae_level;
}

// set AgcCtrlState; if enabled Gain Ceiling enabled; if disabled agcGain enabled
void OV2640::setAgcCtrlState(int agcCtrlState) { // AGC
    sensor_t * s = esp_camera_sensor_get();
    s->set_gain_ctrl(s, agcCtrlState); // 0 = disable , 1 = enable
}

// 
void OV2640::setAgcGain(uint8_t newAgcGain) { // 1x - 31x aka 0 - 30
    sensor_t * s = esp_camera_sensor_get();
    s->set_agc_gain(s, newAgcGain); // 0 - 30
}
uint8_t OV2640::getAgcGain() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.agc_gain;
}

// set GainCeiling
void OV2640::setGainceiling(uint8_t newGainceiling) { // Gain Ceiling  2x - 128x aka 0 - 6
    sensor_t * s = esp_camera_sensor_get();
    s->set_gainceiling(s, (gainceiling_t)newGainceiling);  // 0 to 6
}
uint8_t OV2640::getGainceiling() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.gainceiling;
}

void OV2640::setBpc(uint8_t bpc) { // = BPC
    sensor_t * s = esp_camera_sensor_get();
    s->set_bpc(s, bpc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getBpc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.bpc;
}

void OV2640::setWpc(uint8_t wpc) { // = WPC
    sensor_t * s = esp_camera_sensor_get();
    s->set_wpc(s, wpc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getWpc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.wpc;
}

void OV2640::setRawGma(uint8_t rawGma) { // = Raw GMA
    sensor_t * s = esp_camera_sensor_get();
    s->set_raw_gma(s, rawGma); // 0 = disable , 1 = enable
}
uint8_t OV2640::getRawGma() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.raw_gma;
}

// set lens correction
void OV2640::setLenc(uint8_t lenc) { // = Lens Correction
    sensor_t * s = esp_camera_sensor_get();
    s->set_lenc(s, lenc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getLenc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.lenc;
}

// horizontal mirror
void OV2640::setHmirrorState(uint8_t hMirrorState) { // = H-Mirror
    sensor_t * s = esp_camera_sensor_get();
    s->set_hmirror(s, hMirrorState); // 0 = disable , 1 = enable
}
uint8_t OV2640::getHmirror() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.hmirror;
}

// vertical flip
void OV2640::setVflipState(uint8_t vFlipState) { // = V-Flip
    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, vFlipState); // 0 = disable , 1 = enable
}
uint8_t OV2640::getVflip() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.vflip;
}

// set a colorbar
void OV2640::setColorbar(uint8_t colorbar) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_colorbar(s, colorbar); // 0 = disable , 1 = enable
}
uint8_t OV2640::getColorbar() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.colorbar;
}


esp_err_t OV2640::init() {
    setupCam();

    esp_err_t err = esp_camera_init(&cam_config);
    if (err != ESP_OK) {
        printf("Camera init failed with error 0x%x", err);
        return err;
    }

    return ESP_OK;
}