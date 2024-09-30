/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2012 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.c:  Device Description for                                */
/*               Infineon XMC4800 Flash                                */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures

#ifdef FLASH_MEM

#ifdef XMC4800_1024
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4800 1024kB Flash",        // Device name
   ONCHIP,                        // Device Type
   0x0C000000,                    // Device Start Address
   0x00100000,                    // Device Size in Bytes (1024kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (3 Sectors)
   SECTOR_END                 
};
#endif

#ifdef XMC4800c_1024
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4800 1024kB cached Flash", // Device name
   ONCHIP,                        // Device Type
   0x08000000,                    // Device Start Address
   0x00100000,                    // Device Size in Bytes (1024kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (3 Sectors)
   SECTOR_END                 
};
#endif

#ifdef XMC4800_2048
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4800 2048kB Flash",         // Device name
   ONCHIP,                        // Device Type
   0x0C000000,                    // Device Start Address
   0x00200000,                    // Device Size in Bytes (2048kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (2 Sectors)
   SECTOR_END                 
};
#endif

#ifdef XMC4800c_2048
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4800 2048kB cached Flash",  // Device name
   ONCHIP,                        // Device Type
   0x08000000,                    // Device Start Address
   0x00200000,                    // Device Size in Bytes (2048kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (2 Sectors)
   SECTOR_END                 
};
#endif

#ifdef XMC4800_1536
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4800 1536kB Flash",        // Device name
   ONCHIP,                        // Device Type
   0x0C000000,                    // Device Start Address
   0x00180000,                    // Device Size in Bytes (1536kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (1 Sectors)
   SECTOR_END                 
};
#endif

#ifdef XMC4800c_1536
struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,                // Driver Version, do not modify!
   "XMC4500 512kB cached Flash",  // Device name
   ONCHIP,                        // Device Type
   0x08000000,                    // Device Start Address
   0x00180000,                    // Device Size in Bytes (1536kB)
   256,                           // Programming Page Size (Bytes)
   0,                             // Reserved, must be 0
   0x00,                          // Initial Content of Erased Memory
   100,                           // Program Page Timeout 100 mSec
   20000,                         // Erase Sector Timeout 20000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,             // Sector Size  16kB (8 Sectors)
   0x20000, 0x020000,             // Sector Size 128kB (1 Sector )
   0x40000, 0x040000,             // Sector Size 256kB (1 Sectors)
   SECTOR_END                 
};
#endif

#endif
