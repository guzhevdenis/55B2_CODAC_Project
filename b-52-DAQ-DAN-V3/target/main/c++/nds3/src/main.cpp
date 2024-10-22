/**
 * @file main.cpp
 *
 * @author Denis Guzhev
 * @date 12 Oct 2022
 * Labview version program 1.5
 */

#include "./../include/main.h"
#include <iostream>
#include <mutex>
#include <unistd.h>
#include<functional>
#include <fstream>
#include <nds3/nds.h>
#include <vector>
#include <string>
#include <cstddef>
#include "./../include/nifpga.hpp"
#include <niflexrioapi.h>
#include <cmath>
#include "./../include/state_machine.hpp"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <bits/stdc++.h>
#include "./../include/NiFpga_RawDataAcqusition.h"
//#include "/home/codac-dev/test_HMI-project/b-52-DAQ-DAN-V3/src/main/c++/nds3/include/NiFpga_IRIO_rules_raw_data.h"
//#include "./../include/NiFpga_main_SpectrRaw.h"
#include "/home/codac-dev/55.B2_Prototype/b-52-DAQ-DAN-V3/src/main/c++/nds3/include/NiFpga_main_v151old.h"

#define NDS_EPOCH 1514764800 /* 00:00 of 1/1/2018 in UTC format. */

//the number of elements to read from host-FIFO 
#define SIZE 800000

//Building amplitude spectrum method.
void create_histogram (std::vector<int>&a , std::vector<double>& b, std::vector<double>& c, int range);

void show(std::vector<int>& a);

//Not optimized old methods. 
//void create_range(std::vector<int>& a, int min, int max, int range);
//void count_numbers(std::vector<int>&a, std::vector<int>&b, std::vector<int>& c, int range);


//describe the functions of the mutex
//general type of mutex for working with threads 
static std::map<std::string, Device*> m_DeviceMap;

//general type of mutex for working with threads 
static std::mutex m_lockDeviceMap;
  
//constructor with all parameters
Device::Device(nds::Factory &factory, const std::string &deviceName, const nds::namedParameters_t & parameters ):
 m_name(deviceName) {

	std::lock_guard<std::mutex> lock(m_lockDeviceMap);

    if(m_DeviceMap.find(deviceName) != m_DeviceMap.end()) {
            throw std::logic_error("Device with the same name already allocated. This should not happen");
            m_DeviceMap[deviceName] = this;
    }

	/*Creation of a port-node to communicate with control system*/
	nds::Port rootNode(deviceName);

	//Initialization parameters. It is located in userPreDriverConf.cmd file
    nds::namedParameters_t::const_iterator findParam = parameters.find("INIT");

	//The parameter used to follow the parent node transitions. None - the node won't follow it's parent node transition.
	nds::autoEnable_t autoEnable = nds::autoEnable_t::none;
	
	//Transitions type of STM can also be defined in userPreDriverConf.com file.	
			if (findParam != parameters.end()) {
				if(findParam->second== "OFF"){
					autoEnable = nds::autoEnable_t::off;
				}else if(findParam->second== "ON"){
					autoEnable = nds::autoEnable_t::on;
				}else if(findParam->second== "RUNNING"){
					autoEnable = nds::autoEnable_t::running;
				}
			}
	
	/*FPGA Boolean Indicators*/
	nds::PVBaseIn Status_m = rootNode.addChild (
                    nds::PVDelegateIn<std::int32_t> ("Status", std::bind(&Device::Status_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
	nds::PVBaseIn Clock100PLLUnlocked_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("Clock100PLLUnlocked", std::bind(&Device::Clock100PLLUnlocked_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 
    nds::PVBaseIn EEPROMPowerEnabled_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("EEPROMPowerEnabled", std::bind(&Device::EEPROMPowerEnabled_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 
    nds::PVBaseIn IOModuleIOEnabled_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("IOModuleIOEnabled", std::bind(&Device::IOModuleIOEnabled_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 
    nds::PVBaseIn IOModuleInitializationDone_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("IOModuleInitializationDone", std::bind(&Device::IOModuleInitializationDone_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn IOModulePowerEnabled_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("IOModulePowerEnabled", std::bind(&Device::IOModulePowerEnabled_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn IOModulePowerGood_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("IOModulePowerGood", std::bind(&Device::IOModulePowerGood_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn IOModulePresent_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("IOModulePresent", std::bind(&Device::IOModulePresent_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	/*FPGA I8 Indicators*/
	nds::PVBaseIn DeviceTemperature_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("DeviceTemperature", std::bind(&Device::DeviceTemperature_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 

	/*FPGA  U8 Indicators*/
	nds::PVBaseIn measurement_streamstate_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("measurement_streamstate", std::bind(&Device::measurement_streamstate_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	/*FPGA  U16 Indicators*/ 
	nds::PVBaseIn Device12VPower_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("Device12VPower", std::bind(&Device::Device12VPower_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 
    nds::PVBaseIn Device33VPower_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("Device33VPower", std::bind(&Device::Device33VPower_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
	/*FPGA  U32 Indicators*/
    nds::PVBaseIn ExpectedIOModuleID_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("ExpectedIOModuleID", std::bind(&Device::ExpectedIOModuleID_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn InsertedIOModuleID_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("InsertedIOModuleID", std::bind(&Device::InsertedIOModuleID_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	/*FPGA  U64 Indicators*/
    nds::PVBaseIn SinglePulsesCountRate_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("SinglePulsesCountRate", std::bind(&Device::SinglePulsesCountRate_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn SinglePulsesRegistered_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("SinglePulsesRegistered", std::bind(&Device::SinglePulsesRegistered_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn TotalCountRate_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("TotalCountRate", std::bind(&Device::TotalCountRate_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseIn TotalPulsesRegistered_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("TotalPulsesRegistered", std::bind(&Device::TotalPulsesRegistered_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	nds::PVBaseIn measurement_streammetadata_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("measurement_streammetadata", std::bind(&Device::measurement_streammetadata_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	nds::PVBaseIn measurement_streamsamplestransferred_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("measurement_streamsamplestransferred", std::bind(&Device::measurement_streamsamplestransferred_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	/*FPGA Controls*/

	/*FPGA Bool Controls*/
    nds::PVBaseOut ThresholdValid_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("ThresholdValid", std::bind(&Device::ThresholdValid_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    nds::PVBaseOut start_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("start", std::bind(&Device::start_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	/*FPGA U8 Controls*/	
	nds::PVBaseOut measurement_streamrequeststate_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("measurement_streamrequeststate", std::bind(&Device::measurement_streamrequeststate_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
	/*FPGA I16 Controls*/
    nds::PVBaseOut Threshold_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("Threshold", std::bind(&Device::Threshold_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
	/*FPGA U16 Controls*/
    nds::PVBaseOut ChannelSelect_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("ChannelSelect", std::bind(&Device::ChannelSelect_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	/*FPGA U32 Controls*/
    nds::PVBaseOut maxticksperpulse_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("maxticksperpulse", std::bind(&Device::maxticksperpulse_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
    nds::PVBaseOut minticksperpulse_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("minticksperpulse", std::bind(&Device::minticksperpulse_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	/*FPGA U64 Controls*/
	nds::PVBaseOut measurement_streamnumsamples_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("measurement_streamnumsamples", std::bind(&Device::measurement_streamnumsamples_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	/*FIFO Indicators*/
	nds::PVBaseIn FIFO_depth_m = rootNode.addChild (
                nds::PVDelegateIn<std::int32_t> ("FIFO_depth", std::bind(&Device::FIFO_depth_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));

	nds::PVBaseIn DataBufferSize_m = rootNode.addChild (
                    nds::PVDelegateIn<std::int32_t> ("DataBufferSize", std::bind(&Device::DataBufferSize_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 

	nds::PVBaseIn Number_of_elements_m = rootNode.addChild (
                    nds::PVDelegateIn<std::int32_t> ("NumberOfElements", std::bind(&Device::Number_Of_Elements_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 

	/*Control the fpga resources*/
	nds::PVBaseOut Downloader_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("Downloader", std::bind(&Device::Downloader_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	nds::PVBaseOut Reseter_m = rootNode.addChild (
                nds::PVDelegateOut<std::int32_t> ("Reseter", std::bind(&Device::Reseter_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
		        
    nds::PVBaseOut ConfigureFIFOControl_m = rootNode.addChild (
                    nds::PVDelegateOut<std::int32_t> ("ConfigureFIFOControl", std::bind(&Device::ConfigureFIFO_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
		
	/*NDS calculation*/ 
	nds::PVBaseIn RejectionRate_m = rootNode.addChild (
                    nds::PVDelegateIn<double> ("RejectionRate", std::bind(&Device::RejectionRate_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 

	nds::PVBaseIn maximum_amplitude_m = rootNode.addChild (
    				nds::PVDelegateIn<double> ("maximum_amplitude", std::bind(&Device::maximum_amplitude_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 

	/*Amplitude spectrum FIFO*/
	/*Values of amplitude*/
	nds::PVBaseIn PulsePeaksFIFO_m = rootNode.addChild (
              		nds::PVDelegateIn<std::vector<double>> ("PulsePeaksFIFO", std::bind(&Device::PulsePeaksFIFO_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2))); 
	PulsePeaksFIFO_m.setMaxElements(100000);

	/*Counts of amplitudes inside bins*/
    nds::PVBaseIn edges_m = rootNode.addChild (
              		nds::PVDelegateIn<std::vector<double>> ("edges", std::bind(&Device::edges_Reader,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));
    edges_m.setMaxElements(100000);
      
	/*The size of the bin*/  
    nds::PVBaseOut step_m = rootNode.addChild (
                    nds::PVDelegateOut<std::int32_t> ("step", std::bind(&Device::step_Writer,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2)));


	/*WaveForm Generation Node. For testing purpose*/

    /*WAVEFORM HANDLER*/
    if (findParam != parameters.end() && findParam->second=="YES") {
        //Set compulsory methods
        nds::WaveformGenerationArgs_t<std::vector<double>> handlerWFG = nds::WaveformGenerationArgs_t<std::vector<double>>(
				std::bind(&Device::switchOn_WaveformGeneration, this),
				std::bind(&Device::switchOff_WaveformGeneration, this),
				std::bind(&Device::start_WaveformGeneration, this),
				std::bind(&Device::stop_WaveformGeneration, this),
				std::bind(&Device::recover_WaveformGeneration, this),
				std::bind(&Device::allow_WaveformGeneration_Change, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
				std::bind(&Device::PV_WaveformGeneration_Frequency_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_RefFrequency_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Amp_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Phase_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_UpdateRate_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_DutyCycle_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Gain_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Offset_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Bandwidth_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Resolution_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Impedance_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Coupling_Writer,this,   std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_SignalRef_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_SignalType_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Ground_Writer,this, std::placeholders::_1, std::placeholders::_2));

		//Set optional methods
        handlerWFG.m_DataAWG_Init = std::vector<double> (128, 4.25); //128 doubles with value 4.25
		handlerWFG.PV_Frequency_Initializer = std::bind(&Device::PV_WaveformGeneration_Frequency_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_RefFrequency_Initializer = std::bind(&Device::PV_WaveformGeneration_RefFrequency_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Amp_Initializer = std::bind(&Device::PV_WaveformGeneration_Amp_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Phase_Initializer = std::bind(&Device::PV_WaveformGeneration_Phase_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_UpdateRate_Initializer = std::bind(&Device::PV_WaveformGeneration_UpdateRate_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_DutyCycle_Initializer = std::bind(&Device::PV_WaveformGeneration_DutyCycle_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Gain_Initializer = std::bind(&Device::PV_WaveformGeneration_Gain_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Offset_Initializer = std::bind(&Device::PV_WaveformGeneration_Offset_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Bandwidth_Initializer = std::bind(&Device::PV_WaveformGeneration_Bandwidth_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Resolution_Initializer = std::bind(&Device::PV_WaveformGeneration_Resolution_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Impedance_Initializer = std::bind(&Device::PV_WaveformGeneration_Impedance_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Coupling_Initializer = std::bind(&Device::PV_WaveformGeneration_Coupling_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_SignalRef_Initializer = std::bind(&Device::PV_WaveformGeneration_SignalRef_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_SignalType_Initializer = std::bind(&Device::PV_WaveformGeneration_SignalType_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerWFG.PV_Ground_Initializer = std::bind(&Device::PV_WaveformGeneration_Ground_Initializer, this, std::placeholders::_1, std::placeholders::_2);

		//Set init values
		handlerWFG.m_Decimation_Init = 1;
        m_WaveformGeneration = rootNode.addChild(nds::WaveformGeneration<std::vector<double>>  ("WFGNode", 128 ,handlerWFG));
        //Add the WFG node
    } else {
            m_WaveformGeneration = rootNode.addChild(nds::WaveformGeneration<std::vector<double>>(
                    "WFGNode",
				128,
				std::bind(&Device::switchOn_WaveformGeneration, this),
				std::bind(&Device::switchOff_WaveformGeneration, this),
				std::bind(&Device::start_WaveformGeneration, this),
				std::bind(&Device::stop_WaveformGeneration, this),
				std::bind(&Device::recover_WaveformGeneration, this),
				std::bind(&Device::allow_WaveformGeneration_Change, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
				std::bind(&Device::PV_WaveformGeneration_Frequency_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_RefFrequency_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Amp_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Phase_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_UpdateRate_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_DutyCycle_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Gain_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Offset_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Bandwidth_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Resolution_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Impedance_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Coupling_Writer,this,   std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_SignalRef_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_SignalType_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_WaveformGeneration_Ground_Writer,this, std::placeholders::_1, std::placeholders::_2)
		    ));
    	}

	/*Data Acqusition Node*/

	findParam =  parameters.find("INIT");
	if (findParam != parameters.end() && findParam->second=="YES") {
	/*Data acqusition handler*/
		nds::DataAcquisitionArgs_t handlerDAQ = nds::DataAcquisitionArgs_t (
				std::bind(&Device::switchOn_DataAcquisition, this),
				std::bind(&Device::switchOff_DataAcquisition, this),
				std::bind(&Device::start_DataAcquisition, this),
				std::bind(&Device::stop_DataAcquisition, this),
				std::bind(&Device::recover_DataAcquisition, this),
				std::bind(&Device::allow_DataAcquisition_Change, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
				std::bind(&Device::PV_DataAcquisition_Gain_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Offset_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Bandwidth_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Resolution_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Impedance_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Coupling_Writer,this,   std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_SignalRefType_Writer,this,  std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_Ground_Writer,this, std::placeholders::_1, std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_DMAEnable_Writer,this,std::placeholders:: _1,std::placeholders::_2),
				std::bind(&Device::PV_DataAcquisition_SamplingRate_Writer,this,std::placeholders::_1,std::placeholders::_2),
				autoEnable
		);

		//Optional methods
		handlerDAQ.PV_Gain_Initializer = std::bind(&Device::PV_DataAcquisition_Gain_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Offset_Initializer = std::bind(&Device::PV_DataAcquisition_Offset_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Bandwidth_Initializer = std::bind(&Device::PV_DataAcquisition_Bandwidth_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Resolution_Initializer = std::bind(&Device::PV_DataAcquisition_Resolution_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Impedance_Initializer = std::bind(&Device::PV_DataAcquisition_Impedance_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Coupling_Initializer = std::bind(&Device::PV_DataAcquisition_Coupling_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_SignalRefType_Initializer = std::bind(&Device::PV_DataAcquisition_SignalRefType_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_Ground_Initializer = std::bind(&Device::PV_DataAcquisition_Ground_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_DMAEnable_Initializer = std::bind(&Device::PV_DataAcquisition_DMAEnable_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		handlerDAQ.PV_SamplingRate_Initializer = std::bind(&Device::PV_DataAcquisition_SamplingRate_Initializer, this, std::placeholders::_1, std::placeholders::_2);
		//Init values
		handlerDAQ.m_Decimation_Init = 1;
		handlerDAQ.m_DecimationType_Init = 2;
		m_DataAcquisition = rootNode.addChild(nds::DataAcquisition<std::vector<std::int16_t>>(
				"DataAcquisitionNode",
				SIZE,
				handlerDAQ));
	} else {
		m_DataAcquisition=rootNode.addChild(nds::DataAcquisition<std::vector<std::int16_t>>
				("DataAcquisitionNode",
						SIZE,std::
						bind(&Device::switchOn_DataAcquisition, this),
						std::bind(&Device::switchOff_DataAcquisition,this),
						std::bind(&Device::start_DataAcquisition,this),
						std::bind(&Device::stop_DataAcquisition,this),
						std::bind(&Device::recover_DataAcquisition,this),
						std::bind(&Device::allow_DataAcquisition_Change,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3),
						std::bind(&Device::PV_DataAcquisition_Gain_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Offset_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Bandwidth_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Resolution_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Impedance_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Coupling_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_SignalRefType_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_Ground_Writer,this,std::placeholders::_1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_DMAEnable_Writer,this,std::placeholders:: _1,std::placeholders::_2),
						std::bind(&Device::PV_DataAcquisition_SamplingRate_Writer,this,std::placeholders::_1,std::placeholders::_2),
						autoEnable
				));
	}
	
	/*initialization of port*/
	rootNode.initialize(this,factory);

}

//Destructor. It closes the FPGA session, releases the fifo elements, releases the name of the device.
Device::~Device(){   

	status = NiFpga_ReleaseFifoElements(session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, number_of_elements);
    NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
    status = NiFpga_Finalize();
	std::lock_guard<std::mutex> lock(m_lockDeviceMap);
	m_DeviceMap.erase(m_name);
}

/*find device function*/
Device* Device::getInstance (const std::string& deviceName) {

    std::lock_guard<std::mutex> lock(m_lockDeviceMap);
    std::map<std::string, Device*>::const_iterator findDevice =
    m_DeviceMap.find(deviceName);

    if(findDevice == m_DeviceMap.end()) {
        return 0;
    }
    return findDevice->second;
}

void* Device::allocateDevice(nds::Factory& factory, const
std::string& DeviceName, const nds::namedParameters_t& parameters) {
    return new Device(factory, DeviceName, parameters);
}

/*Deallocation function*/
void Device::deallocateDevice(void* DeviceName){
	//free the allocated memory
    m_DeviceMap.erase(((Device*)DeviceName)->m_name);
    delete (Device*)DeviceName;
}

/*Delegate function to read proccess variables*/
/*Functions for Reading variables from FPGA*/
/*Bool*/
void Device::Status_Reader(const timespec* timestamp, std::int32_t *pIn){
  *pIn = int(Device::status);
}
void Device::Clock100PLLUnlocked_Reader(const timespec* timestamp, std::int32_t *pIn){
    status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_Clock100PLLUnlocked, (NiFpga_Bool*)(pIn));
}

void Device::EEPROMPowerEnabled_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_EEPROMPowerEnabled, (NiFpga_Bool*)(pIn));
}

void Device::IOModuleIOEnabled_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_IOModuleIOEnabled, (NiFpga_Bool*)(pIn));
}

void Device::IOModuleInitializationDone_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_IOModuleInitializationDone, (NiFpga_Bool*)(pIn));
}

void Device::IOModulePowerEnabled_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_IOModulePowerEnabled, (NiFpga_Bool*)(pIn));
}

void Device::IOModulePowerGood_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_IOModulePowerGood, (NiFpga_Bool*)(pIn));
}

void Device::IOModulePresent_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadBool(session, NiFpga_main_v151_IndicatorBool_IOModulePresent, (NiFpga_Bool*)(pIn));
}

/*I8*/

void Device::DeviceTemperature_Reader(const timespec* timestamp, std::int32_t *pIn){
	//status = NiFpga_ReadI8(session, NiFpga_main_v151_IndicatorI8_DeviceTemperature, (int8_t*)(pIn));
	int8_t* help;
	if (NiFpga_IsNotError(status)) 
	{
		NiFpga_MergeStatus(&status, NiFpga_ReadI8(session,
			static_cast<int32_t>(NiFpga_main_v151_IndicatorI8_DeviceTemperature),
			help));
		*pIn = *help;
	}
}

/*U8*/
/*void Device::streamstate_Reader(const timespec* timestamp, std::int32_t *pIn)
{
	status = NiFpga_ReadU8(session, NiFpga_main_v151_IndicatorU8_streamstate0, (uint8_t*)(pIn));
}*/

void Device::measurement_streamstate_Reader(const timespec* timestamp, std::int32_t *pIn){

	/*It allows to choose the LabVIEW program*/
	if (Device::mode == 1){
		status = NiFpga_ReadU8(session, NiFpga_main_v151_IndicatorU8_streamstate, (uint8_t*)(pIn));
	} //Amplitude spectrum program
	if (Device::mode == 2){
		status = NiFpga_ReadU8(session, NiFpga_RawDataAcqusition_IndicatorU8_streamstate0 , (uint8_t*)(pIn));
	}//Raw data acqusition program
}

/*U16*/
void Device::Device12VPower_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU16(session, NiFpga_main_v151_IndicatorU16_Device12VPower, (uint16_t*)(pIn));
}

void Device::Device33VPower_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU16(session, NiFpga_main_v151_IndicatorU16_Device33VPower, (uint16_t*)(pIn));
}

/*U32*/
void Device::ExpectedIOModuleID_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU32(session, NiFpga_main_v151_IndicatorU32_ExpectedIOModuleID, (uint32_t*)(pIn));
}

void Device::InsertedIOModuleID_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU32(session, NiFpga_main_v151_IndicatorU32_InsertedIOModuleID, (uint32_t*)(pIn));
}

/*U64*/
void Device::SinglePulsesCountRate_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_SinglePulsesCountRate, (uint64_t*)(pIn));
}

void Device::SinglePulsesRegistered_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_SinglePulsesRegistered, (uint64_t*)(pIn));
}

void Device::TotalCountRate_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_TotalCountRate, (uint64_t*)(pIn));
}

void Device::TotalPulsesRegistered_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_TotalPulsesRegistered, (uint64_t*)(pIn));
}

void Device::measurement_streammetadata_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_streammetadata, (uint64_t*)(pIn));
}

void Device::measurement_streamsamplestransferred_Reader(const timespec* timestamp, std::int32_t *pIn){
	status = NiFpga_ReadU64(session, NiFpga_main_v151_IndicatorU64_streamsamplestransferred, (uint64_t*)(pIn));
}

/*Functions for Writing variables to FPGA*/
/*Bool*/
void Device::ThresholdValid_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteBool(session, NiFpga_main_v151_ControlBool_ThresholdValid, (NiFpga_Bool&)(pOut));
}
void Device::start_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteBool(session, NiFpga_main_v151_ControlBool_start, (NiFpga_Bool&)(pOut));
	std::cout<<"the Start buttom was pressed"<<std::endl;
}

/*U8*/

void Device::measurement_streamrequeststate_Writer(const timespec& timestamp, const std::int32_t & pOut){
	
	status = NiFpga_WriteU8(session, NiFpga_main_v151_ControlU8_streamrequeststate, (pOut));

	if (pOut == 0){
			m_bStop_DataAcquisition=true;
			m_DataAcquisition_Thread.join();
			//switching to idle state
			//stops data acquisition process(it can be done also through Stop method in DAQ node state machine) 
	}

	if(pOut == 2){
		status = NiFpga_ReleaseFifoElements(session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, number_of_elements);
		m_bStop_DataAcquisition=false;
		m_DataAcquisition_Thread =
	  	m_DataAcquisition.runInThread("DAQ",
					std::bind(&Device::DataAcquisition_thread_body,this));
			//switching to continious state
			//starts the data acqusition process 
	}

	/*	         
    /* "    0 - idle\n"
           "    1 - finite transfer\n"
           "    2 - continuous\n"
           "    3 - done\n"
           "    4 - error\n"
           "    5 - not ready\n"
           "    6 - cancel\n"
    */
}

/*I16*/
void Device::Threshold_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteI16(session, NiFpga_main_v151_ControlI16_Threshold, (int16_t&)(pOut));
}

/*U16*/
void Device::ChannelSelect_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteU16(session, NiFpga_main_v151_ControlU16_ChannelSelect, (uint16_t&)(pOut));
}

/*U32*/
void Device::maxticksperpulse_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteU16(session, NiFpga_main_v151_ControlU32_maxticksperpulse, (uint32_t&)(pOut));
}

void Device::minticksperpulse_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_WriteU32(session, NiFpga_main_v151_ControlU32_minticksperpulse, (uint32_t&)(pOut));
}

void Device::measurement_streamnumsamples_Writer(const timespec& timestamp, const std::int32_t & pOut){
	status = NiFpga_StartFifo (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);
	status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU64_streamnumsamples, (uint64_t&)(pOut));
}

//FIFO methods
void Device::DataBufferSize_Reader(const timespec* timestamp, std::int32_t *pIn){
  *pIn = Device::data_buffer_size;
} 

void Device::Number_Of_Elements_Reader(const timespec* timestamp, std::int32_t *pIn){
  *pIn = Device::number_of_elements;
}

void Device::FIFO_depth_Reader(const timespec* timestamp, std::int32_t *pIn){
  *pIn = Device::fifo_depth;
} 

//Configure FPGA resources methods
void Device::Downloader_Writer(const timespec& timestamp, const std::int32_t & pOut){
	/*Amplitude spectrum programm*/
   	if (pOut == 1){

		if (Device::mode == 2){
				NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
				status = NiFpga_Finalize();
		} 

		Device::mode = 1; 
        status = NiFpga_Initialize(); //Initialization of NiFpga library

			std::cout << "Amplitude Spectrum" << std::endl;
		NiFpga_MergeStatus(&status, NiFpga_Open("/home/codac-dev/test_HMI-project/b-52-DAQ-DAN-V3/src/main/c++/nds3/bitfile/vnc_main_v151old.lvbitx",
		                   NiFpga_main_v151_Signature,
		                   "RIO0",
		                    0,
		                    &session)); //download Bitfile to FPGA target

		//start controller
		NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
        NiFpga_main_v151_ControlBool_start, NiFpga_True));

		//Threshold Valid
		NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
        NiFpga_main_v151_ControlBool_ThresholdValid, NiFpga_True));

		status = NiFpga_WriteI16 (session, NiFpga_main_v151_ControlI16_Threshold, 900); //900 out of 32767. Range of ADC -1V -- 1V. (-32768 to 32767)
																						//900 -- 0.03V
		   
	    status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU64_streamnumsamples, sample_size);

	    status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU8_streamrequeststate, 0);

		status = NiFpga_StopFifo (session,NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);
		status = NiFpga_ConfigureFifo2 (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, 10*1023, &fifo_depth); //the size of host-FIFO 10 times bigger than target-FIFO
		status = NiFpga_StartFifo (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);

		//configuration of time window for pulse detection
		status = NiFpga_WriteU32(session, NiFpga_main_v151_ControlU32_maxticksperpulse, 30);
		status = NiFpga_WriteU32(session, NiFpga_main_v151_ControlU32_minticksperpulse, 10);

		status = NiFpga_WriteU16(session, NiFpga_main_v151_ControlU16_ChannelSelect, 0);
		status = 10;
		//max and min 
    }

	//RawDataAcquisition Programm
	if (pOut == 2) {	

		if(Device::mode == 1){
    			NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
    			status = NiFpga_Finalize();
		}

		Device::mode = 2;
		status = NiFpga_Initialize(); //Initialization of NiFpga library

		NiFpga_MergeStatus(&status, NiFpga_Open("/home/codac-dev/test_HMI-project/b-52-DAQ-DAN-V3/src/main/c++/nds3/bitfile/NiFpga_RawDataAcqusition.lvbitx",
		                   NiFpga_RawDataAcqusition_Signature,
		                   "RIO0",
		                    0,
		                    &session)); //download Bitfile to FPGA target

		status = NiFpga_WriteU64(session, NiFpga_RawDataAcqusition_ControlU64_streamnumsamples0, sample_size);
		status = NiFpga_WriteU64(session,  NiFpga_RawDataAcqusition_ControlU8_streamrequeststate0 , 0);

	    status = NiFpga_StopFifo (session,NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0);
		status = NiFpga_ConfigureFifo2 (session, NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0, 8191*100000, &fifo_depth); //100000 times bigger than target-FIFO
		status = NiFpga_StartFifo (session, NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0);
	}

	//RawDataAndSepctrum Programm

	/*if (pOut == 3 ) {
		
		//TODO: add switching off from other mode
	Device::mode = 3;
	status = NiFpga_Initialize();
	std::cout << "SpectrAndRaw Mode" << std::endl;

	NiFpga_MergeStatus(&status, NiFpga_Open("/home/codac-dev/55.B2_Prototype/b-52-DAQ-DAN-V3/src/main/c++/nds3/bitfile/NiFpga_main_SpectrRaw.lvbitx",
		                   "8B6E52C60259BD7D8CFB7006174C6A04",
		                   "RIO0",
		                    0,
		                    &session)); //download Bitfile to FPGA target
		//start controller
		NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
        NiFpga_main_v151_ControlBool_start, NiFpga_True));

		
		//Threshold Valid
		NiFpga_MergeStatus(&status, NiFpga_WriteBool(session,
        NiFpga_main_v151_ControlBool_ThresholdValid, NiFpga_True));

		status = NiFpga_WriteI16 (session, NiFpga_main_v151_ControlI16_Threshold, 900); //900 out of 32767. Range of ADC -1V -- 1V. (-32768 to 32767)
																						//900 -- 0.03V
		   
	    status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU64_streamnumsamples, sample_size);

	    status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU8_streamrequeststate, 0);

		status = NiFpga_StopFifo (session,NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);
		status = NiFpga_ConfigureFifo2 (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, 10*1023, &fifo_depth); //the size of host-FIFO 10 times bigger than target-FIFO
		status = NiFpga_StartFifo (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);

		//configuration of time window for pulse detection
		status = NiFpga_WriteU32(session, NiFpga_main_v151_ControlU32_maxticksperpulse, 30);
		status = NiFpga_WriteU32(session, NiFpga_main_v151_ControlU32_minticksperpulse, 10);

		status = NiFpga_WriteU16(session, NiFpga_main_v151_ControlU16_ChannelSelect, 0);
		//max and min 

		status = NiFpga_StopFifo (session,NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO);
		status = NiFpga_ConfigureFifo2 (session, NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO, 800000, &fifo_depth); //100000 times bigger than target-FIFO
		status = NiFpga_StartFifo (session, NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO);
		status = 10;
	}*/
	

}



void Device::Reseter_Writer(const timespec& timestamp, const std::int32_t & pOut){
    if (pOut == 1){
       status = NiFpga_Reset(session);
    }
}

void Device::ConfigureFIFO_Writer(const timespec& timestamp, const std::int32_t & pOut){
      if(pOut == 1){
		  fifo_depth = 1000000;
          status = NiFpga_StopFifo (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO);
          status = NiFpga_ConfigureFifo2 (session, NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, fifo_depth, &fifo_depth);    
      }
}

void Device::step_Writer(const timespec& timestamp, const std::int32_t & pOut){
      Device::step = pOut;
}


/*Waveform generation node functions*/
/*It is used for test purpose*/
void Device::PV_WaveformGeneration_Frequency_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 110};
	*value = 2.5e6; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_RefFrequency_Initializer(
		timespec* timestamp, double* value) {
	*timestamp = {NDS_EPOCH, 120};
	*value = 1.0e3; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Amp_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 130};
	*value = 3.3; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Phase_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 140};
	*value = 60.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_UpdateRate_Initializer(
		timespec* timestamp, double* value) {
	*timestamp = {NDS_EPOCH, 150};
	*value = 4.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_DutyCycle_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 160};
	*value = 75.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Gain_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 170};
	*value = 5.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Offset_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 180};
	*value = 1.1; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Bandwidth_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 190};
	*value = 20.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Resolution_Initializer(
		timespec* timestamp, double* value) {
	*timestamp = {NDS_EPOCH, 200};
	*value = 2.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Impedance_Initializer(timespec* timestamp,
		std::int32_t* value) {
	*timestamp = {NDS_EPOCH, 210};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Coupling_Initializer(timespec* timestamp,
		std::int32_t* value) {
	*timestamp = {NDS_EPOCH, 220};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_SignalRef_Initializer(timespec* timestamp,
		std::int32_t* value) {
	*timestamp = {NDS_EPOCH, 230};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_SignalType_Initializer(
		timespec* timestamp, std::int32_t* value) {
	*timestamp = {NDS_EPOCH, 240};
	*value = 1; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_WaveformGeneration_Ground_Initializer(timespec* timestamp,
		std::int32_t* value) {
	*timestamp = {NDS_EPOCH, 250};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::switchOn_WaveformGeneration(){

}

void Device::switchOff_WaveformGeneration(){

}

void Device::start_WaveformGeneration(){
	m_bStop_WaveformGeneration = false; //< We will set to true to stop the acquisition thread
	/**
	 *   Start the acquisition thread.
	 *   We don't need to check if the thread was already started because the state
	 *   machine guarantees that the start handler is called only while the state
	 *   is ON.
	 */
	m_WaveformGeneration_Thread =
	  m_WaveformGeneration.runInThread("WFG",
						  std::bind(&Device::WaveformGeneration_thread_body, this));
}

void Device::stop_WaveformGeneration(){
	m_bStop_WaveformGeneration = true;
	m_WaveformGeneration_Thread.join();
}

void Device::recover_WaveformGeneration(){
    throw nds::StateMachineRollBack("Cannot recover"); //TODO: Study this
}

bool Device::allow_WaveformGeneration_Change(const nds::state_t, const nds::state_t, const nds::state_t){
	return true;
}

/*
* WaveformGeneration setters
*/
void Device::PV_WaveformGeneration_Frequency_Writer(const timespec& /*timestamp*/, const double& value){
	double HW_value;
	//Value has the frequency to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real frequency programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setFrequency(m_WaveformGeneration.getTimestamp(),HW_value);
}
void Device::PV_WaveformGeneration_RefFrequency_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the RefFrequency to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real RefFrequency programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setRefFrequency(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Amp_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//value has the amplitude to be programmed on the hardware
	//call to function programming the hardware. This function should return the real amplitude programmed. This value has to be set to the readback attribute.
	//in the meantime without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setAmplitude(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Phase_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the Phase to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Phase programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setPhase(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_UpdateRate_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the UpdateRate to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real UpdateRate programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setUpdateRate(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_DutyCycle_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the DutyCycle to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real DutyCycle programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setDutyCycle(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Gain_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the Gain to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Gain programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setGain(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Offset_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the Offset to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Offset programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setOffset(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Bandwidth_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the Bandwidth to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Bandwidth programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setBandwidth(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Resolution_Writer(const timespec& timestamp, const double& value){
	double HW_value;
	//Value has the Resolution to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Resolution programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setResolution(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Impedance_Writer(const timespec& timestamp, const std::int32_t& value){
	std::int32_t HW_value;
	//Value has the Impedance to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Impedance programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setImpedance(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Coupling_Writer(const timespec& timestamp, const std::int32_t& value){
	std::int32_t HW_value;
	//Value has the Coupling to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Coupling programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setCoupling(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_SignalRef_Writer(const timespec& timestamp, const std::int32_t& value){
	std::int32_t HW_value;
	//Value has the SignalRef to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real SignalRef programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setSignalRef(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_SignalType_Writer(const timespec& timestamp, const std::int32_t& value){
	std::int32_t HW_value;
	//value has the SignalType to be programmed on the hardware
	//call to function programming the hardware. This function should return the real SignalType programmed. This value has to be set to the readback attribute.
	//in the meantime without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setSignalType(timestamp,HW_value);
}
void Device::PV_WaveformGeneration_Ground_Writer(const timespec& timestamp, const std::int32_t& value){
	std::int32_t HW_value;
	//value has the Ground to be programmed on the hardware
	//call to function programming the hardware. This function should return the real Ground programmed. This value has to be set to the readback attribute.
	//in the meantime without real hardware value and  HW_value are equal.
	HW_value=value;
	m_WaveformGeneration.setGround(timestamp,HW_value);
}

/*The first argument is DRIVER_NAME, the second is the name of the class.
  The DRIVER_NAME is used in ndsCreateDevice function as the first argument*/
/*
* Body of function to generate data. In this example we are going to generate a sine wave.
*/
void Device::WaveformGeneration_thread_body(){

	// Let's allocate a vector that will contain the data that we will push to the control system or to the data acquisition node
	std::vector<double> outputData(m_WaveformGeneration.getMaxElements(),0);

	//Counter for number of pushed data blocks
	std::int32_t NumberOfPushedDataBlocks(0);

	// A counter for the angle in the sin() operation
	std::int64_t angle(0);

	size_t last_sample(0);

    timespec timestamp =  {NDS_EPOCH, 240};
	// Get RefFrequency
	double RefFrequency = m_WaveformGeneration.getRefFrequency();
	// Get DutyCycle
	double DutyCycle = m_WaveformGeneration.getDutyCycle();
	// Get Gain
	double Gain = m_WaveformGeneration.getGain();
	// Get Bandwidth
	double Bandwidth = m_WaveformGeneration.getBandwidth();
	// Get Resolution
	double Resolution = m_WaveformGeneration.getResolution();
	// Get Coupling
	double Coupling = m_WaveformGeneration.getCoupling();
	// Get SignalRef
	double SignalRef = m_WaveformGeneration.getSignalRef();
	// Get Ground
	double Ground = m_WaveformGeneration.getGround();
	// Get impedance
	double impedance = m_WaveformGeneration.getImpedance();

	std::cout<<"Signal generator configured with:"<<std::endl;
	std::cout<<"\tRefFrequency = "<<RefFrequency<<std::endl;
	std::cout<<"\tDutyCycle = "<<DutyCycle<<std::endl;
	std::cout<<"\tGain = "<<Gain<<std::endl;
	std::cout<<"\tBandwidth = "<<Bandwidth<<std::endl;
	std::cout<<"\tResolution = "<<Resolution<<std::endl;
	std::cout<<"\tCoupling = "<<Coupling<<std::endl;
	std::cout<<"\tSignalRef = "<<SignalRef<<std::endl;
	std::cout<<"\tGround = "<<Ground<<std::endl;
	std::cout<<"\tImpedance = "<<impedance<<std::endl;

	// Run until the state machine stops us
	while(!m_bStop_WaveformGeneration){

		size_t scanVector(0);
		// Get signalType
		size_t signalType = m_WaveformGeneration.getSignalType();
		// Get amplitude
		double amplitude = m_WaveformGeneration.getAmplitude();
		// Get frequency
		double frequency = m_WaveformGeneration.getFrequency();
		double period = 1/frequency;
		// Get updateRate
		double updateRate = m_WaveformGeneration.getUpdateRate();
		// Get offset
		double offset = m_WaveformGeneration.getOffset();
		// Get phase
		double phase = m_WaveformGeneration.getPhase();

		switch(signalType){

			case 0:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				last_sample+=scanVector;
				break;
			case 1:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				break;
			case 2:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				break;
			//SINE WAVE
			case 3:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = (double)amplitude * sin((2*M_PI*(scanVector+last_sample)*frequency)/updateRate + phase) + offset;
				}
				break;
			//SAW
			case 4:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = ((angle & 0xff) < 128) ? amplitude : - amplitude;
				}
				break;
			//TRIANGLE
			case 5:	
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){		
					outputData[scanVector] = (amplitude*4/period) * abs((((scanVector+last_sample) - int(period)/2))%int(period)-period/2) - amplitude + offset;
				}
				break;
			case 6:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				break;
			case 7:
				for(scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				break;
			default:
				for( scanVector=0; scanVector != outputData.size(); ++scanVector){
					outputData[scanVector] = amplitude;
				}
				break;
		}

		//Save last sample generated.
		last_sample+=scanVector;

		// Push the vector to the control system
		m_WaveformGeneration.push(m_WaveformGeneration.getTimestamp(), outputData);
		++NumberOfPushedDataBlocks;
		//TODO: Send values to data acquisition node.

		// Rest for a while
		::usleep(100000);
	}

	m_WaveformGeneration.setNumberOfPushedDataBlocks(m_WaveformGeneration.getTimestamp(),NumberOfPushedDataBlocks);
}


/*Data Acquisiton node methods*/
void Device::switchOn_DataAcquisition() {
}

void Device::switchOff_DataAcquisition() {
}

void Device::start_DataAcquisition() {

	if (Device::mode == 1){
		status = NiFpga_WriteU8(session, NiFpga_main_v151_ControlU8_streamrequeststate, 2);
	} // it starts acquisition procces of Amplitude spectrum program
	if (Device::mode == 2){
		status = NiFpga_WriteU8(session, NiFpga_RawDataAcqusition_ControlU8_streamrequeststate0, 2);
	} // it starts process of Raw data acquisition program

	m_bStop_DataAcquisition=false; //starts the cycle 

	m_DataAcquisition_Thread =
	  m_DataAcquisition.runInThread("DAQ",
					std::bind(&Device::DataAcquisition_thread_body,this)); // runs the thread in thread body method 
}

void Device::stop_DataAcquisition() {

	m_bStop_DataAcquisition=true;
	if (Device::mode == 1){
		status = NiFpga_WriteU64(session, NiFpga_main_v151_ControlU8_streamrequeststate,0);
	} //it stops acquisition process of Amplitude spectrum program 

	if (Device::mode == 2){
		status = NiFpga_WriteU8(session, NiFpga_RawDataAcqusition_ControlU8_streamrequeststate0, 0);
	} //it stops process of Raw data acquisition program
	m_DataAcquisition_Thread.join();
}

void Device::recover_DataAcquisition() {
	throw nds::StateMachineRollBack("Cannot recover");
}

bool Device::allow_DataAcquisition_Change(const nds::state_t , const nds::state_t , const nds::state_t ) {
	return true;
}

void Device::DataAcquisition_thread_body() {

	/*optional methods*/
	// Get Gain
	double Gain = m_DataAcquisition.getGain();
	// Get Bandwidth
	double Bandwidth = m_DataAcquisition.getBandwidth();
	// Get Resolution
	double Resolution = m_DataAcquisition.getResolution();
	// Get Coupling
	double Coupling = m_DataAcquisition.getCoupling();
	// Get SignalRefType
	double SignalRefTYpe = m_DataAcquisition.getSignalRefType();
	// Get Ground
	double Ground = m_DataAcquisition.getGround();
	// Get offset
	double Offset = m_DataAcquisition.getOffset();
	// Get impedance
	std::int32_t Impedance = m_DataAcquisition.getImpedance();
	// Get SamplingRate
	double SamplingRate = m_DataAcquisition.getSamplingRate();

	std::cout<<"\tGain = "<< Gain<<std::endl;
	std::cout<<"\tBandwidth = "<< Bandwidth <<std::endl;
	std::cout<<"\tResolution = "<< Resolution <<std::endl;
	std::cout<<"\tCoupling = "<<Coupling<<std::endl;
	std::cout<<"\tSignalRefType = "<<SignalRefTYpe<<std::endl;
	std::cout<<"\tGround = "<<Ground<<std::endl;
	std::cout<<"\tOffset = "<<Offset<<std::endl;
	std::cout<<"\tImpedance = "<<Impedance<<std::endl;
	std::cout<<"\tSamplingRate = "<<SamplingRate<<std::endl;

	std::int16_t* outputdata;
	std::vector<int16_t> data;
	data.reserve(SIZE);
	data.resize(SIZE);

	// Run until the state machine stops us

	if(Device::mode == 1)

	{
		while(!m_bStop_DataAcquisition){
			
		std::int32_t NumberOfPushedDataBlocks(0);
		std::vector<int32_t> data_buffer;
		

        size_t elements_requested = 1023;
        size_t elements_acquired = 0;
        size_t elements_remaining = 0;

		data_buffer.reserve(100000);
        size_t fifo_max_reserve = fifo_depth / 2;                                         
		status = NiFpga_Status_Success;

		std::thread fifo_reader([&]() {
                    					status = NiFpga_AcquireFifoReadElementsI16(session,
                     							NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, &outputdata, elements_requested, 5000, 
												&elements_acquired, &elements_remaining); 
				
                   						number_of_elements = elements_acquired;
                   
				  						data_buffer.insert(data_buffer.end(), outputdata, outputdata+elements_acquired);
										status = NiFpga_ReleaseFifoElements(session,
													NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, elements_acquired);
		});

				fifo_reader.join(); 

                create_histogram (data_buffer, amplitudes,  numbers, Device::step);
	
				if (amplitudes.size() >= 10000000)
				{
					amplitudes.clear();
					numbers.clear();
				}
	}

	//m_DataAcquisition.setNumberOfPushedDataBlocks(m_DataAcquisition.getTimestamp(),NumberOfPushedDataBlocks);
	}

	if (Device::mode == 2)
	{
			size_t elements_requested = SIZE; //theoretically 1ms of acquisition
		 
           	size_t elements_acquired = 0;
           	size_t elements_remaining = 0;

			std::int16_t* rawdata;

			rawdata = (std::int16_t*)malloc(elements_requested*2);

			memset(rawdata, 0, elements_requested*2);


			while(!m_bStop_DataAcquisition){
			
					status = NiFpga_AcquireFifoReadElementsI16(session,
                    NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0, &rawdata, elements_requested, 5000, 
					&elements_acquired, &elements_remaining);
					
					//number_of_elements = elements_acquired;

				  	memcpy(data.data(), rawdata, elements_acquired*2);
					
					//std::cout << data[6] <<std::endl;	
					//for (int k = 0; k < elements_acquired; k++)
					//{
					//	data.push_back(*(rawdata+k));
					//}
					//data.insert(data.begin(), data.data(), data.data()+100);
					//data.shrink_to_fit();
					//it have to be checked properly for DAN connection
					//usleep(0.0001);

					m_DataAcquisition.push(m_DataAcquisition.getTimestamp(), data);
					
					status = NiFpga_ReleaseFifoElements(session, NiFpga_RawDataAcqusition_TargetToHostFifoI16_DMATtoHOST0, elements_acquired);
					data.clear();

			}
	}

	//Amplitude spectrum and Raw Data acquisition mode 
	/*if (Device::mode == 3)
	{
		while(!m_bStop_DataAcquisition){
			
		std::int32_t NumberOfPushedDataBlocks(0);
		std::vector<int32_t> data_buffer;
		

        size_t elements_requested = 1023;
        size_t elements_acquired = 0;
        size_t elements_remaining = 0;

		data_buffer.reserve(100000);
        size_t fifo_max_reserve = fifo_depth / 2;                                         
		status = NiFpga_Status_Success;

			size_t elements_requested_raw = SIZE; //theoretically 1ms of acquisition
		 
           	size_t elements_acquired_raw = 0;
           	size_t elements_remaining_raw = 0;

			std::int16_t* rawdata;

			rawdata = (std::int16_t*)malloc(elements_requested*2);

			memset(rawdata, 0, elements_requested*2);


		std::thread fifo_reader([&]() {
                    					status = NiFpga_AcquireFifoReadElementsI16(session,
                     							NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, &outputdata, elements_requested, 5000, 
												&elements_acquired, &elements_remaining); 
				
                   						number_of_elements = elements_acquired;
                   
				  						data_buffer.insert(data_buffer.end(), outputdata, outputdata+elements_acquired);
										status = NiFpga_ReleaseFifoElements(session,
													NiFpga_main_v151_TargetToHostFifoI16_PulsePeaksFIFO, elements_acquired);

													status = NiFpga_AcquireFifoReadElementsI16(session,
                    NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO, &rawdata, elements_requested_raw, 5000, 
					&elements_acquired_raw, &elements_remaining_raw);
					
					//number_of_elements = elements_acquired;

				  	memcpy(data.data(), rawdata, elements_acquired_raw*2);
					
					//std::cout << data[6] <<std::endl;	
					//for (int k = 0; k < elements_acquired; k++)
					//{
					//	data.push_back(*(rawdata+k));
					//}
					//data.insert(data.begin(), data.data(), data.data()+100);
					//data.shrink_to_fit();
					//it have to be checked properly for DAN connection
					//usleep(0.0001);

					m_DataAcquisition.push(m_DataAcquisition.getTimestamp(), data);
					
					status = NiFpga_ReleaseFifoElements(session, NiFpga_main_v151_TargetToHostFifoI16_ToHostFIFO, elements_acquired_raw);
					data.clear();
		});

				fifo_reader.join(); 

                create_histogram (data_buffer, amplitudes,  numbers, Device::step);
	
				if (amplitudes.size() >= 10000)
				{
					amplitudes.clear();
					numbers.clear();
				}
	}	
			
			
	}*/
}

void vector_formalization(std::vector<std::int16_t>& data, int numberOfElements){
	data.insert(data.begin(), data.data(), data.data() + numberOfElements);
} 

void Device::PV_DataAcquisition_Gain_Writer(const timespec& timestamp,
	const double& value) {
	double HW_value;
	HW_value=value;
	m_DataAcquisition.setGain(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Offset_Writer(const timespec& timestamp,
		const double& value) {
	double HW_value;
	//Value has the Offset to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Offset programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setOffset(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Bandwidth_Writer(const timespec& timestamp,
		const double& value) {
	double HW_value;
	//Value has the Bandwidth to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Bandwidth programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setBandwidth(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Resolution_Writer(const timespec& timestamp,
		const double& value) {
	double HW_value;
	//Value has the Resolution to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Resolution programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setResolution(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Impedance_Writer(const timespec& timestamp,
		const double& value) {
	double HW_value;
	//Value has the Impedance to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Impedance programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setImpedance(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Coupling_Writer(const timespec& timestamp,
		const std::int32_t& value) {
	std::int32_t HW_value;
	//Value has the Coupling to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Coupling programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setCoupling(timestamp,HW_value);
}

void Device::PV_DataAcquisition_SignalRefType_Writer(const timespec& timestamp,
		const std::int32_t& value) {
	std::int32_t HW_value;
	//Value has the SignalRefType to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real SignalRefType programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setSignalRefType(timestamp,HW_value);
}
void Device::PV_DataAcquisition_Ground_Writer(const timespec& timestamp,
		const std::int32_t& value) {
	std::int32_t HW_value;
	//Value has the Ground to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real Ground programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setGround(timestamp,HW_value);
}


void Device::PV_DataAcquisition_DMAEnable_Writer(const timespec& timestamp,
		const std::int32_t& value) {
	std::int32_t HW_value;
	//Value has the DMAEnable value to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real DMAEnable value programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setDMAEnable(timestamp,HW_value);
}

void Device::PV_DataAcquisition_SamplingRate_Writer(const timespec& timestamp,
		const double& value) {
	double HW_value;
	//Value has the SamplingRate to be programmed on the hardware.
	//Call to function programming the hardware. This function should return the real SamplingRate programmed. This value has to be set to the readback attribute.
	//In the meantime, without real hardware value and  HW_value are equal.
	HW_value=value;
	m_DataAcquisition.setSamplingRate(timestamp,HW_value);
}

void Device::PV_DataAcquisition_Gain_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 10};
	*value = 2.5; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_Offset_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 20};
	*value = 3.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_Bandwidth_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 30};
	*value = 10.0; //Note that this value has no sense and it is fixed only for testing purposes.

}

void Device::PV_DataAcquisition_Resolution_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 40};
	*value = 15.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_Impedance_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 50};
	*value = 2.2; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_Coupling_Initializer(timespec* timestamp,
		int32_t* value) {
	*timestamp = {NDS_EPOCH, 60};
	*value = 1; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_SignalRefType_Initializer(
		timespec* timestamp, int32_t* value) {
	*timestamp = {NDS_EPOCH, 70};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_Ground_Initializer(timespec* timestamp,
		int32_t* value) {
	*timestamp = {NDS_EPOCH, 80};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_DMAEnable_Initializer(timespec* timestamp,
		int32_t* value) {
	*timestamp = {NDS_EPOCH, 90};
	*value = 0; //Note that this value has no sense and it is fixed only for testing purposes.
}

void Device::PV_DataAcquisition_SamplingRate_Initializer(timespec* timestamp,
		double* value) {
	*timestamp = {NDS_EPOCH, 100};
	*value = 10.0; //Note that this value has no sense and it is fixed only for testing purposes.
}

//Additional calculation

/*Number of rejections*/
  void Device::RejectionRate_Reader(const timespec* timestamp, double *pIn){
	  if(NiFpga_IsNotError(status)){
        std::uint64_t *total = 0;
        std::uint64_t *single = 0;
        NiFpga_MergeStatus(&status, NiFpga_ReadU64(session,
                            static_cast<std::uint64_t>( NiFpga_main_v151_IndicatorU64_TotalCountRate), 
                             total));
        NiFpga_MergeStatus(&status, NiFpga_ReadU64(session,
                            static_cast<std::uint64_t>(NiFpga_main_v151_IndicatorU64_SinglePulsesCountRate), 
                            single));
        //calculation of Rejection rate    
		if (total != 0){
			*pIn = ((*total)-(*single) / (*total)) * 100;
		}
		else{
			*pIn = 0;
		}
    }
  }

//Amplitude spectrum calculation. 
/*https://stackoverflow.com/questions/49458662/calculate-histogram-from-a-set-of-data-using-the-standard-library-or-the-boost-l*/

/*Making of distribution from PulsePeaks FIFO*/ 
/*range - bin width, can be changed in data acquisition thread body*/ 
  void create_histogram (std::vector<int>&a , std::vector<double>& b, std::vector<double>& c, int range){

	if (a.empty() != true){
		std::sort(a.begin(), a.end());
		std::map<int, int> histogram;

		double bin = 0; //Choose your starting bin
		const double bin_width = range; //Choose your bin interval

		for (const auto& e : a){
			e >= bin + bin_width ? bin += bin_width : false;
			++histogram[bin];
		}

		for (const auto& x : histogram){
			b.push_back((x.first + bin_width)/32767);
			c.push_back(x.second);					
		}
	}
}

//for test purpose
void show(std::vector<int>& a){
    for (int i = 0; i < a.size(); ++i ){
        std::cout<<a[i]<<" ";
    }
    std::cout<<std::endl;
}

/*Not optimized old methods*/
/*
void create_range(std::vector<int>& a, int min, int max, int range){
    for (int i = min; i <= max; i = i + range)
    {
        a.push_back(i);
    }
}

void count_numbers(std::vector<int>&a, std::vector<int>&b, std::vector<int>& c, int range){
    //int range = b[1]-b[0];
    for (int i = 0; i < b.size(); ++i)
    {
        int counter = 0;
        for (int j = 0; j < range; ++j)
        {
            counter = counter + std::count(a.begin(), a.end(),b[i]+j); //optimize it
        }
        c.push_back(counter);
    }
}
*/

/*Amplitudes readings*/
void Device::PulsePeaksFIFO_Reader (const timespec* timestamp, std::vector<double> *pIn){
     *pIn = amplitudes;
}

/*Counts reading*/
void Device::edges_Reader (const timespec* timestamp, std::vector<double> *pIn){
  *pIn = numbers;
}

/*Maximum amplitude calculation*/
void Device::maximum_amplitude_Reader (const timespec* timestamp, double *pIn){
	*pIn = *(std::max_element(amplitudes.begin(), amplitudes.end()));
}

NDS_DEFINE_DRIVER(Device, Device)
