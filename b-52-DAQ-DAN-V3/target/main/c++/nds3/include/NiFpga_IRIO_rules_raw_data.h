/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_IRIO_rules_raw_data_h__
#define __NiFpga_IRIO_rules_raw_data_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_IRIO_rules_raw_data_Bitfile;
 */
#define NiFpga_IRIO_rules_raw_data_Bitfile "NiFpga_IRIO_rules_raw_data.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_IRIO_rules_raw_data_Signature = "2DBF2C9C9FC7AFFB0626AE26DF99E743";

typedef enum
{
   NiFpga_IRIO_rules_raw_data_IndicatorI16_DMAOverflow = 0x10002,
} NiFpga_IRIO_rules_raw_data_IndicatorI16;

typedef enum
{
   NiFpga_IRIO_rules_raw_data_ControlBool_stop = 0x10006,
} NiFpga_IRIO_rules_raw_data_ControlBool;

typedef enum
{
   NiFpga_IRIO_rules_raw_data_TargetToHostFifoI16_DMATOHOST0 = 0,
} NiFpga_IRIO_rules_raw_data_TargetToHostFifoI16;

#endif
