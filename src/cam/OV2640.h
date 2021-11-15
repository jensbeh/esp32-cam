#include "esp_camera.h"

class OV2640 {
public:
    OV2640(){
        fb = NULL;
    };

    esp_err_t init();
    void makeFrameBuffer();
    size_t getSize();
    uint8_t *getFrameBuffer();
    int getWidth();
    int getHeight();
    framesize_t getFrameSize();
    pixformat_t getPixelFormat();
    int8_t getBrightness();

    void setFrameSize(framesize_t size);
    void setJpegQuality(int jpeg_quality);
    void setBrightness(int newBrightness);
    void setPixelFormat(pixformat_t format);

private:
    void checkForEmptyFB();

    camera_fb_t *fb;
};

// Camera pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
