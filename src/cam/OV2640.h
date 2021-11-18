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
    int getJpegQuality();
    uint8_t getQuality();
    int8_t getBrightness();
    int8_t getContrast();
    int8_t getSaturation();
    int8_t getSharpness();
    uint8_t getDenoise();
    uint8_t getSpecialEffect();
    int8_t getWhitebal(); // xxx
    uint8_t getWbMode();
    uint8_t getAwbGain();
    int8_t getExposureCtrl(); // xxx
    uint8_t getAec2();
    int8_t getAeLevel();
    uint16_t getAecValue();
    int8_t getGainCtrl(); //xxx
    uint8_t getAgcGain();
    uint8_t getGainceiling();
    uint8_t getBpc();
    uint8_t getWpc();
    uint8_t getRawGma();
    uint8_t getLenc();
    uint8_t getHmirror();
    uint8_t getVflip();
    uint8_t getDcw();
    uint8_t getColorbar();

    void setFrameSize(framesize_t size);
    void setJpegQuality(int jpeg_quality);
    void setQuality(uint8_t newQuality);
    void setPixelFormat(pixformat_t format);
    void setBrightness(int8_t newBrightness);
    void setContrast(int8_t newContrast);
    void setSaturation(int8_t newSaturation);
    void setSharpness(int8_t newSharpness);
    void setDenoise(uint8_t newDenoise);
    void setSpecialEffect(uint8_t newSpecialEffect);
    void setWhitebal(int whitebal); // xxx
    void setWbMode(uint8_t newWbMode);
    void setAwbGain(uint8_t awbGain);
    void setExposureCtrl(int exposureCtrl); // xxx
    void setAec2(uint8_t aec2);
    void setAeLevel(int8_t newAeLevel);
    void setAecValue(uint16_t newAecValue);
    void setGainCtrl(int gainCtrl); // xxx
    void setAgcGain(uint8_t newAgcGain);
    void setGainceiling(uint8_t newGainceiling);
    void setBpc(uint8_t bpc);
    void setWpc(uint8_t wpc);
    void setRawGma(uint8_t rawGma);
    void setLenc(uint8_t lenc);
    void setHmirror(uint8_t hMirror);
    void setVflip(uint8_t vFlip);
    void setDcw(uint8_t dcw);
    void setColorbar(uint8_t colorbar);

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
