#include "fmt_ioc.h"
#include <Device_IOC.h>
#include <RTE_DeviceConfig.h>
#include <xmc_gpio.h>

#define IOC_CALLBACK_ID(eruId, output) (eruId * OUTPUT_COUNT_PER_ERU + output)

const XMC_ERU_ETL_EDGE_DETECTION_t edgeMap[] = {
    [EDGE_TYPE_NONE] = XMC_ERU_ETL_EDGE_DETECTION_DISABLED,
    [EDGE_TYPE_FALLING] = XMC_ERU_ETL_EDGE_DETECTION_FALLING,
    [EDGE_TYPE_RISING] = XMC_ERU_ETL_EDGE_DETECTION_RISING,
    [EDGE_TYPE_BOTH] = XMC_ERU_ETL_EDGE_DETECTION_BOTH,
};

// #if 1 for code fold.  define only used IOC configs to save prog.mem
#if 1
#ifdef USE_IOC0  
const RTE_IOC_t ioc0 =  IOC_0;
#endif
#ifdef USE_IOC1  
const RTE_IOC_t ioc1 =  IOC_1;
#endif
#ifdef USE_IOC2  
const RTE_IOC_t ioc2 =  IOC_2;
#endif
#ifdef USE_IOC3  
const RTE_IOC_t ioc3 =  IOC_3;
#endif
#ifdef USE_IOC4  
const RTE_IOC_t ioc4 =  IOC_4;
#endif
#ifdef USE_IOC5  
const RTE_IOC_t ioc5 =  IOC_5;
#endif
#ifdef USE_IOC6  
const RTE_IOC_t ioc6 =  IOC_6;
#endif
#ifdef USE_IOC7  
const RTE_IOC_t ioc7 =  IOC_7;
#endif
#ifdef USE_IOC8  
const RTE_IOC_t ioc8 =  IOC_8;
#endif
#ifdef USE_IOC9  
const RTE_IOC_t ioc9 =  IOC_9;
#endif
#ifdef USE_IOC10 
const RTE_IOC_t ioc10 = IOC_10;
#endif
#ifdef USE_IOC11 
const RTE_IOC_t ioc11 = IOC_11;
#endif
#ifdef USE_IOC12 
const RTE_IOC_t ioc12 = IOC_12;
#endif
#ifdef USE_IOC13 
const RTE_IOC_t ioc13 = IOC_13;
#endif
#ifdef USE_IOC14 
const RTE_IOC_t ioc14 = IOC_14;
#endif
#ifdef USE_IOC15 
const RTE_IOC_t ioc15 = IOC_15;
#endif
#ifdef USE_IOC16 
const RTE_IOC_t ioc16 = IOC_16;
#endif
#ifdef USE_IOC17 
const RTE_IOC_t ioc17 = IOC_17;
#endif
#ifdef USE_IOC18 
const RTE_IOC_t ioc18 = IOC_18;
#endif
#ifdef USE_IOC19 
const RTE_IOC_t ioc19 = IOC_19;
#endif
#ifdef USE_IOC20 
const RTE_IOC_t ioc20 = IOC_20;
#endif
#ifdef USE_IOC21 
const RTE_IOC_t ioc21 = IOC_21;
#endif
#ifdef USE_IOC22 
const RTE_IOC_t ioc22 = IOC_22;
#endif
#ifdef USE_IOC23 
const RTE_IOC_t ioc23 = IOC_23;
#endif
#ifdef USE_IOC24 
const RTE_IOC_t ioc24 = IOC_24;
#endif
#ifdef USE_IOC25 
const RTE_IOC_t ioc25 = IOC_25;
#endif
#ifdef USE_IOC26 
const RTE_IOC_t ioc26 = IOC_26;
#endif
#ifdef USE_IOC27 
const RTE_IOC_t ioc27 = IOC_27;
#endif
#ifdef USE_IOC28 
const RTE_IOC_t ioc28 = IOC_28;
#endif
#ifdef USE_IOC29 
const RTE_IOC_t ioc29 = IOC_29;
#endif
#ifdef USE_IOC30 
const RTE_IOC_t ioc30 = IOC_30;
#endif
#ifdef USE_IOC31 
const RTE_IOC_t ioc31 = IOC_31;
#endif
#ifdef USE_IOC32 
const RTE_IOC_t ioc32 = IOC_32;
#endif
#ifdef USE_IOC33 
const RTE_IOC_t ioc33 = IOC_33;
#endif
#endif

const RTE_IOC_t *iocConfigs[RTE_IOC_COUNT] = {
#ifdef USE_IOC0
  [0] = &ioc0,
#endif
#ifdef USE_IOC1
  [1] = &ioc1,
#endif
#ifdef USE_IOC2
  [2] = &ioc2,
#endif
#ifdef USE_IOC3
  [3] = &ioc3,
#endif
#ifdef USE_IOC4
  [4] = &ioc4,
#endif
#ifdef USE_IOC5
  [5] = &ioc5,
#endif
#ifdef USE_IOC6
  [6] = &ioc6,
#endif
#ifdef USE_IOC7
  [7] = &ioc7,
#endif
#ifdef USE_IOC8
  [8] = &ioc8,
#endif
#ifdef USE_IOC9
  [9] = &ioc9,
#endif
#ifdef USE_IOC10
 [10] = &ioc10,
#endif
#ifdef USE_IOC11
 [11] = &ioc11,
#endif
#ifdef USE_IOC12
 [12] = &ioc12,
#endif
#ifdef USE_IOC13
 [13] = &ioc13,
#endif
#ifdef USE_IOC14
 [14] = &ioc14,
#endif
#ifdef USE_IOC15
 [15] = &ioc15,
#endif
#ifdef USE_IOC16
 [16] = &ioc16,
#endif
#ifdef USE_IOC17
 [17] = &ioc17,
#endif
#ifdef USE_IOC18
 [18] = &ioc18,
#endif
#ifdef USE_IOC19
 [19] = &ioc19,
#endif
#ifdef USE_IOC20
 [20] = &ioc20,
#endif
#ifdef USE_IOC21
 [21] = &ioc21,
#endif
#ifdef USE_IOC22
 [22] = &ioc22,
#endif
#ifdef USE_IOC23
 [23] = &ioc23,
#endif
#ifdef USE_IOC24
 [24] = &ioc24,
#endif
#ifdef USE_IOC25
 [25] = &ioc25,
#endif
#ifdef USE_IOC26
 [26] = &ioc26,
#endif
#ifdef USE_IOC27
 [27] = &ioc27,
#endif
#ifdef USE_IOC28
 [28] = &ioc28,
#endif
#ifdef USE_IOC29
 [29] = &ioc29,
#endif
#ifdef USE_IOC30
 [30] = &ioc30,
#endif
#ifdef USE_IOC31
 [31] = &ioc31,
#endif
#ifdef USE_IOC32
 [32] = &ioc32,
#endif
#ifdef USE_IOC33
 [33] = &ioc33,
#endif
};

typedef void (*isrCallback_t)(void);
isrCallback_t callbacks[ERU_COUNT * OUTPUT_COUNT_PER_ERU] = {0};

static bool iocIdValid(uint8_t iocId)
{
  return iocId < RTE_IOC_COUNT && (iocConfigs[iocId] != NULL);
}

/** Init Interrupt-on-change ISR, linking to a specific GPIO.
 *
 */
bool fmt_initIoc(
    uint8_t iocId,
    uint8_t outputChannel,
    edgeType_t activeEdges,
    uint32_t priority,
    void (*callback)(void))
{
  if (!iocIdValid(iocId) || outputChannel >= OUTPUT_COUNT_PER_ERU)
    return false;

  const RTE_IOC_t *ioc = iocConfigs[iocId];
  uint8_t irqNum = ioc->usableIrqNums[outputChannel];

  callbacks[IOC_CALLBACK_ID(ioc->eruId, outputChannel)] = callback;

  const XMC_GPIO_CONFIG_t gpio_config = {.mode = XMC_GPIO_MODE_INPUT_TRISTATE};
  XMC_GPIO_Init(ioc->port, ioc->pin, &gpio_config);

  // Configure ERU for rising edge detection on P1.0
  XMC_ERU_ETL_CONFIG_t eru_etl_config = {
      .input_b = ioc->inputChannel,
      .source = ioc->source,
      .edge_detection = edgeMap[activeEdges],
      .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
      .enable_output_trigger = XMC_ERU_ETL_OUTPUT_TRIGGER_ENABLED,
      .output_trigger_channel = outputChannel,
  };

  // Configure OGU for interrupt generation
  XMC_ERU_OGU_CONFIG_t eru_ogu_config = {
      .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER,
  };

  XMC_ERU_ETL_Init(ioc->eru, ioc->etlNum, &eru_etl_config);
  XMC_ERU_OGU_Init(ioc->eru, outputChannel, &eru_ogu_config);

  uint32_t encodedPrio =
      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, 0);

  NVIC_SetPriority(irqNum, encodedPrio);

  NVIC_EnableIRQ(irqNum);
  return true;
}

void fmt_enableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = iocConfigs[iocId];
  ioc->eru->EXICON_b[ioc->etlNum].PE = true;
}

void fmt_disableIoc(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return;
  const RTE_IOC_t *ioc = iocConfigs[iocId];
  ioc->eru->EXICON_b[ioc->etlNum].PE = false;
}

bool fmt_getIocPinState(uint8_t iocId)
{
  if (!iocIdValid(iocId))
    return false;
  const RTE_IOC_t *ioc = iocConfigs[iocId];
  return XMC_GPIO_GetInput(ioc->port, ioc->pin);
}

/******************* ISRs *******************/
#if 1
#ifdef USE_ERU0_0_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(0,0)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU0_1_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(0,1)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU0_2_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(0,2)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU0_3_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(0,3)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU1_0_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(1,0)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU1_1_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(1,1)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU1_2_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(1,2)];
  if (cb)
    cb();
}
#endif
#ifdef USE_ERU1_3_for_IOC
void ERU0_0_IRQHandler(void)
{
  callback_t cb = callbacks[IOC_CALLBACK_ID(1,3)];
  if (cb)
    cb();
}
#endif
#endif
