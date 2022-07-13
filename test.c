#include "stm32f7xx.h"
#include <math.h>

#define CLOCKFREQ 16000000 
#define FSIN (50 - f_st)
#define DEADTIME 18

void TIM1_init(void), TIM6_init(void), ETR_init(void), ETR_Handler(void), 
    clock_config(void), SinUpdate(void), Initial_Data(void);

int32_t maxCount, amplitude, points, step = 0;

uint32_t sin1, sin2, sin3; 

volatile uint32_t impulses, f_st;

int main(void)
{
    Initial_Data();
	
    TIM1_init();
	
	ETR_init();
	TIM6_init();
	
	NVIC_SetPriority(TIM1_CC_IRQn, 1);
	NVIC_SetPriority(TIM6_DAC_IRQn , 0);
    __enable_irq();
    
    while(1)
    {

    }
}

void SinUpdate(void)
{
    if (f_st < 50)
    {
        if (f_st >= 1) points = CLOCKFREQ/(FSIN*maxCount); 
        sin1 = amplitude + amplitude*sin(1.571*4*step/points);
        sin2 = amplitude + amplitude*sin(1.571*4*step/points - 2.094);
        sin3 = amplitude + amplitude*sin(1.571*4*step/points + 2.094); 
    } else if (f_st > 50)
    {
        points = CLOCKFREQ/(-FSIN*maxCount); 
        sin1 = amplitude + amplitude*sin(1.571*4*step/points);
        sin2 = amplitude + amplitude*sin(1.571*4*step/points + 2.094);
        sin3 = amplitude + amplitude*sin(1.571*4*step/points - 2.094); 
    } else
    {
        sin1 = maxCount;
        sin2 = 0;
        sin3 = 0; 
    }
    
    TIM1->CCR1 = sin1; 
    TIM1->CCR2 = sin2;
    TIM1->CCR3 = sin3;
}

void TIM1_CC_IRQHandler(void){
    if (step > points) step = 0;
    
    SinUpdate();
    
    step++;
}

void TIM6_DAC_IRQHandler(void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
	
	impulses = TIM2->CNT;
    f_st = impulses * .05f;
	
	TIM2->CNT = 0;
	TIM6->SR &= ~TIM_SR_UIF;
	
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM6->CR1 |= TIM_CR1_CEN;
}

void Initial_Data(void)
{
    maxCount = CLOCKFREQ/10000;
    amplitude = maxCount/2;
    points = CLOCKFREQ/(FSIN*maxCount); 
}

void TIM1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //CH1,2,3
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //CH1n,2n,3n
    
    //PA8 - CH1
    GPIOA->MODER |= (2 << 16);
    GPIOA->OSPEEDR |= (3 << 16);
    GPIOA->AFR[1] |= (1 << 0); 
    //PA9 - CH2
    GPIOA->MODER |= (2 << 18);
    GPIOA->OSPEEDR |= (3 << 18);
    GPIOA->AFR[1] |= (1 << 4);
    //PA10 - CH3
    GPIOA->MODER |= (2 << 20);
    GPIOA->OSPEEDR |= (3 << 20);
    GPIOA->AFR[1] |= (1 << 8); 
    //PB13 - CH1n
    GPIOB->MODER |= (2 << 26);
    GPIOB->OSPEEDR |= (3 << 26);
    GPIOB->AFR[1] |= (1 << 20);
    //PB14 - CH2n
    GPIOB->MODER |= (2 << 28);
    GPIOB->OSPEEDR |= (3 << 28);
    GPIOB->AFR[1] |= (1 << 24);
    //PB15 - CH3n
    GPIOB->MODER |= (2 << 30);
    GPIOB->OSPEEDR |= (3 << 30);
    GPIOB->AFR[1] |= (1 << 28);
	
	TIM1->PSC = 1-1;
    TIM1->ARR = maxCount;

    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;

    TIM1->BDTR = TIM_BDTR_LOCK_0 | TIM_BDTR_MOE | DEADTIME;
    
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE | TIM_CCER_CC1NP;
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC2NE | TIM_CCER_CC2NP;
    TIM1->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC3NE | TIM_CCER_CC3NP;
     
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; 
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;

	TIM1->CR1 &= ~TIM_CR1_DIR; 
	TIM1->CR1 |= (3 << 5); 
    
    TIM1->DIER |= TIM_DIER_CC4IE;
    TIM1->CR1 |= TIM_CR1_CEN;
    
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    
    SinUpdate();

    step = 1; 
}

void TIM6_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	TIM6->PSC = maxCount-1;
	TIM6->ARR = 10000;
	TIM6->CR1 &= ~TIM_CR1_DIR;
	TIM6->CR1 |= TIM_CR1_OPM;
	TIM6->DIER |= TIM_DIER_UIE;
	
    TIM6->CR1 |= TIM_CR1_CEN;
    
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void ETR_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
    //PA0 - CH1
    GPIOA->MODER |= (2 << 0);
    GPIOA->AFR[0] |= (1 << 0); //AF1 = TIM2_CH1_ETR
    
	TIM2->PSC = 1-1;
	TIM2->CNT = 0;
	TIM2->ARR = 60000;
	TIM2->CR1 &= ~TIM_CR1_DIR;

	TIM2->SMCR |= (9 << 8);
	TIM2->SMCR &= ~TIM_SMCR_ETPS; 
	TIM2->SMCR &= ~TIM_SMCR_ETP;
	TIM2->SMCR |= TIM_SMCR_ECE;
    
    TIM2->CR1 |= TIM_CR1_CEN;
}

void clock_config(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
    
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= (3 << 21);
    RCC->CFGR |= (3 << 30);
    
    RCC->CFGR |= (0 << 4);
    RCC->CFGR |= (4 << 10);
    RCC->CFGR |= (4 << 13);
    
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI | (8 << 0) | (180 << 6) | (2 << 16);

    RCC->CR |= RCC_CR_PLLON;
    RCC->CFGR |= (2 << 0);
}
