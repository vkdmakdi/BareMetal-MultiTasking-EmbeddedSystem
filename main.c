#include "main.h"
#include <stdint.h>

#define RCC_BASE                0x40021000U
#define RCC_CLK_CTRL_REG        (*(volatile uint32_t *)(RCC_BASE + 0x00U))
#define RCC_CLK_CFG_REG         (*(volatile uint32_t *)(RCC_BASE + 0x08U))
#define RCC_AHB2ENR_REG         (*(volatile uint32_t *)(RCC_BASE + 0x4CU))

#define GPIOA_BASE              0x48000000U
#define GPIOA_MODE_REG          (*(volatile uint32_t *)(GPIOA_BASE + 0x00U))
#define GPIOA_OTYPE_REG         (*(volatile uint32_t *)(GPIOA_BASE + 0x04U))
#define GPIOA_OSPEED_REG        (*(volatile uint32_t *)(GPIOA_BASE + 0x08U))
#define GPIOA_PUPDR_REG         (*(volatile uint32_t *)(GPIOA_BASE + 0x0CU))
#define GPIOA_BSRR_REG          (*(volatile uint32_t *)(GPIOA_BASE + 0x18U))
#define GPIOA_BRR_REG           (*(volatile uint32_t *)(GPIOA_BASE + 0x28U))

#define EXTI_BASE               0x40021800U
#define EXTI_IMR1_REG           (*(volatile uint32_t *)(EXTI_BASE + 0x00U))
#define EXTI_RTSR1_REG          (*(volatile uint32_t *)(EXTI_BASE + 0x08U))
#define EXTI_FTSR1_REG          (*(volatile uint32_t *)(EXTI_BASE + 0x0CU))
#define EXTI_PR1_REG            (*(volatile uint32_t *)(EXTI_BASE + 0x14U))

#define SYSTCK_BASE             0xE000E010U
#define SYSTCK_CTRL_REG         (*(volatile uint32_t *)(SYSTCK_BASE + 0x00U))
#define SYSTCK_LOAD_REG         (*(volatile uint32_t *)(SYSTCK_BASE + 0x04U))
#define SYSTCK_VAL_REG          (*(volatile uint32_t *)(SYSTCK_BASE + 0x08U))

#define LED_PIN                 (1U << 5)   // PA5
#define BUTTON_PIN              (1U << 0)   // PA0

volatile uint32_t g_ms = 0;
volatile uint8_t g_run = 1;

void clk_config(void);
void systck_init(void);
void gpio_init(void);
void exti_init(void);
void app_task(void);

void SysTick_Handler(void)
{
    g_ms++;
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_PR1_REG & BUTTON_PIN)
    {
        EXTI_PR1_REG = BUTTON_PIN;   // clear pending flag
        g_run ^= 1U;                 // toggle run/stop
    }
}

static uint32_t millis(void)
{
    return g_ms;
}

void clk_config(void)
{
    RCC_CLK_CTRL_REG |= (1U << 8);          // Enable HSI16
    RCC_CLK_CFG_REG &= ~(0xFU << 24);
    RCC_CLK_CFG_REG |= (3U << 24);          // Keep your original clock select
}

void systck_init(void)
{
    SYSTCK_LOAD_REG = 16000U - 1U;          // 1 ms tick at 16 MHz
    SYSTCK_VAL_REG  = 0U;
    SYSTCK_CTRL_REG = 7U;                   // ENABLE | TICKINT | CLKSOURCE
}

void gpio_init(void)
{
    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // PA5 as output
    GPIOA->MODER &= ~(3U << (5 * 2));   // clear mode bits
    GPIOA->MODER |=  (1U << (5 * 2));   // set as output

    GPIOA->OTYPER &= ~(1U << 5);        // push-pull
    GPIOA->OSPEEDR &= ~(3U << (5 * 2)); // low speed
    GPIOA->PUPDR &= ~(3U << (5 * 2));   // no pull

    // PA0 as input with pull-down
    GPIOA->MODER &= ~(3U << (0 * 2));   // input mode
    GPIOA->PUPDR &= ~(3U << (0 * 2));
    GPIOA->PUPDR |=  (2U << (0 * 2));   // pull-down

    GPIOA->BRR = (1U << 5);             // LED off initially
}


void exti_init(void)
{
    EXTI->IMR1  |= (1U << 0);   // unmask EXTI0
    EXTI->RTSR1 |= (1U << 0);   // rising edge trigger
    EXTI->FTSR1 &= ~(1U << 0);  // no falling edge

    EXTI->PR1 = (1U << 0);      // clear any pending interrupt
    NVIC_EnableIRQ(EXTI0_IRQn); // NVIC enable for EXTI0
}


void app_task(void)
{
    static uint32_t last_toggle = 0;
    static uint8_t led_state = 0;

    if (!g_run)
    {
        GPIOA_BRR_REG = LED_PIN;
        led_state = 0;
        return;
    }

    if ((millis() - last_toggle) >= 500U)
    {
        last_toggle = millis();
        led_state ^= 1U;

        if (led_state)
        {
            GPIOA_BSRR_REG = LED_PIN;       // LED ON
        }
        else
        {
            GPIOA_BRR_REG = LED_PIN;        // LED OFF
        }
    }
}

int main(void)
{
    clk_config();
    systck_init();
    gpio_init();
    exti_init();

    while (1)
    {
        app_task();
    }
}

void Error_Handler(void)
{
    while (1) { }
}
