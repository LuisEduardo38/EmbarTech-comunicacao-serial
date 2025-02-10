//Biblioteca para manipulação básica
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

//Biblioteca para manipulação do display
#include "hardware/i2c.h"
#include "bibliotecas/ssd1306.h"
#include "bibliotecas/font.h"

//Biblioteca para manipulação da matriz de led
#include "hardware/pio.h"
#include "ws2818b.pio.h"

////Biblioteca para manipulação basíca
const uint8_t led_red_pino = 13;
const uint8_t led_blue_pino = 12;
const uint8_t led_green_pino = 11;
volatile bool estado_red = false;
volatile bool estado_blue = false;
volatile bool estado_green = false;

//Variáveis para manipulação dos botões da placa.
const uint8_t btn_a = 5;
const uint8_t btn_b = 6;
const uint8_t btn_j = 22;
volatile bool estado_btn_a = false;
volatile bool estado_btn_b = false;

//Parâmetros para comunicação com o display.
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//Variáveis para controle da matriz.
const uint8_t matriz_pino = 7;
const uint8_t numero_leds = 25;

//Frames com o números
double segundo0 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo1 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 1.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0};

double segundo2 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo3 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo4 [] = {0.0, 0.0, 1.0, 0.0, 0.0,
                      1.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 1.0,
                      0.0, 1.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0};

double segundo5 [] = {0.0, 0.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo6 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo7 [] = {0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo8 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

double segundo9 [] = {0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0,
                      0.0, 1.0, 0.0, 1.0, 0.0,
                      0.0, 1.0, 1.0, 1.0, 0.0};

//Variável para realizar o debounce
volatile uint32_t ultimo_tempo = 0;

//Protótipos das funções do projeto.
void iniciar_pinos();
void gpio_irq_handler(uint gpio, uint32_t events);
void escolha_numero(uint8_t numero, PIO pio, int sm);
void imprimir_numero(double *numero, PIO pio, int sm);

int main(){
    stdio_init_all();
    iniciar_pinos();

    //Configurações para manipular o display.
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

    //Configurações para manipular a matriz de led.
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2818b_program);
    ws2818b_program_init(pio, sm, offset, matriz_pino, 800000);

    //Configurações dos botões para entrar nas rotinas de interrupções.
    gpio_set_irq_enabled_with_callback(btn_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btn_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(btn_j, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    bool cor = true;
    //Loop principal
    while (true) {
        //IF para validar se a placa está conectado.
        if(stdio_usb_connected){
            //A estrutura deste IF foi estruturada para não bloquear a execução do código.
            char tecla;
            int tecla_lida = getchar_timeout_us(0);
            if (tecla_lida != PICO_ERROR_TIMEOUT){
                tecla = (char)tecla_lida;
                cor = !cor;
                ssd1306_fill(&ssd, !cor);
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
                ssd1306_draw_char(&ssd, tecla, 8, 10);
            }

            //IFs para registra os estados dos leds
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

            //IF para chamar as funções para imprimir os números na matriz
            if((tecla >= '1') && (tecla <= '9')){
                uint8_t numero = tecla - '0';
                escolha_numero(numero, pio, sm);
            }
        }
        sleep_ms(500);
    }
}

//Função para iniciar os pinos da placa.
void iniciar_pinos(){
    //Iniciando os pinos do LED RGB
    gpio_init(led_red_pino);
    gpio_init(led_blue_pino);
    gpio_init(led_green_pino);
    gpio_set_dir(led_red_pino, GPIO_OUT);
    gpio_set_dir(led_blue_pino, GPIO_OUT);
    gpio_set_dir(led_green_pino, GPIO_OUT);
    gpio_put(led_red_pino, estado_red);
    gpio_put(led_blue_pino, estado_blue);
    gpio_put(led_green_pino, estado_green);

    //Iniciando os pinos dos botões da placa
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

//Função para gerenciar as rotinas de iterrupções
void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if(tempo_atual - ultimo_tempo > 200){
        ultimo_tempo = tempo_atual;
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

//Função para gerencia qual frame será chamando
void escolha_numero(uint8_t numero, PIO pio, int sm){
    double *vetor[] = {segundo0, segundo1, segundo2, segundo3, segundo4, 
                       segundo5, segundo6, segundo7, segundo8, segundo9};

    imprimir_numero(vetor[numero], pio, sm);
}

//Função que imprimi os números na matriz
void imprimir_numero(double *numero, PIO pio, int sm){
    uint8_t red, blue, green;
    red = 100;
    blue = 0;
    green = 0;

    for(uint8_t i = 0; i < numero_leds; i++){
        uint8_t RED = (uint8_t)(red * numero[i]);
        uint8_t BLUE = (uint8_t)(blue * numero[i]);
        uint8_t GREEN = (uint8_t)(green * numero[i]);

        pio_sm_put_blocking(pio, sm, GREEN);
        pio_sm_put_blocking(pio, sm, RED);
        pio_sm_put_blocking(pio, sm, BLUE);
    }
}