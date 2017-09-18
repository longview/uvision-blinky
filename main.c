/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
 
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif
 
#include "stm32f10x.h"                  // Device header
 
#include "stdio.h" 
 
 uint16_t AD_value;
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
    printf("System clock: %u Hz\n", SystemCoreClock);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

  // Configure AIN on PA0 and PA1, ADC channel IN0 and IN1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_InitTypeDef ADC_InitStructure;

  RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADCCLK = PCLK22/6 = 72/6=12MHz
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //Enable ADC1 Clock

  /* ADC1 configuration */
  ADC_DeInit(ADC1); //Power-on default
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //Independent conversion mode (single)
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; //Convert single channel only
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //Convert 1 time
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //No external triggering
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //Right 12-bit data alignment
  ADC_InitStructure.ADC_NbrOfChannel = 1; //single channel conversion
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_TempSensorVrefintCmd(ENABLE); //wake up temperature sensor
  ADC_Cmd(ADC1, ENABLE); //Enable ADC1


  ADC_ResetCalibration(ADC1); //Enable ADC1 reset calibration register
  while(ADC_GetResetCalibrationStatus(ADC1)); //check the end of ADC1 reset calibration register
  ADC_StartCalibration(ADC1); //Start ADC1 calibration
  while(ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE); //Start ADC1 software conversion



  while (1)
  {
      while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Wail for conversion complete
      AD_value = ADC_GetConversionValue(ADC1); //Read ADC value
      ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear EOC flag

      printf("ADC value, changed: %d \r\n", AD_value);

  }
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
#ifdef RTE_Compiler_EventRecorder
  // Initialize and start Event Recorder
  EventRecorderInitialize(EventRecordError, 1U);
#endif
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  osKernelStart();    	// Start thread execution
	
	
	
	
  for (;;) {}
}
