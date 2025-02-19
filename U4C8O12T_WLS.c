#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22     // GPIO para botão do Joystick
#define Botao_A 5          // GPIO para botão A
#define LED_RED_PIN 13     // GPIO para LED Vermelho
#define LED_GREEN_PIN 11   // GPIO para LED Verde
#define LED_BLUE_PIN 12    // GPIO para LED Azul

//Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define Botao_B 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

// Variáveis globais
bool pwm_enabled = true;
bool green_led_state = false;
uint8_t border_style = 0;

// Inicializa o PWM para um GPIO específico
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Função para desenhar a borda do display
void draw_border(ssd1306_t *ssd, uint8_t style) {
    switch (style) {
        case 0:
            ssd1306_rect(ssd, 0, 0, 128, 64, true, false);
            break;
        case 1:
            ssd1306_rect(ssd, 2, 2, 124, 60, true, false);
            break;
        case 2:
            ssd1306_rect(ssd, 4, 4, 120, 56, true, false);
            break;
        default:
            break;
    }
}

int main() {
    stdio_init_all();

    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(Botao_B);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_B);
    gpio_set_irq_enabled_with_callback(Botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa os GPIOs
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // Inicializa os LEDs
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    // Inicializa o I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Inicializa o PWM para os LEDs
    uint pwm_wrap = 4095;
    pwm_init_gpio(LED_RED_PIN, pwm_wrap);
    pwm_init_gpio(LED_BLUE_PIN, pwm_wrap);

    // Variáveis para leitura do joystick
    uint16_t adc_value_x, adc_value_y;
    bool joystick_button_state = false;
    bool button_a_state = false;

    // Posição inicial do quadrado no display
    uint8_t square_x = (128 - 8) / 2; // Centraliza no eixo X
    uint8_t square_y = (64 - 8) / 2;  // Centraliza no eixo Y

    while (true) {
        // Leitura dos valores do joystick
        adc_select_input(0); // Eixo X
        adc_value_x = adc_read();
        adc_select_input(1); // Eixo Y
        adc_value_y = adc_read();
    
        // Controle do LED Azul (eixo Y)
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_BLUE_PIN, abs(adc_value_y - 2048) * 2);
        }
    
        // Controle do LED Vermelho (eixo X)
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, abs(adc_value_x - 2048) * 2);
        }
    
        // Controle do LED Verde (botão do joystick)
        if (!gpio_get(JOYSTICK_PB) && !joystick_button_state) {
            joystick_button_state = true;
            green_led_state = !green_led_state;
            gpio_put(LED_GREEN_PIN, green_led_state);
            border_style = (border_style + 1) % 3;
        } else if (gpio_get(JOYSTICK_PB)) {
            joystick_button_state = false;
        }
    
        // Controle do Botão A (ativa/desativa PWM)
        if (!gpio_get(Botao_A) && !button_a_state) {
            button_a_state = true;
            pwm_enabled = !pwm_enabled;
            if (!pwm_enabled) {
                gpio_put(LED_RED_PIN, false);
                gpio_put(LED_GREEN_PIN, false);
                gpio_put(LED_BLUE_PIN, false);
            }
        } else if (gpio_get(Botao_A)) {
            button_a_state = false;
        }
    
        // Movimento do quadrado no display
        square_x = 60 + (adc_value_x) / 64;
        square_y = 18 + (adc_value_y) / 48;
    
        // Limites do display
        if (square_x < 0) square_x = 0;
        if (square_x > 120) square_x = 120; // 128 - 8 (tamanho do quadrado)
        if (square_y < 56) square_y = 0;
        if (square_y > 130) square_y = 56; // 64 - 8 (tamanho do quadrado)
    
        // Limpa o display e desenha o quadrado e a borda
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, square_x, square_y, 8, 8, true, true);
        draw_border(&ssd, border_style);
        ssd1306_send_data(&ssd);
    
        sleep_ms(10);
    }
}
