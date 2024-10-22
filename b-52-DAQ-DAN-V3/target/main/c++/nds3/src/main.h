#ifndef VNC_H_
#define VNC_H_

#include <nds3/nds.h>
#include "NiFpga.h"
#include "NiFpga_singlechannelmain.h" 
#include "nifpga.hpp"
#include "state_machine.hpp"


const char* bitfile = "NiFpga_singlechannelmain.lvbitx";
const char* signature = "33E328438B060871C6869B2BCB185D06"; 
const char* device = "RIO0";

uint32_t control   = NiFpga_singlechannelmain_ControlU8_streamrequeststate0;
uint32_t indicator = NiFpga_singlechannelmain_IndicatorU8_streamstate0;

const uint32_t dma_fifo =
        NiFpga_singlechannelmain_TargetToHostFifoI16_FIFO;

        class Device

        {
            public: 
            //constructor
            Device(nds::Factory& factory,
                   const std::string& deviceName,
                   const nds::namedParameters_t&);
             //destructor
            ~Device();
    
            //New FPGA interface
            NiFpga_Session session; 
            NiFpga_Status status;

            //undrelaying FPGA session
            //methods for reading indicators
            //std::int32_t is supported by AsynDriver
            void State_Reader(const timespec* timestamp, std::int32_t *pIn);
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
            //special indicators

            //Allocation method
            static void* allocateDevice(nds::Factory& factory, const
            std::string& deviceName, const nds::namedParameters_t&
            parameters);
            //getDevice function 
            Device* getInstance (const std::string& deviceName);
            //Deallocation method
            static void deallocateDevice(void* deviceName);

            size_t sample_size = 10000000;
            size_t fifo_depth = 10485760;
            size_t data_buffer_size = 0;
            size_t number_of_elements = 0;
            std::string out_directory = std::string("/home/codac-dev/NDS_projects/nds-pcf/daq_stream/nds3/recordings/");
            std::string out_file_name = std::string("recording.bin");
            std::string output = std::string("test");
            
            //ni::fpga::status status = NiFpga_Status_Success;

            std::int32_t status_of_file_recording = -1;
            private:
            //Device name
            std::string m_name;


            /*DATA GENERATION*/
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

        };


#endif
