/**
 * \file reader_pps.h
 * \copyright This file is part of the Open-ISO7816-Stack project and is distributed under the MIT license. See LICENSE file in the root directory.  
 * This file contains the prototypes of the functions to deal with PPS iso7815-3 9-Protocol and parameters selection .
 */

#ifndef __READER_PPS_H__
#define __READER_PPS_H__
#include <stdint.h>
#include "reader.h"
#include "reader_hal_comm_settings.h"

#define PPS0    0   /* PPS format character */
#define PPS1    1   /* PPS parameter character 1 */
#define PPS2    2   /* PPS parameter character 2 */
#define PPS3    3   /* PPS parameter character 3 */

#define PPS0_PPS1_BIT 0x10
#define PPS0_PPS2_BIT 0x20
#define PPS0_PPS3_BIT 0x40
#define PPS0_T1_BIT 0x01

typedef struct READER_PPS_Pps READER_PPS_Pps;
struct READER_PPS_Pps {
  uint8_t pps[4];
};

void READER_PPS_InitStruct(READER_PPS_Pps *pps);
void READER_PPS_Set_T1(READER_PPS_Pps *pps);
void READER_PPS_Set_PPS1(READER_PPS_Pps *pps, uint8_t TA1);
void READER_PPS_Set_PPS2(READER_PPS_Pps *pps, uint8_t val);
void READER_PPS_Set_PPS3(READER_PPS_Pps *pps, uint8_t val);
READER_Status READER_PPS_Exchange(const READER_PPS_Pps *pps, READER_HAL_CommSettings *pSettings);


#endif //__READER_PPS_H__
