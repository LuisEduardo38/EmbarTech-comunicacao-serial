#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "hardware/i2c.h"
#include "bibliotecas/ssd1306.h"
#include "bibliotecas/font.h"

const uint8_t led_red_pino = 13;
const uint8_t led_blue_pino = 12;
const uint8_t led_green_pino = 11;
volatile bool estado_red = false;
volatile bool estado_blue = false;
volatile bool estado_green = false;

const uint8_t btn_a = 5;
const uint8_t btn_b = 6;
const uint8_t btn_j = 22;
volatile bool estado_btn_a = false;
volatile bool estado_btn_b = false;


#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

volatile uint32_t ultimo_tempo = 0;

void iniciar_pinos();
void gpio_irq_handler(uint gpio, uint32_t events);

int main()
{
    stdio_init_all();
    iniciar_pinos();

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

    gpio_set_irq_enabled_with_callback(btn_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btn_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btn_j, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    bool cor = true;
    while (true) {
        if(stdio_usb_connected){
            char tecla;
            if(scanf("%c", &tecla) == 1){
                cor = !cor;
                ssd1306_fill(&ssd, !cor);
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
                ssd1306_draw_char(&ssd, tecla, 8, 10);
                if(estado_btn_a == true){
                    ssd1306_draw_string(&ssd, "VERDE: TRUE", 8, 30);
                    printf("Verde: TRUE\n");
                }
                else{
                    ssd1306_draw_string(&ssd, "VERDE: FALSE", 8, 30);
                    printf("Verde: FALSE\n"); 
                }
                if(estado_btn_b == true){
                    ssd1306_draw_string(&ssd, "AZUL: TRUE", 8, 48);
                    printf("Azul: TRUE\n");
                }
                else{
                    ssd1306_draw_string(&ssd, "AZUL: FALSE", 8, 48);
                    printf("Azul: FALSE\n");
                }
                ssd1306_send_data(&ssd);
            }
        }
    }
}

void iniciar_pinos(){
    gpio_init(led_red_pino);
    gpio_init(led_blue_pino);
    gpio_init(led_green_pino);
    gpio_set_dir(led_red_pino, GPIO_OUT);
    gpio_set_dir(led_blue_pino, GPIO_OUT);
    gpio_set_dir(led_green_pino, GPIO_OUT);
    gpio_put(led_red_pino, estado_red);
    gpio_put(led_blue_pino, estado_blue);
    gpio_put(led_green_pino, estado_green);

    gpio_init(btn_a);
    gpio_init(btn_b);
    gpio_init(btn_j);
    gpio_set_dir(btn_a, GPIO_IN);
    gpio_set_dir(btn_b, GPIO_IN);
    gpio_set_dir(btn_j, GPIO_IN);
    gpio_pull_up(btn_a);
    gpio_pull_up(btn_b);
    gpio_pull_up(btn_j);
}

void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if(tempo_atual - ultimo_tempo > 200){
        if(gpio == 5){
            estado_green = !estado_green;
            estado_btn_a = !estado_btn_a;
            gpio_put(led_green_pino, estado_green);
        }
        else if(gpio == 6){
            estado_blue = !estado_blue;
            estado_btn_b = !estado_btn_b;
            gpio_put(led_blue_pino, estado_blue);
        }
        else if(gpio == 22){
            reset_usb_boot(0, 0);
        }
    }
}