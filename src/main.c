
// main.c
// Kathy Guo
// kaguo@g.hmc.edu
// 9/24/2026
// Implements a timer on a STM32 MCU to play music on a speaker

// includes to add libraries
#include <stdint.h>
#define __IO volatile

// define macros
#define RCC_BASE   0x40021000UL
#define TIM16_BASE 0x40014400UL
#define GPIOA_BASE 0x48000000UL

// RCC registers
typedef struct {
    __IO uint32_t CR;           // 0x00
    __IO uint32_t ICSCR;        // 0x04
    __IO uint32_t CFGR;         // 0x08
    __IO uint32_t PLLCFGR;      // 0x0C
    __IO uint32_t PLLSAI1CFGR;  // 0x10
    uint32_t RESERVED0;            // 0x14
    __IO uint32_t CIER;         // 0x18
    __IO uint32_t CIFR;         // 0x1C
    __IO uint32_t CICR;         // 0x20
    uint32_t RESERVED1;            // 0x24
    __IO uint32_t AHB1RSTR;     // 0x28
    __IO uint32_t AHB2RSTR;     // 0x2C
    __IO uint32_t AHB3RSTR;     // 0x30
    uint32_t RESERVED2;            // 0x34
    __IO uint32_t APB1RSTR1;    // 0x38
    __IO uint32_t APB1RSTR2;    // 0x3C
    __IO uint32_t APB2RSTR;     // 0x40
    uint32_t RESERVED3;            // 0x44
    __IO uint32_t AHB1ENR;      // 0x48
    __IO uint32_t AHB2ENR;      // 0x4C
    __IO uint32_t AHB3ENR;      // 0x50
    uint32_t RESERVED4;            // 0x54
    __IO uint32_t APB1ENR1;     // 0x58
    __IO uint32_t APB1ENR2;     // 0x5C
    __IO uint32_t APB2ENR;      // 0x60
    uint32_t RESERVED5;            // 0x64
    __IO uint32_t AHB1SMENR;    // 0x68
    __IO uint32_t AHB2SMENR;    // 0x6C
    __IO uint32_t AHB3SMENR;    // 0x70
    uint32_t RESERVED6;            // 0x74
    __IO uint32_t APB1SMENR1;   // 0x78
    __IO uint32_t APB1SMENR2;   // 0x7C
    __IO uint32_t APB2SMENR;    // 0x80
    uint32_t RESERVED7;            // 0x84
    __IO uint32_t CCIPR;        // 0x88
    uint32_t RESERVED8;            // 0x8C
    __IO uint32_t BDCR;         // 0x90
    __IO uint32_t CSR;          // 0x94
    __IO uint32_t CRRCR;        // 0x98
    __IO uint32_t CCIPR2;       // 0x9C
} RCC_TypeDef;

typedef struct {
    __IO uint32_t CR1;       // 0x00
    __IO uint32_t CR2;       // 0x04
    uint32_t RESERVED0;     // 0x08
    __IO uint32_t DIER;      // 0x0C
    __IO uint32_t SR;        // 0x10
    __IO uint32_t EGR;       // 0x14
    __IO uint32_t CCMR1;     // 0x18
    uint32_t RESERVED1;     // 0x1C
    __IO uint32_t CCER;      // 0x20
    __IO uint32_t CNT;       // 0x24
    __IO uint32_t PSC;       // 0x28
    __IO uint32_t ARR;       // 0x2C
    __IO uint32_t RCR;       // 0x30
    __IO uint32_t CCR1;      // 0x34
    uint32_t RESERVED2[3];  // 0x38-0x40
    __IO uint32_t BDTR;      // 0x44
    __IO uint32_t DCR;       // 0x48
    __IO uint32_t DMAR;      // 0x4C
    __IO uint32_t OR1;       // 0x50
    uint32_t RESERVED3[3];  // 0x54-0x5C
    __IO uint32_t OR2;       // 0x60
} TIM16_TypeDef;


typedef struct {
    __IO uint32_t MODER;     // enables mode selection
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFRL;      // enables alternate function configurations
    __IO uint32_t AFRH;
    __IO uint32_t BRR;
    __IO uint32_t ASCR;
} GPIO_TypeDef;


#define RCC   ((RCC_TypeDef *) RCC_BASE)
#define TIM16 ((TIM16_TypeDef *) TIM16_BASE)
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)


// peripheral clock-enable bits

// RCC_CR
#define MSION 0
#define MSIRDY 1 // 1 when MSI oscillator is ready
#define MSIRGSEL 3
#define MSIRANGE 4

// RCC registers for GPIOA and TIM16
#define GPIOAEN 0
#define TIM16EN 17

// RCC_CFGR
#define SW 0
#define SWS 2
#define HPRE  4
#define PPRE2 11

// GPIOA
#define MODE6 12
#define AFSEL6 24

// TIM16
#define CEN 0
#define UG 0
#define OC1M 4
#define OC1PE 3
#define CC1E 0
#define CC1P 1
#define MOE 15
#define UIF 0

// lab4_starter.c
// Fur Elise, E155 Lab 4
// Updated Fall 2024

 //Pitch in Hz, duration in ms
const int notes[][2] = {
{659, 125},
{623, 125},
{659, 125},
{623, 125},
{659, 125},
{494, 125},
{587, 125},
{523, 125},
{440, 250},
{  0, 125},
{262, 125},
{330, 125},
{440, 125},
{494, 250},
{  0, 125},
{330, 125},
{416, 125},
{494, 125},
{523, 250},
{  0, 125},
{330, 125},
{659, 125},
{623, 125},
{659, 125},
{623, 125},
{659, 125},
{494, 125},
{587, 125},
{523, 125},
{440, 250},
{  0, 125},
{262, 125},
{330, 125},
{440, 125},
{494, 250},
{  0, 125},
{330, 125},
{523, 125},
{494, 125},
{440, 250},
{  0, 125},
{494, 125},
{523, 125},
{587, 125},
{659, 375},
{392, 125},
{699, 125},
{659, 125},
{587, 375},
{349, 125},
{659, 125},
{587, 125},
{523, 375},
{330, 125},
{587, 125},
{523, 125},
{494, 250},
{  0, 125},
{330, 125},
{659, 125},
{  0, 250},
{659, 125},
{1319, 125},
{  0, 250},
{623, 125},
{659, 125},
{  0, 250},
{623, 125},
{659, 125},
{623, 125},
{659, 125},
{623, 125},
{659, 125},
{494, 125},
{587, 125},
{523, 125},
{440, 250},
{  0, 125},
{262, 125},
{330, 125},
{440, 125},
{494, 250},
{  0, 125},
{330, 125},
{416, 125},
{494, 125},
{523, 250},
{  0, 125},
{330, 125},
{659, 125},
{623, 125},
{659, 125},
{623, 125},
{659, 125},
{494, 125},
{587, 125},
{523, 125},
{440, 250},
{  0, 125},
{262, 125},
{330, 125},
{440, 125},
{494, 250},
{  0, 125},
{330, 125},
{523, 125},
{494, 125},
{440, 500},
{  0,   0}};

//const int notes[][2] = {
//    // Pickup: "Do you re-"
//    {330, 240}, // E4
//    {392, 120}, // G4
//    {440, 120}, // A4

//    // Measure 1 & 2: "-mem-ber the 21st night of Sep-"
//    {494, 240}, // B4
//    {494, 240}, // B4
//    {  0, 240}, // Rest
//    {440, 240}, // A4
//    {494, 240}, // B4
//    {587, 24`0}, // D5
//    {659, 240}, // E5
//    {440, 120}, // A4
//    {392, 120}, // G4
//    {440, 120}, // A4
//    {494, 120}, // B4

//    // Measure 3 & 4: "-tem-ber?"
//    {494, 240}, // B4
//    {494, 240}, // B4
//    {  0, 240}, // Rest
//    {  0, 120}, // Rest
//    {392, 120}, // G4
//    {440, 120}, // A4

//    // Measure 4 & 5: "Love was chang-in' the mind's pre-"
//    {494, 120}, // B4
//    {587, 120}, // D5
//    {659, 240}, // E5
//    {440, 240}, // A4
//    {392, 240}, // G4
//    {392, 240}, // G4

//    // Measure 6 & 7: "-ten-ders, while..."
//    {494, 240}, // B4
//    {494, 240}, // B4
//    {  0, 240}, // Rest
//    {  0, 120}, // Rest
//    {392, 120}, // G4
//    {440, 120}, // A4

//    // Measure 7 & 8: "...chas-ing the clouds a-way"
//    {494, 120}, // B4 ("chas-")
//    {587, 120}, // D5 ("-ing")
//    {659, 240}, // E5 ("the")
//    {440, 240}, // A4 ("clouds")
//    {392, 240}, // G4 ("a-")
//    {440, 240},
//    {392, 480}, // G4 ("way") — held resolution note

//    // End Marker
//    {  0,   0}
//};

void play_note(uint32_t frequency, uint32_t duration) {

    uint32_t period;
    uint32_t cycles;

    // stop TIM16 before configuring the next note
    TIM16->CR1 &= ~(1 << CEN);

    // disable the output
    TIM16->CCER &= ~(1 << CC1E);

    if (frequency == 0) {
        // rest: use a silent 1 kHz timer
        // one timer period = 1 ms
        period = 1000;
        cycles = duration;
    }
    else {
        // counter clock = 1 MHz
        // calculate the timer counts per PWM period
        period = 1000000 / frequency;

        // calculate the number of completed periods needed for the requested duration
        cycles = (duration * 1000 + period / 2) / period;
    }

    // configure the PWM period
    TIM16->ARR = period - 1;

    // maintain 50% duty cycle
    TIM16->CCR1 = period / 2;

    // load the new settings and reset the counter
    TIM16->EGR = (1 << UG);

    // clear the update flag caused by UG
    TIM16->SR &= ~(1 << UIF);

    // enable the speaker output for actual notes
    if (frequency != 0) {
        TIM16->CCER |= (1 << CC1E);
    }

    // start TIM16
    TIM16->CR1 |= (1 << CEN);

    // count completed timer periods
    for (uint32_t i = 0; i < cycles; i++) {

        // wait for the next update event
        while (!(TIM16->SR & (1 << UIF))) {
        }

        // clear the flag for the next period
        TIM16->SR &= ~(1 << UIF);
    }

    // stop note
    TIM16->CCER &= ~(1 << CC1E);
    TIM16->CR1 &= ~(1 << CEN);
}

int main(void) {

  // enable peripheral clocks
  RCC->AHB2ENR |= (1 << GPIOAEN);
  RCC->APB2ENR |= (1 << TIM16EN);

  // enable MSI and wait until ready
  RCC->CR |= (1 << MSION);

  while (!(RCC->CR & (1 << MSIRDY))) {
    //wait for MSI to become ready
  }

  // select MSIRANGE from RCC_CR
  RCC->CR |= (1 << MSIRGSEL);

  // set MSI frequency to 4 MHz
  RCC->CR &= ~(15 << MSIRANGE);
  RCC->CR |= (6 << MSIRANGE);

  // select MSI as the system clock
  RCC->CFGR &= ~(3 << SW);

  // Wait until MSI is selected
  while ((RCC->CFGR & (3 << SWS)) != 0) {
  }

  // Set AHB, APB1 and APB2 prescalers to 1
  RCC->CFGR &= ~((15 << HPRE) | (7 << PPRE2));

  // set PA6 to alternate-function mode (10)
  GPIOA->MODER &= ~(3 << MODE6);
  GPIOA->MODER |= (2 << MODE6);

  // select AF14 for PA6
  GPIOA->AFRL &= ~(15 << AFSEL6);
  GPIOA->AFRL |= (14 << AFSEL6);

  // configure timer freq
  TIM16->PSC = 3;  // prescaler = 3; divide 4Mhz by (3+1)

  // select PWM mode 1 and enable preloading
  TIM16->CCMR1 = (6 << OC1M) | (1 << OC1PE);

  // clear all TIM16 bit
  TIM16->CCER = 0;

  // enables TIM16 main output gate
  TIM16->BDTR |= (1 << MOE);

  while (1) {
    for (int i = 0; notes[i][1] != 0; i++) {
      uint32_t frequency = notes[i][0];
      uint32_t duration = notes[i][1];

      play_note(frequency, duration);
    }

  // Song finished: remain silent
    return (0);
  }
}
