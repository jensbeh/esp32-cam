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

framesize_t OV2640::getFrameSize() {
    return cam_config.frame_size;
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

void OV2640::setJpegQuality(int jpeg_quality) {
    //10-63 lower number means higher quality
    cam_config.jpeg_quality = jpeg_quality;
}

pixformat_t OV2640::getPixelFormat() {
    return cam_config.pixel_format;
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

/*
sensor_t * s = esp_camera_sensor_get();
s->set_brightness(s, 0);     // -2 to 2
s->set_contrast(s, 0);       // -2 to 2
s->set_saturation(s, 0);     // -2 to 2
s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
s->set_aec2(s, 0);           // 0 = disable , 1 = enable
s->set_ae_level(s, 0);       // -2 to 2
s->set_aec_value(s, 300);    // 0 to 1200
s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
s->set_agc_gain(s, 0);       // 0 to 30
s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
s->set_bpc(s, 0);            // 0 = disable , 1 = enable
s->set_wpc(s, 1);            // 0 = disable , 1 = enable
s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
s->set_lenc(s, 1);           // 0 = disable , 1 = enable
s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
s->set_vflip(s, 0);          // 0 = disable , 1 = enable
s->set_dcw(s, 1);            // 0 = disable , 1 = enable
s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
*/

esp_err_t OV2640::init() {
    setupCam();

    esp_err_t err = esp_camera_init(&cam_config);
    if (err != ESP_OK) {
        printf("Camera init failed with error 0x%x", err);
        return err;
    }

    return ESP_OK;
}