/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_RawDataAcqusition_h__
#define __NiFpga_RawDataAcqusition_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_RawDataAcqusition_Bitfile;
 */
#define NiFpga_RawDataAcqusition_Bitfile "NiFpga_RawDataAcqusition.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_RawDataAcqusition_Signature = "55A0AF73AC278A37B1E54EFC527C3824";

typedef enum
{
   NiFpga_RawDataAcqusition_IndicatorU8_streamstate0 = 0x8001000E,
} NiFpga_RawDataAcqusition_IndicatorU8;

typedef enum
{
   NiFpga_RawDataAcqusition_IndicatorU64_streamsamplestransferred0 = 0x80010000,
} NiFpga_RawDataAcqusition_IndicatorU64;

typedef enum
{
   NiFpga_RawDataAcqusition_ControlU8_streamrequeststate0 = 0x8001000A,
} NiFpga_RawDataAcqusition_ControlU8;

typedef enum
{
   NiFpga_RawDataAcqusition_ControlU64_streamnumsamples0 = 0x80010004,
} NiFpga_RawDataAcqusition_ControlU64;

typedef enum
{
   NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0 = 0,
} NiFpga_RawDataAcqusition_TargetToHostFifoI16;

#endif
