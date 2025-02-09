#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"
#include "bibliotecas/ssd1306.h"
#include "bibliotecas/font.h"

const uint8_t led_red_pino = 11;
const uint8_t led_blue_pino = 12;
const uint8_t led_green_pino = 13;
volatile bool estado_red = false;
volatile bool estado_blue = false;
volatile bool estado_green = false;

const uint8_t btn_a = 5;
const uint8_t btn_b = 6;
const uint8_t btn_j = 22;

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

int main()
{
    stdio_init_all();

    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
  
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;
    while (true) {
        if(stdio_usb_connected){
            char tecla;
            if(scanf("%c", &tecla) == 1){
                cor = !cor;
                // Atualiza o conteúdo do display com animações
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
                ssd1306_draw_char(&ssd, tecla, 20, 30);
                ssd1306_send_data(&ssd); // Atualiza o display
            }
        }
    }
}
