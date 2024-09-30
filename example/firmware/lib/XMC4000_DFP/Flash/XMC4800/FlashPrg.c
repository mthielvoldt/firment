/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2012 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashPrg.c:  Flash Programming Functions adapted for               */
/*               Infineon XMC4500 Flash                                */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures

typedef volatile unsigned char        vu8;
typedef volatile unsigned long        vu32;
typedef          unsigned long         u32;
typedef volatile unsigned long long   vu64;
typedef          unsigned long long    u64;

#define M8(adr) (*((vu8   *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))
#define M64(adr) (*((vu64 *) (adr)))

// Peripheral Memory Map
#define FLASH0_BASE            (0x58002000)

#define FLASH0                 ((Flash0_TypeDef *) FLASH0_BASE)

// Flash Registers
typedef struct {
  vu32 RESERVED0[2];
  vu32 ID;                     // offset 0x008 R/W Flash Module Identification Register
  vu32 RESERVED1[1];
  vu32 FSR;                    // offset 0x010 R/W Flash Status Register
  vu32 FCON;                   // offset 0x014 R/W Flash Configuration Register
  vu32 MARP;                   // offset 0x018 R/W Flash Margin Control Register
  vu32 RESERVED2[1];
  vu32 PROCON[3];              // offset 0x020 R/W Flash Protection Configuration User 0..2
} Flash0_TypeDef;

// Flash Status Register definitions
#define FSR_PBUSY              (1UL <<  0)
#define FSR_FABUSY             (1UL <<  1)
#define FSR_PFOPER             (1UL <<  8)
#define FSR_SQER               (1UL << 10)
#define FSR_PROER              (1UL << 11)
#define FSR_VER                (1UL << 31)

#define FSR_ERR                (FSR_PROER | FSR_VER)

#define FLASH_CACHE            (0x08000000UL)
#define FLASH_NoCACHE          (0x0C000000UL)     // base address used for flashing
#define FLASH_OFFS             (0x04000000UL)     // offset used for cached Flash

#if defined XMC4800_1024 || defined XMC4800c_1024
  #define FLASH_SIZE           (0x00100000UL)
#endif
#if defined XMC4800_1536  || defined XMC4800c_1536
  #define FLASH_SIZE           (0x00180000UL)
#endif
#if defined XMC4800_2048  || defined XMC4800c_2048
  #define FLASH_SIZE           (0x00200000UL)
#endif

  #define FLASH_SIZE_16KB      (0x00020000UL)
  #define FLASH_SIZE_128KB     (0x00040000UL)
  #define FLASH_SIZE_256KB     (FLASH_SIZE)

/*
  -----------------+-----------+-------------+-------------+-------------+-------------+-------------+--------------
                   |           |  1st write  |  2nd write  |  3rd write  |  4th write  |  5th write  |  6th write
     Command       | Number of |-------------+-------------+-------------+-------------+-------------+--------------
                   | writes    | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data | Addr.| Data
  -----------------+-----------+------+------+------+------+------+------+------+------+------+------+------+-------
  Reset to Read          1      0x5554 0xF0   --     --     --     --     --     --     --     --     --     --
  Enter Page Mode        1      0x55F4 0x5y     --     --     --     --     --     --     --     --     --     --
  Load Page              1      0x55F0 WD     --     --     --     --     --     --     --     --     --     --
  Write Page             4      0x5554 0xAA   0xAAA8 0x55   0x5554 0xA0   PA     0xAA   --     --     --     --
       Sector Erase      6      0x5554 0xAA   0xAAA8 0x55   0x5554 0x80   0x5554 0xAA   0x0AA8 0x55   SA     0x30
  phy. Sector Erase      6      0x5554 0xAA   0xAAA8 0x55   0x5554 0x80   0x5554 0xAA   0x0AA8 0x55   SA     0x40
  Clear Status           1      0x5554 0xF5   --     --     --     --     --     --     --     --     --     --
 */

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  return (0);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  return (0);
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {
  unsigned long adr = FLASH_NoCACHE;

  /* it exists no 'Flash erase' command so we erase every single sector */
  while (adr < (FLASH_NoCACHE + FLASH_SIZE)) {
     EraseSector (adr);

     if (adr < FLASH_NoCACHE + FLASH_SIZE_16KB) {
       adr += 0x04000;         /*  16 kB sector */
     }
     else if (adr < FLASH_NoCACHE + FLASH_SIZE_128KB) {
       adr += 0x20000;         /* 128 kB sector */
     }
     else if (adr < FLASH_NoCACHE + FLASH_SIZE_256KB) {
       adr += 0x40000;         /* 256 kB sector */
     }

  }

  return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {

  M32(FLASH_NoCACHE + 0x5554) = 0xF5;                   // Start Clear Status Command

  M32(FLASH_NoCACHE + 0x5554) = 0xAA;                   // Start Block Erase Command
  M32(FLASH_NoCACHE + 0xAAA8) = 0x55;
  M32(FLASH_NoCACHE + 0x5554) = 0x80;
  M32(FLASH_NoCACHE + 0x5554) = 0xAA;
  M32(FLASH_NoCACHE + 0xAAA8) = 0x55;
  M32(adr | FLASH_OFFS)    = 0x30;                     // erase logical sector

  while  (FLASH0->FSR & FSR_PBUSY);                     // Wait until Erase completed

  if (FLASH0->FSR & FSR_ERR) {                          // Check for Error
    return (1);                                         // Failed
  }

  return 0;
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {
  int i;

  // check if page is empty
  for (i = 0; i < sz; i++) {
    if (M8((adr+i) | FLASH_OFFS) != 0x00)               // 0x00 = valEmpty
      return (1);
  }

  sz = (sz + 7) & ~7;                                   // Adjust size for DWords

  M32(FLASH_NoCACHE + 0x5554) = 0xF5;                   // Start Clear Status Command

  M32(FLASH_NoCACHE + 0x5554) = 0x50;                   // Start Enter Page Mode Command
  while (sz) {
    M64(FLASH_NoCACHE + 0x55F0) = *((u64 *)buf);        // Start Load Page Command
    buf += 8;
    sz  -= 8;
  }

  M32(FLASH_NoCACHE + 0x5554) = 0xAA;                   // Start Write Page Command
  M32(FLASH_NoCACHE + 0xAAA8) = 0x55;
  M32(FLASH_NoCACHE + 0x5554) = 0xA0;
  M32(adr | FLASH_OFFS)       = 0xAA;

  while  (FLASH0->FSR & FSR_PBUSY);                     // Wait until Write Page completed

  if (FLASH0->FSR & FSR_ERR) {                          // Check for Error
    return (1);                                         // Failed
  }

  return (0);
}

