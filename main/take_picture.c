/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

// =============================== SETUP ======================================

// 1. Board setup (Uncomment):
// #define BOARD_WROVER_KIT
// #define BOARD_ESP32CAM_AITHINKER

/**
 * 2. Kconfig setup
 *
 * If you have a Kconfig file, copy the content from
 *  https://github.com/espressif/esp32-camera/blob/master/Kconfig into it.
 * In case you haven't, copy and paste this Kconfig file inside the src directory.
 * This Kconfig file has definitions that allows more control over the camera and
 * how it will be initialized.
 */

/**
 * 3. Enable PSRAM on sdkconfig:
 *
 * CONFIG_ESP32_SPIRAM_SUPPORT=y
 *
 * More info on
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-esp32-spiram-support
 */

// ================================ CODE ======================================

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_task_wdt.h"  
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"

#define BOARD_WROVER_KIT 1

// WROVER-KIT PIN Map
#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 13// XC
#define CAM_PIN_SIOD 4 // SDA
#define CAM_PIN_SIOC 5 // SCL 

#define CAM_PIN_D7 18
#define CAM_PIN_D6 12
#define CAM_PIN_D5 10
#define CAM_PIN_D4 8
#define CAM_PIN_D3 9
#define CAM_PIN_D2 11
#define CAM_PIN_D8 17
#define CAM_PIN_D9 16
#define CAM_PIN_VSYNC 6 // VSYNC
#define CAM_PIN_HSYNC 7 // HSYNC
#define CAM_PIN_PCLK 15// pixel clock

#endif

#define CHECK(function) if (function != ESP_OK) { printf("Error in " #function "\n"); return; }

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

static const char *TAG = "example:take_picture";

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D9,
    .pin_d6 = CAM_PIN_D8,
    .pin_d5 = CAM_PIN_D7,
    .pin_d4 = CAM_PIN_D6,
    .pin_d3 = CAM_PIN_D5,
    .pin_d2 = CAM_PIN_D4,
    .pin_d1 = CAM_PIN_D3,
    .pin_d0 = CAM_PIN_D2,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HSYNC,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 24000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_GRAYSCALE, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_QVGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 63, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera(void)
{
    //initialize the camera
    ESP_LOGI(TAG, "In Init Camera");
    // print free heap size
    ESP_LOGI(TAG, "Free heap size: %lu", esp_get_free_heap_size());
    // print psram size
    
    ESP_LOGI(TAG, "PSRAM size: %d", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    // rprint image size
    ESP_LOGI(TAG, "Image size: %d", camera_config.frame_size);
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    ESP_LOGI(TAG, "Camera Init Succeeded");
    return ESP_OK;
}
#endif


void senf_photo(uint8_t *data, size_t len)
{
    // send photo
    uart_write_bytes(UART_NUM_0, (const char *)data, len);
    ESP_LOGI(TAG, "Photo sent");
}


void app_main(void)
{
#if ESP_CAMERA_SUPPORTED
    vTaskDelay(1000 / portTICK_RATE_MS);

    // print started app
    if(ESP_OK != init_camera()) {

        ESP_LOGI(TAG, "Current pixel format: %d", camera_config.pixel_format);
        ESP_LOGE(TAG, "Camera Init Failed");
        // wait 5 seconds
        vTaskDelay(5000 / portTICK_RATE_MS);
        return;
    }

    while (1)
    {
        // wait 1 second
        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu byites,\n Now sending...", pic->len);

        if (!pic) {
            ESP_LOGE(TAG, "Failed to capture photo");
            return;
        } else {
            send_photo(pic->buf, pic->len);
            ESP_LOGI(TAG, "Picture sent: %d ", pic->len);
            // wait 5 seconds
            vTaskDelay(5000 / portTICK_RATE_MS);

        }
        // use pic->buf to access the image

        esp_camera_fb_return(pic);

        vTaskDelay(1000 / portTICK_RATE_MS);
        break;
    }
#else
    ESP_LOGE(TAG, "Camera support is not available for this chip");
    return;
#endif
}
