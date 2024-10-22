/**
 * @file main.h
 *
 * @author Denis Guzhev
 * @date 12 Oct 2022
 * Labview version program 1.5
 */

#ifndef VNC_H_
#define VNC_H_

#include <nds3/nds.h>
#include "NiFpga.h"
//#include "./../include/NiFpga_main_v151.h" 
//#include "nifpga.hpp"
//#include "state_machine.hpp"


const char* bitfile = "NiFpga_singlechannelmain.lvbitx";
const char* signature = "33E328438B060871C6869B2BCB185D06"; 
const char* device = "RIO0";

//uint32_t control   = NiFpga_singlechannelmain_ControlU8_streamrequeststate0;
// /uint32_t indicator = NiFpga_singlechannelmain_IndicatorU8_streamstate0;

//const uint32_t dma_fifo =
//        NiFpga_singlechannelmain_TargetToHostFifoI16_FIFO;

        class Device

        {
            public: 
            //constructor
            Device(nds::Factory& factory,
                   const std::string& deviceName,
                   const nds::namedParameters_t&);
             //destructor
            ~Device();
    
            //LabVIEW FPGA session and status parameters 
            NiFpga_Session session; 
            NiFpga_Status status = -1;

            //undrelaying FPGA session
            //methods for reading indicators
            //std::int32_t is supported by AsynDriver
           /* void State_Reader(const timespec* timestamp, std::int32_t *pIn);
            void TransferingToFile_Reader(const timespec* timestamp, std::int32_t *pIn);
            void DataBufferSize_Reader(const timespec* timestamp, std::int32_t *pIn);
            void Number_Of_Elements_Reader (const timespec* timestamp, std::int32_t *pIn);
            void Status_Reader (const timespec* timestamp, std::int32_t *pIn);
            void FIFO_depth_Reader (const timespec* timestamp, std::int32_t *pIn);
            void StreamSample_Reader(const timespec* timestamp, std::int32_t *pIn);
            void OutFileName_Reader(const timespec* timestamp, std::string *pIn);
            void testSignal_Reader(const timespec* timestamp, std::vector<double> *pIn);

            //methods for writing to controllers
            void State_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void Downloader_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void Reseter_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void StartRoutine_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void StreamSample_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void OutFileName_Writer(const timespec& timestamp, const std::string & pOut);
            void ConfigureFIFO_Writer (const timespec& timestamp, const std::int32_t & pOut);
            */
            //special indicators

            //Allocation method
            static void* allocateDevice(nds::Factory& factory, const
            std::string& deviceName, const nds::namedParameters_t&
            parameters);
            //getDevice function 
            Device* getInstance (const std::string& deviceName);
            //Deallocation method
            static void deallocateDevice(void* deviceName);

            /*size_t sample_size = 10000000;
            size_t fifo_depth = 10485760;
            size_t data_buffer_size = 0;
            size_t number_of_elements = 0;*/
            /*
            std::string out_directory = std::string("/home/codac-dev/NDS_projects/nds-pcf/daq_stream/nds3/recordings/");
            std::string out_file_name = std::string("recording.bin");
            std::string output = std::string("test");
            
            //ni::fpga::status status = NiFpga_Status_Success;

            std::int32_t status_of_file_recording = -1;
            */
            private:
            //Device name
            std::string m_name; 

            /*Data Generation node*/
            nds::WaveformGeneration<std::vector<double>> m_WaveformGeneration;
            /**
             * Methods to control WaveformGeneration state machine
             */
            void switchOn_WaveformGeneration();  ///< Called to switch on the WaveformGeneration node.
            void switchOff_WaveformGeneration(); ///< Called to switch off the WaveformGeneration node.
            void start_WaveformGeneration();     ///< Called to start the WaveformGeneration node.
            void stop_WaveformGeneration();      ///< Called to stop the WaveformGeneration node.
            void recover_WaveformGeneration();   ///< Called to recover the WaveformGeneration node from a failure.

            bool allow_WaveformGeneration_Change(const nds::state_t, const nds::state_t, const nds::state_t); ///< Called to verify if a state change is allowed

            /**
             * WaveformGeneration setters
             */
            void PV_WaveformGeneration_Frequency_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_RefFrequency_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Amp_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Phase_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_UpdateRate_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_DutyCycle_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Gain_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Offset_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Bandwidth_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Resolution_Writer(const timespec& timestamp, const double& value);
            void PV_WaveformGeneration_Impedance_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_WaveformGeneration_Coupling_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_WaveformGeneration_SignalRef_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_WaveformGeneration_SignalType_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_WaveformGeneration_Ground_Writer(const timespec& timestamp, const std::int32_t& value);

            /**
             * WaveformGeneration Initializers
             */
            void PV_WaveformGeneration_Frequency_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_RefFrequency_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Amp_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Phase_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_UpdateRate_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_DutyCycle_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Gain_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Offset_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Bandwidth_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Resolution_Initializer(timespec* timestamp, double* value);
            void PV_WaveformGeneration_Impedance_Initializer(timespec* timestamp, std::int32_t* value);
            void PV_WaveformGeneration_Coupling_Initializer(timespec* timestamp, std::int32_t* value);
            void PV_WaveformGeneration_SignalRef_Initializer(timespec* timestamp, std::int32_t* value);
            void PV_WaveformGeneration_SignalType_Initializer(timespec* timestamp, std::int32_t* value);
            void PV_WaveformGeneration_Ground_Initializer(timespec* timestamp, std::int32_t* value);


            /**
             * @brief Function that continuously generates a sinusoidal wave to m_WaveformGeneration.
             *        It is launched by start_WaveformGeneration() in a separate thread.
             */
            void WaveformGeneration_thread_body();

            /**
             * @brief A thread that runs WaveformGeneration_thread_body().
             */
            nds::Thread m_WaveformGeneration_Thread;

            /**
             * @brief A boolean flag that stop the WaveformGeneration loop in WaveformGeneration_thread_body()
             *        when true.
             */
            volatile bool m_bStop_WaveformGeneration;

                /*Data Acquisition node*/
            nds::DataAcquisition<std::vector<std::int16_t>> m_DataAcquisition;

                        //Methods to control DataAcquisition Node state machine.
            void switchOn_DataAcquisition();  //Called to switch on the DataAcquisition node.
            void switchOff_DataAcquisition(); //Called to switch off the DataAcquisition node.
            void start_DataAcquisition();     //Called to start the DataAcquisition node.
            void stop_DataAcquisition();      //Called to stop the DataAcquisition node.
            void recover_DataAcquisition();   //Called to recover the DataAcquisition node from a failure.
            bool allow_DataAcquisition_Change(const nds::state_t, const nds::state_t, const nds::state_t); //Called to verify if a state change is allowed

                //DataAcquisition setters
            void PV_DataAcquisition_Gain_Writer(const timespec& timestamp, const double& value);
            void PV_DataAcquisition_Offset_Writer(const timespec& timestamp, const double& value);
            void PV_DataAcquisition_Bandwidth_Writer(const timespec& timestamp, const double& value);
            void PV_DataAcquisition_Resolution_Writer(const timespec& timestamp, const double& value);
            void PV_DataAcquisition_Impedance_Writer(const timespec& timestamp, const double& value);
            void PV_DataAcquisition_Coupling_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_DataAcquisition_SignalRefType_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_DataAcquisition_Ground_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_DataAcquisition_DMAEnable_Writer(const timespec& timestamp, const std::int32_t& value);
            void PV_DataAcquisition_SamplingRate_Writer(const timespec& timestamp, const double& value);

                /**
                 * DataAcquisition initializers
                 */
            void PV_DataAcquisition_Gain_Initializer(timespec* timestamp, double* value);
            void PV_DataAcquisition_Offset_Initializer(timespec* timestamp, double* value);
            void PV_DataAcquisition_Bandwidth_Initializer(timespec* timestamp, double* value);
            void PV_DataAcquisition_Resolution_Initializer(timespec* timestamp, double* value);
            void PV_DataAcquisition_Impedance_Initializer(timespec* timestamp, double* value);
            void PV_DataAcquisition_Coupling_Initializer(timespec* timestamp, int32_t* value);
            void PV_DataAcquisition_SignalRefType_Initializer(timespec* timestamp, int32_t* value);
            void PV_DataAcquisition_Ground_Initializer(timespec* timestamp, int32_t* value);
            void PV_DataAcquisition_DMAEnable_Initializer(timespec* timestamp, int32_t* value);
            void PV_DataAcquisition_SamplingRate_Initializer(timespec* timestamp, double* value);


                //Function that continuously acquires data. It is launched by
                //start_DataAcquisition() in a separate thread.
            void DataAcquisition_thread_body();

                //A thread that runs DataAcquisition_thread_body().
            nds::Thread m_DataAcquisition_Thread;

            //A boolean flag that stop the DataAcquisition loop in
            //DataAcquisition_thread_body() when true.
            volatile bool m_bStop_DataAcquisition;

            /*FPGA functions*/
            
            /*Functions for bool indicators*/
            void Status_Reader(const timespec* timestamp, std::int32_t *pIn);
            void Clock100PLLUnlocked_Reader(const timespec* timestamp, std::int32_t *pIn);
            void EEPROMPowerEnabled_Reader(const timespec* timestamp, std::int32_t *pIn);
            void IOModuleIOEnabled_Reader(const timespec* timestamp, std::int32_t *pIn);
            void IOModuleInitializationDone_Reader(const timespec* timestamp, std::int32_t *pIn);
            void IOModulePowerEnabled_Reader(const timespec* timestamp, std::int32_t *pIn);
            void IOModulePowerGood_Reader(const timespec* timestamp, std::int32_t *pIn);
            void IOModulePresent_Reader(const timespec* timestamp, std::int32_t *pIn);

            /*Function for I8 indicators*/
            void DeviceTemperature_Reader(const timespec* timestamp, std::int32_t *pIn);

            /*Function for U8 indicators*/
            //void streamstate_Reader(const timespec* timestamp, std::int32_t *pIn);
            void measurement_streamstate_Reader(const timespec* timestamp, std::int32_t *pIn); 

            /*Indicators for U16*/
            void Device12VPower_Reader(const timespec* timestamp, std::int32_t *pIn);
            void Device33VPower_Reader(const timespec* timestamp, std::int32_t *pIn);

            /*Indicators for U32*/ 
            void ExpectedIOModuleID_Reader(const timespec* timestamp, std::int32_t *pIn);
            void InsertedIOModuleID_Reader(const timespec* timestamp, std::int32_t *pIn);

            /*Indicators for U64*/
            void SinglePulsesCountRate_Reader(const timespec* timestamp, std::int32_t *pIn);
            void SinglePulsesRegistered_Reader(const timespec* timestamp, std::int32_t *pIn);
            void TotalCountRate_Reader(const timespec* timestamp, std::int32_t *pIn);
            void TotalPulsesRegistered_Reader(const timespec* timestamp, std::int32_t *pIn);

            //void streammetadata_Reader(const timespec* timestamp, std::int32_t *pIn);
            //void streamsamplestransferred_Reader(const timespec* timestamp, std::int32_t *pIn);

            void measurement_streammetadata_Reader(const timespec* timestamp, std::int32_t *pIn);
            void measurement_streamsamplestransferred_Reader(const timespec* timestamp, std::int32_t *pIn);


            /*Controls for bool*/
            void ThresholdValid_Writer(const timespec& timestamp, const std::int32_t& pOut);
            void start_Writer(const timespec& timestamp, const std::int32_t& pOut);

            /*Controls for U8*/
            //void streamrequeststate_Writer(const timespec& timestamp, const std::int32_t& pOut);
            void measurement_streamrequeststate_Writer(const timespec& timestamp, const std::int32_t& pOut);
            
            /*Controls for I16*/
            void Threshold_Writer(const timespec& timestamp, const std::int32_t& pOut);

            /*Controls for U16*/
            void ChannelSelect_Writer(const timespec& timestamp, const std::int32_t& pOut);

            /*Controls for U32*/
            void maxticksperpulse_Writer(const timespec& timestamp, const std::int32_t& pOut);
            void minticksperpulse_Writer(const timespec& timestamp, const std::int32_t& pOut);

            /*Controls for U64*/
            //void streamnumsamples_Writer(const timespec& timestamp, const std::int32_t& pOut);
            void  measurement_streamnumsamples_Writer(const timespec& timestamp, const std::int32_t& pOut);

            /*Reader for FIFO*/
            void DataBufferSize_Reader(const timespec* timestamp, std::int32_t *pIn);
            void Number_Of_Elements_Reader(const timespec* timestamp, std::int32_t *pIn);
            void FIFO_depth_Reader(const timespec* timestamp, std::int32_t *pIn);

            /*Configure FPGA resources*/
            void Downloader_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void Reseter_Writer(const timespec& timestamp, const std::int32_t & pOut);
            void ConfigureFIFO_Writer(const timespec& timestamp, const std::int32_t & pOut);


            /*Additional calculation*/
            void RejectionRate_Reader(const timespec* timestamp, double *pIn);
           // NiFpga_Bool* getNiFpga_Bool (std::int32_t* pIn); not longer needed

           //FPGA FIFO methods 
           void PulsePeaksFIFO_Reader (const timespec* timestamp, std::vector<double> *pIn);          
           void edges_Reader (const timespec* timestamp, std::vector<double> *pIn);

           void step_Writer(const timespec& timestamp, const std::int32_t & pOut);
           void maximum_amplitude_Reader (const timespec* timestamp, double *pIn);
           //values copied from previous working version
            size_t sample_size = 1000000;
            size_t fifo_depth = 10*1023;
            size_t data_buffer_size = 10*1023;
            size_t number_of_elements = 10*1023; //it should be at least 2 times bigger

            std::vector<double> amplitudes {0};
		std::vector<double> numbers;
            size_t step = 1000; //previosly was 1000

            int16_t mode = 0;

            std::vector<double> PulsePeaksFIFO;

        };


#endif
