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
    pixformat_t getPixelFormat();

    void setPixelFormat(pixformat_t format);

    /////////////////////////
    // Advanced Cam Settings
    /////////////////////////
    framesize_t getFrameSize();
    uint8_t getQuality();
    int8_t getBrightness();
    int8_t getContrast();
    int8_t getSaturation();
    uint8_t getSpecialEffect();

    uint8_t getAwbGainSate();
    uint8_t getWbMode();

    uint8_t getAec2();
    int8_t getAeLevel();
    uint16_t getAecValue();
    uint8_t getAgcGain();
    uint8_t getGainceiling();
    uint8_t getBpc();
    uint8_t getWpc();
    uint8_t getRawGma();
    uint8_t getLenc();
    uint8_t getHmirror();
    uint8_t getVflip();
    uint8_t getColorbar();

    int getAutoWhiteBalanceState();
    int getExposureCtrlState();
    int getAgcCtrlState();

    void setFrameSize(framesize_t size);
    void setQuality(uint8_t newQuality);
    void setBrightness(int8_t newBrightness);
    void setContrast(int8_t newContrast);
    void setSaturation(int8_t newSaturation);
    void setSpecialEffect(uint8_t newSpecialEffect);

    void setAutoWhiteBalanceState(int autoWhiteBalanceState);
    void setAutoWbGainState(uint8_t autoWbGainState);
    void setWbMode(uint8_t newWbMode);
    
    void setExposureCtrlState(int exposureCtrlState);
    void setAec2(uint8_t aec2);
    void setAeLevel(int8_t newAeLevel);
    void setAecValue(uint16_t newAecValue);
    void setAgcCtrlState(int agcCtrlState);
    void setAgcGain(uint8_t newAgcGain);
    void setGainceiling(uint8_t newGainceiling);
    void setBpc(uint8_t bpc);
    void setWpc(uint8_t wpc);
    void setRawGma(uint8_t rawGma);
    void setLenc(uint8_t lenc);
    void setHmirrorState(uint8_t hMirrorState);
    void setVflipState(uint8_t vFlipState);
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
