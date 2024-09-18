#include <stdint.h>

typedef struct
{
  // IBUSERR instruction bus error
  uint8_t ibus : 1;      
  // PRECISERR The instruction just before caused the err.
  uint8_t precise : 1;   
  // IMPRECISERR HW could not determine exact instruction.
  // - De-referencing a NULL pointer.
  uint8_t imprecise : 1; 
  // UNSTKERR Fault on unstacking for ret. from exception.
  uint8_t unStk : 1;     
  // STKERR Fault on stacking for exception entry.
  uint8_t stk : 1;       
  // LSPERR Fault on floating-pt lazy state preservation.
  uint8_t lsp : 1;       
  uint8_t : 1;
  // Bus Fault Address Reg. is valid.
  uint8_t bfarvalid : 1; 
} busFault_t;

void HardFault_Handler(void)
{
  while (1)
    ;
}

void MemManage_Handler(void)
{
  while (1)
    ;
}

void BusFault_Handler(void)
{
  busFault_t busFault = *(busFault_t*)0xE000ED29;
  (void)busFault;
  while (1)
    ;
}

void UsageFault_Handler(void)
{
  while (1)
    ;
}