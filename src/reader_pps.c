/**
 * \file reader_pps.c
 * \copyright This file is part of the Open-ISO7816-Stack project and is distributed under the MIT license. See LICENSE file in the root directory.
 * This file contains the prototypes of the functions to deal with PPS iso7815-3 9-Protocol and parameters selection .
 */

#include "reader_pps.h"
#include "reader_hal.h"

void READER_PPS_InitStruct(READER_PPS_Pps *pps)
{
  pps->pps[PPS0] = 0;
  pps->pps[PPS1] = 0;
  pps->pps[PPS2] = 0;
  pps->pps[PPS3] = 0;
}

void READER_PPS_Set_T1(READER_PPS_Pps *pps)
{
  pps->pps[PPS0] |= PPS0_T1_BIT;
}

void READER_PPS_Set_PPS1(READER_PPS_Pps *pps, uint8_t TA1)
{
  pps->pps[PPS0] |= PPS0_PPS1_BIT;
  pps->pps[PPS1] = TA1;
}

void READER_PPS_Set_PPS2(READER_PPS_Pps *pps, uint8_t val)
{
  pps->pps[PPS0] |= PPS0_PPS2_BIT;
  pps->pps[PPS1] = val;
}

void READER_PPS_Set_PPS3(READER_PPS_Pps *pps, uint8_t val)
{
  pps->pps[PPS0] |= PPS0_PPS3_BIT;
  pps->pps[PPS1] = val;
}


READER_Status READER_PPS_Exchange(const READER_PPS_Pps *pps, READER_HAL_CommSettings *pSettings)
{
  uint8_t buff[6], pck = 0xff;
  uint32_t size = 0, i, timeout = 100;
  READER_Status status;

  if(!pps->pps[PPS0]) {
    return READER_BAD_ARG;
  }
  buff[size ++] = 0xff;
  buff[size ++] = pps->pps[PPS0];
  if(pps->pps[PPS0] & PPS0_PPS1_BIT) {
    buff[size ++] = pps->pps[PPS1];
  }
  if(pps->pps[PPS0] & PPS0_PPS2_BIT) {
    buff[size ++] = pps->pps[PPS2];
  }
  if(pps->pps[PPS0] & PPS0_PPS3_BIT) {
    buff[size ++] = pps->pps[PPS3];
  }
  buff[size] = 0;
  for(i = 0; i < size; i++) {
    buff[size] ^= buff[i];
  }
  size ++;

  //Send PPS request
  status = READER_HAL_SendCharFrame(pSettings, READER_HAL_PROTOCOL_ANY, buff, size, timeout);
  if(status != READER_OK) {
    return status;
  }

  //Receive PPS response
  //PPSS
  status = READER_HAL_RcvChar(pSettings, READER_HAL_PROTOCOL_ANY, &buff[0], timeout);
  if(status != READER_OK) {
    return status;
  }
  if(buff[0] != 0xff) {
    return READER_ERR;
  }

  //PPS[0]
  status = READER_HAL_RcvChar(pSettings, READER_HAL_PROTOCOL_ANY, &buff[0], timeout);
  if(status != READER_OK) {
    return status;
  }

  size = 1; //only PCK
  if(buff[0] & PPS0_PPS1_BIT) {
    size ++;
  }
  if(buff[0] & PPS0_PPS2_BIT) {
    size ++;
  }
  if(buff[0] & PPS0_PPS3_BIT) {
    size ++;
  }

  for(i = 0; i < size; i++) {
    status = READER_HAL_RcvChar(pSettings, READER_HAL_PROTOCOL_ANY, &buff[i + 1], timeout);
    if(status != READER_OK) {
      return status;
    }
  }

  for(i = 0; i < size; i++) {
    pck ^= buff[i];
  }
  if(pck != buff[size]) {
    //Bad PCK
    return READER_ERR;
  }

  if( ((buff[0] ^ pps->pps[PPS0]) & PPS0_PPS1_BIT)
      || ((buff[0] ^ pps->pps[PPS0]) & PPS0_PPS2_BIT)
      || ((buff[0] ^ pps->pps[PPS0]) & PPS0_PPS3_BIT)
      || ((buff[0] ^ pps->pps[PPS0]) & PPS0_T1_BIT)) {
    //PPS0 not match
    return READER_ERR;
  }

  if( (pps->pps[PPS0] & PPS0_PPS1_BIT && pps->pps[PPS1] != buff[PPS1])
    || (pps->pps[PPS0] & PPS0_PPS2_BIT && pps->pps[PPS2] != buff[PPS2])
    || (pps->pps[PPS0] & PPS0_PPS2_BIT && pps->pps[PPS2] != buff[PPS2])) {
    //PPS1...PPS3 not match
    return READER_ERR;
  }
  return READER_OK;
}
