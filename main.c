#include <stdio.h>
// #include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


unsigned short map_range(unsigned short adc_read, unsigned short adc_range_min, unsigned short adc_range_max);

void app_main() {

    // 
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    unsigned short adc_value_ch0 = 0;
    unsigned short adc_value_ch3 = 0;

    const int pwn_gpios[8] = { 0, 4, 15, 12, 14, 27, 26, 25 };

    const unsigned short adc_range_min_led_N = 1810;   // OK
    const unsigned short adc_range_max_led_N = 2860;   // OK

    const unsigned short adc_range_min_led_E = 2180;   // OK
    const unsigned short adc_range_max_led_E = 3450;   // OK

    const unsigned short adc_range_min_led_S = 1390;   // OK
    const unsigned short adc_range_max_led_S = 500;    // OK

    const unsigned short adc_range_min_led_W = 1930;   // OK
    const unsigned short adc_range_max_led_W = 750;    // OK

    unsigned short pwm_duty_N;
    unsigned short pwm_duty_E;
    unsigned short pwm_duty_S;
    unsigned short pwm_duty_W;

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_config);

    for(int i = 0; i < 8; i++) {
        ledc_channel_config_t channel_config = {
            .channel = LEDC_CHANNEL_0 + i,
            .duty = 0,
            .gpio_num = pwn_gpios[i],
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_sel = LEDC_TIMER_0,
            .hpoint = 0
        };
        ledc_channel_config(&channel_config);
    }


    while (1) {
        adc_value_ch0 = adc1_get_raw(ADC1_CHANNEL_0);
        adc_value_ch3 = adc1_get_raw(ADC1_CHANNEL_3);

        pwm_duty_N = map_range(adc_value_ch3, adc_range_min_led_N, adc_range_max_led_N);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pwm_duty_N);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

        pwm_duty_E = map_range(adc_value_ch0, adc_range_min_led_E, adc_range_max_led_E);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, pwm_duty_E);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

        pwm_duty_S = map_range(adc_value_ch3, adc_range_min_led_S, adc_range_max_led_S);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4, pwm_duty_S);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_4);

        pwm_duty_W = map_range(adc_value_ch0, adc_range_min_led_W, adc_range_max_led_W);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_6, pwm_duty_W);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_6);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, (pwm_duty_N + pwm_duty_E) / 2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, (pwm_duty_S + pwm_duty_E) / 2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5, (pwm_duty_S + pwm_duty_W) / 2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_5);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_7, (pwm_duty_N + pwm_duty_W) / 2);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_7);

        // printf("adc_value_ch0 (L/R) = %d ", adc_value_ch0);
        // printf("\tadc_value_ch3 (U/D) = %d\n", adc_value_ch3);
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // gpio_reset_pin(GPIO_NUM_2);
    // gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    // gpio_set_level(GPIO_NUM_2, 1);

}

unsigned short map_range(unsigned short adc_read, unsigned short adc_range_min, unsigned short adc_range_max) {
    if (adc_range_min < adc_range_max) {
        if (adc_read < adc_range_min) {
            adc_read = adc_range_min;
        } 
        else if (adc_read > adc_range_max) {
            adc_read = adc_range_max;
        }
    } 
    else {
        if (adc_read > adc_range_min) {
            adc_read = adc_range_min;
        } 
        else if (adc_read < adc_range_max) {
            adc_read = adc_range_max;
        }
    }
    return (adc_read - adc_range_min) * 1023 / (adc_range_max - adc_range_min);
}