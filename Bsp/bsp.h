#ifndef _BSP_H_ 
#define _BSP_H_

#ifdef BSP_GLOBALS
#define BSP_EXT
#else 
#define BSP_EXT extern 
#endif 


#define LEDn                        4

#define LED1_CLOCK                      RCC_APB2Periph_GPIOB
#define LED2_CLOCK                      RCC_APB2Periph_GPIOB
#define LED3_CLOCK                      RCC_APB2Periph_GPIOB
#define LED4_CLOCK                      RCC_APB2Periph_GPIOC
#define LED1_GPIO_PORT                       GPIOB
#define LED2_GPIO_PORT                       GPIOB
#define LED3_GPIO_PORT                       GPIOB
#define LED4_GPIO_PORT                       GPIOC
#define LED1_GPIO_PIN                 			GPIO_Pin_9
#define LED2_GPIO_PIN                  			GPIO_Pin_8
#define LED3_GPIO_PIN                  			GPIO_Pin_5
#define LED4_GPIO_PIN                  			GPIO_Pin_13

typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
  LED5 = 4  /* Bicolor Led */
} Led_TypeDef;  
    
extern void delay_ms(unsigned int ms);
extern void HW_IO_init(void);
extern void BSP_Init(void);
extern void Board_Init(void);
extern void SetLEDStatus(Led_TypeDef Led, unsigned char status);
extern void SetLEDToggle(Led_TypeDef Led);
extern void SetRELAYToggle(Led_TypeDef Led);
unsigned int OS_CPU_SysTickClkFreq (void);

#endif
