#ifndef _BSP_H_ 
#define _BSP_H_

#ifdef BSP_GLOBALS
#define BSP_EXT
#else 
#define BSP_EXT extern 
#endif 

// LED Pin: PA8
#define LED_CLOCK                      RCC_APB2Periph_GPIOB
#define LED_BASE                       GPIOB
#define LED_CONFIG_PIN                 GPIO_Pin_5
#define LED_RELAY_PIN                  GPIO_Pin_12
   
    
extern void delay_ms(unsigned int ms);
extern void HW_IO_init(void);
extern void BSP_Init(void);
extern void Board_Init(void);
extern void SetLEDStatus(unsigned char status);
void SetLEDToggle(void);
void SetRELAYToggle(void);
unsigned int OS_CPU_SysTickClkFreq (void);

#endif
