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

void OV2640::setFrameSize(framesize_t size) {
    /*
    FRAMESIZE_UXGA (1600 x 1200)
    FRAMESIZE_QVGA (320 x 240)
    FRAMESIZE_CIF (352 x 288)
    FRAMESIZE_VGA (640 x 480)
    FRAMESIZE_SVGA (800 x 600)
    FRAMESIZE_XGA (1024 x 768)
    FRAMESIZE_SXGA (1280 x 1024)
    */
    cam_config.frame_size = size;
}
framesize_t OV2640::getFrameSize() {
    return cam_config.frame_size;
}

void OV2640::setJpegQuality(int jpeg_quality) {
    //10-63 lower number means higher quality
    cam_config.jpeg_quality = jpeg_quality;
}
int OV2640::getJpegQuality() {
    return cam_config.jpeg_quality;
}

void OV2640::setQuality(uint8_t newQuality) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, newQuality); //0 - 63
}
uint8_t OV2640::getQuality() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.quality;
}

void OV2640::setBrightness(int8_t newBrightness) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_brightness(s, newBrightness); // -2 to 2
}
int8_t OV2640::getBrightness() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.brightness;
}

void OV2640::setContrast(int8_t newContrast) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_contrast(s, newContrast); // -2 to 2
}
int8_t OV2640::getContrast() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.contrast;
}

void OV2640::setSaturation(int8_t newSaturation) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_saturation(s, newSaturation); // -2 to 2
}
int8_t OV2640::getSaturation() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.saturation;
}

void OV2640::setSharpness(int8_t newSharpness) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_sharpness(s, newSharpness); // -2 to 2
}
int8_t OV2640::getSharpness() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.sharpness;
}

void OV2640::setDenoise(uint8_t newDenoise) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_denoise(s, newDenoise); // ???
}
uint8_t OV2640::getDenoise() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.sharpness;
}

void OV2640::setSpecialEffect(uint8_t newSpecialEffect) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_special_effect(s, newSpecialEffect); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
}
uint8_t OV2640::getSpecialEffect() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.special_effect;
}

void OV2640::setWhitebal(int whitebal) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_whitebal(s, whitebal); // 0 = disable , 1 = enable
}
//int8_t OV2640::getWhitebal() {
//    sensor_t * s = esp_camera_sensor_get();
//    return s->status.whitebal;
//}

void OV2640::setWbMode(uint8_t newWbMode) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_wb_mode(s, newWbMode); // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
}
uint8_t OV2640::getWbMode() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.wb_mode;
}

void OV2640::setAwbGain(uint8_t awbGain) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_awb_gain(s, awbGain); // 0 = disable , 1 = enable
}
uint8_t OV2640::getAwbGain() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.awb_gain;
}

void OV2640::setExposureCtrl(int exposureCtrl) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_exposure_ctrl(s, exposureCtrl); // 0 = disable , 1 = enable
}
//int8_t OV2640::getExposureCtrl() {
//    sensor_t * s = esp_camera_sensor_get();
//    return s->status.exposureCtrl;
//}

void OV2640::setAec2(uint8_t aec2) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_aec2(s, aec2); // 0 = disable , 1 = enable
}
uint8_t OV2640::getAec2() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.aec2;
}

void OV2640::setAeLevel(int8_t newAeLevel) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_ae_level(s, newAeLevel); // -2 to 2
}
int8_t OV2640::getAeLevel() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.ae_level;
}

void OV2640::setAecValue(uint16_t newAecValue) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_aec_value(s, newAecValue); // 0 - 1200
}
uint16_t OV2640::getAecValue() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.aec_value;
}

void OV2640::setGainCtrl(int gainCtrl) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_gain_ctrl(s, gainCtrl); // 0 = disable , 1 = enable
}
//int8_t OV2640::getGainCtrl() {
//    sensor_t * s = esp_camera_sensor_get();
//    return s->status.gainctrl;
//}

void OV2640::setAgcGain(uint8_t newAgcGain) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_agc_gain(s, newAgcGain); // 0 - 30
}
uint8_t OV2640::getAgcGain() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.agc_gain;
}

void OV2640::setGainceiling(uint8_t newGainceiling) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_gainceiling(s, (gainceiling_t)newGainceiling);  // 0 to 6
}
uint8_t OV2640::getGainceiling() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.gainceiling;
}

void OV2640::setBpc(uint8_t bpc) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_bpc(s, bpc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getBpc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.bpc;
}

void OV2640::setWpc(uint8_t wpc) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_wpc(s, wpc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getWpc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.wpc;
}

void OV2640::setRawGma(uint8_t rawGma) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_raw_gma(s, rawGma); // 0 = disable , 1 = enable
}
uint8_t OV2640::getRawGma() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.raw_gma;
}

void OV2640::setLenc(uint8_t lenc) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_lenc(s, lenc); // 0 = disable , 1 = enable
}
uint8_t OV2640::getLenc() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.lenc;
}

void OV2640::setHmirror(uint8_t hMirror) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_hmirror(s, hMirror); // 0 = disable , 1 = enable
}
uint8_t OV2640::getHmirror() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.hmirror;
}

void OV2640::setVflip(uint8_t vFlip) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, vFlip); // 0 = disable , 1 = enable
}
uint8_t OV2640::getVflip() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.vflip;
}

void OV2640::setDcw(uint8_t dcw) {
    sensor_t * s = esp_camera_sensor_get();
    s->set_dcw(s, dcw); // 0 = disable , 1 = enable
}
uint8_t OV2640::getDcw() {
    sensor_t * s = esp_camera_sensor_get();
    return s->status.dcw;
}

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