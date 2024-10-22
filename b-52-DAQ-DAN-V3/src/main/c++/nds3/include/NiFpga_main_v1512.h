/*
 * Generated with the FPGA Interface C API Generator 18.0.0
 * for NI-RIO 18.0.0 or later.
 */

#ifndef __NiFpga_main_v1512_h__
#define __NiFpga_main_v1512_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1800
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_main_v1512_Bitfile;
 */
#define NiFpga_main_v1512_Bitfile "NiFpga_main_v1512.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_main_v1512_Signature = "8B6E52C60259BD7D8CFB7006174C6A04";

typedef enum
{
   NiFpga_main_v151_IndicatorBool_Clock100PLLUnlocked = 0x1003E,
   NiFpga_main_v151_IndicatorBool_EEPROMPowerEnabled = 0x1004E,
   NiFpga_main_v151_IndicatorBool_IOModuleIOEnabled = 0x1005A,
   NiFpga_main_v151_IndicatorBool_IOModuleInitializationDone = 0x1006A,
   NiFpga_main_v151_IndicatorBool_IOModulePowerEnabled = 0x1005E,
   NiFpga_main_v151_IndicatorBool_IOModulePowerGood = 0x10062,
   NiFpga_main_v151_IndicatorBool_IOModulePresent = 0x10066,
} NiFpga_main_v151_IndicatorBool;

typedef enum
{
   NiFpga_main_v151_IndicatorI8_DeviceTemperature = 0x1004A,
} NiFpga_main_v151_IndicatorI8;

typedef enum
{
   NiFpga_main_v151_IndicatorU8_streamstate = 0x80010002,
} NiFpga_main_v151_IndicatorU8;

typedef enum
{
   NiFpga_main_v151_IndicatorU16_Device12VPower = 0x10046,
   NiFpga_main_v151_IndicatorU16_Device33VPower = 0x10042,
} NiFpga_main_v151_IndicatorU16;

typedef enum
{
   NiFpga_main_v151_IndicatorU32_ExpectedIOModuleID = 0x10050,
   NiFpga_main_v151_IndicatorU32_InsertedIOModuleID = 0x10054,
} NiFpga_main_v151_IndicatorU32;

typedef enum
{
   NiFpga_main_v151_IndicatorU64_SinglePulsesCountRate = 0x10018,
   NiFpga_main_v151_IndicatorU64_SinglePulsesRegistered = 0x10030,
   NiFpga_main_v151_IndicatorU64_TotalCountRate = 0x10024,
   NiFpga_main_v151_IndicatorU64_TotalPulsesRegistered = 0x1002C,
   NiFpga_main_v151_IndicatorU64_streammetadata = 0x80010008,
   NiFpga_main_v151_IndicatorU64_streamsamplestransferred = 0x80010004,
} NiFpga_main_v151_IndicatorU64;

typedef enum
{
   NiFpga_main_v151_ControlBool_ThresholdValid = 0x10022,
   NiFpga_main_v151_ControlBool_start = 0x1003A,
} NiFpga_main_v151_ControlBool;

typedef enum
{
   NiFpga_main_v151_ControlU8_streamrequeststate = 0x8001000E,
} NiFpga_main_v151_ControlU8;

typedef enum
{
   NiFpga_main_v151_ControlI16_Threshold = 0x1001E,
} NiFpga_main_v151_ControlI16;

typedef enum
{
   NiFpga_main_v151_ControlU16_ChannelSelect = 0x80010016,
} NiFpga_main_v151_ControlU16;

typedef enum
{
   NiFpga_main_v151_ControlU32_maxticksperpulse = 0x10028,
   NiFpga_main_v151_ControlU32_minticksperpulse = 0x10034,
} NiFpga_main_v151_ControlU32;

typedef enum
{
   NiFpga_main_v151_ControlU64_streamnumsamples = 0x80010010,
} NiFpga_main_v151_ControlU64;

typedef enum
{
   NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO = 1,
   NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO = 0,
} NiFpga_main_v151_TargetToHostFifoI16;

#endif
