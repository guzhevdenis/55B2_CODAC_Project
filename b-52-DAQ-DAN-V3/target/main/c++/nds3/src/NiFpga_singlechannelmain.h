/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_singlechannelmain_h__
#define __NiFpga_singlechannelmain_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_singlechannelmain_Bitfile;
 */
#define NiFpga_singlechannelmain_Bitfile "NiFpga_singlechannelmain.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_singlechannelmain_Signature = "33E328438B060871C6869B2BCB185D06";

typedef enum
{
   NiFpga_singlechannelmain_IndicatorU8_streamstate0 = 0x8001000E,
} NiFpga_singlechannelmain_IndicatorU8;

typedef enum
{
   NiFpga_singlechannelmain_IndicatorU64_streamsamplestransferred0 = 0x80010000,
} NiFpga_singlechannelmain_IndicatorU64;

typedef enum
{
   NiFpga_singlechannelmain_ControlU8_streamrequeststate0 = 0x8001000A,
} NiFpga_singlechannelmain_ControlU8;

typedef enum
{
   NiFpga_singlechannelmain_ControlU64_streamnumsamples0 = 0x80010004,
} NiFpga_singlechannelmain_ControlU64;

typedef enum
{
   NiFpga_singlechannelmain_TargetToHostFifoI16_FIFO = 0,
} NiFpga_singlechannelmain_TargetToHostFifoI16;

#endif
