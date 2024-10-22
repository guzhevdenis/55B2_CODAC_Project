/**
 * @file DeviceNDSDANCopy.cpp
 *
 * @author Denis Guzhev
 * @date 12 Oct 2022
 * Labview version program 1.5
 */

#include <tcn.h>
#include <math.h>
#include <iostream>
#include "include/DeviceNDSDANCopy.h"

#define NDS_EPOCH 1514764800 /* 00:00 of 1/1/2018 in UTC format. */
#define S2NS 1000000000 //Seconds to nanoSeconds
int counter = 0;

//the number of elements from Data Acqusition Node in main driver
#define SIZE 800000

DeviceNDSDANCopy::DeviceNDSDANCopy (nds::Factory& factory,
                                const std::string& deviceName,
                                const nds::namedParameters_t& namedParameters) :
      m_deviceName(deviceName),
      m_samplesCounter(0),
      m_danBuffer(NULL),
      rootNode(deviceName),
      m_data_PV("buffer",
                  std::bind(&DeviceNDSDANCopy::PV_buffer_writer, this, std::placeholders::_1, std::placeholders::_2)),
      m_MaxChannels_PV(nds::PVDelegateOut<std::int32_t>("MaxChannels",
                  std::bind(&DeviceNDSDANCopy::setMaxChannels, this, std::placeholders::_1, std::placeholders::_2)))
{

  m_DANCP = rootNode.addChild(ndsdan::DANCP("DANCP",
                  std::bind(&DeviceNDSDANCopy::dancp_initData, this, std::placeholders::_1),
                  std::bind(&DeviceNDSDANCopy::dancp_releaseData, this, std::placeholders::_1),
                  std::bind(&DeviceNDSDANCopy::dancp_setHeader, this, std::placeholders::_1)));

  m_MaxChannels_RBVPV = nds::PVVariableIn<std::int32_t>("MaxChannels_RBV");
  m_MaxChannels_RBVPV.setValue(static_cast<int32_t>(0));

  m_stateMachine = rootNode.addChild(nds::StateMachine( false,
                  std::bind(&DeviceNDSDANCopy::stm_SwitchOn, this),
                  std::bind(&DeviceNDSDANCopy::stm_SwitchOff, this),
                  std::bind(&DeviceNDSDANCopy::stm_Start, this),
                  std::bind(&DeviceNDSDANCopy::stm_Stop, this),
                  std::bind(&DeviceNDSDANCopy::stm_recover, this),
                  std::bind(&DeviceNDSDANCopy::stm_allowChange, this,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

  rootNode.addChild(m_data_PV);
  m_data_PV.setMaxElements(SIZE);
  
  rootNode.addChild(m_MaxChannels_PV);
  rootNode.addChild(m_MaxChannels_RBVPV);

  rootNode.initialize(this, factory);

  rootNode.setLogLevel(nds::logLevel_t::debug);

  //initializePVs();

  nds::namedParameters_t::const_iterator findParam =  namedParameters.find("INIT");
  if (findParam != namedParameters.end() && findParam->second=="YES") {
    initializePVs();
  }
}



DeviceNDSDANCopy::~DeviceNDSDANCopy ()
{
}


#ifdef EPICS

  NDS_DEFINE_DRIVER(DeviceNDSDANCopy, DeviceNDSDANCopy)
#else

void*
DeviceNDSDANCopy::allocateDevice (nds::Factory& factory,
                                  const std::string& deviceName,
                                  const nds::namedParameters_t& namedParameters){
    return new DeviceNDSDANCopy(factory, deviceName, namedParameters);
}


void
  DeviceNDSDANCopy::deallocateDevice (void* deviceName){
    delete (DeviceNDSDANCopy*)deviceName;
}
#endif

void
DeviceNDSDANCopy::setMaxChannels(const timespec& timestamp,
                                const std::int32_t& maxNumberOfChannels){
  nds::state_t currentState = m_stateMachine.getLocalState();
  if (currentState == nds::state_t::off) {
      m_MaxChannels_RBVPV.setValue(rootNode.getTimestamp(), maxNumberOfChannels);
      m_MaxChannels_RBVPV.push(rootNode.getTimestamp(), maxNumberOfChannels);
  } else {
      ndsErrorStream(m_MaxChannels_RBVPV) << "The maximum number of channels can only be modified in OFF state." << std::endl;
  }
}

void
DeviceNDSDANCopy::stm_SwitchOn (void){
  ndsDebugStream(rootNode) << (m_deviceName + "-STM: Switching On" ) << std::endl;
}

void
DeviceNDSDANCopy::stm_SwitchOff (void){
  ndsDebugStream(m_stateMachine) << (m_deviceName + "-STM: Switching Off" ) << std::endl;
}

void
DeviceNDSDANCopy::stm_Start (void){
  ndsDebugStream(m_stateMachine) << (m_deviceName + "-STM: Starting" ) << std::endl;
}

void
DeviceNDSDANCopy::stm_Stop (void){
  ndsDebugStream(m_stateMachine) << (m_deviceName + "-STM: Stopping" ) << std::endl;
  ndsDebugStream(m_stateMachine) << "DAN-DAQ successfully stopped." << std::endl;
}

void
DeviceNDSDANCopy::stm_recover (void){
  ndsDebugStream(m_stateMachine) << (m_deviceName + "-STM: Recovering" ) << std::endl;
}

bool
DeviceNDSDANCopy::stm_allowChange(const nds::state_t currentState, const nds::state_t currentGlobalState, const nds::state_t desiredState){
  if (currentState == nds::state_t::on && desiredState == nds::state_t::running && m_danBuffer == NULL) {
      ndsErrorStream(m_stateMachine) << (m_deviceName + "-STM: State change not allowed until switching on the DANCP' state machine" ) << std::endl;
      return false;
  } else {
      ndsDebugStream(m_stateMachine) << (m_deviceName + "-STM: Allowing change" ) << std::endl;
      return true;
  }
}

//THIS FUNCTION WAS INITIALY IN THE EXAMPLE. 
void*
DeviceNDSDANCopy::dancp_initData (ndsdan::dataDAN_t* dataDAN){
    std::int32_t bufferSize = m_DANCP.getBufferSize(); //initial string
    ndsErrorStream(m_DANCP) << "bufferSize = m_DANCP.getBufferSize() " << bufferSize << std::endl;
    if (bufferSize > 0) {
    //m_danBuffer = (std::int16_t *) malloc(bufferSize);
    //memset(m_danBuffer, 0, bufferSize);
    //ndsErrorStream(m_DANCP) << "Size of _dan Buffer through sizeof " << sizeof(m_danBuffer) << std::endl;
    std::cout<< "Size of _dan Buffer through 8*size " << bufferSize << std::endl;
    return m_danBuffer;
  } else {
      return NULL;
  }
}

void DeviceNDSDANCopy::PV_buffer_writer(const timespec& timestamp, const std::vector<std::int16_t>& value){

  std::int32_t blockSize = m_DANCP.getBlockNumberOfSamples();
  std::int32_t nChannels = m_DANCP.getNumberOfChannels();
  std::int32_t sampleSize = m_DANCP.getSampleSize();
  std::int32_t blockSizeInBytes = blockSize*nChannels*sampleSize;
  std::int32_t vectorSize = value.capacity()*2; //Received vector size in bytes. 2 for int16
 
  timespec actualTime;
  std::uint64_t currentTimeNs;
  //std::int16_t* help = value.data();

  if(vectorSize == blockSizeInBytes){
        
          
        memcpy(m_danBuffer, value.data(), blockSizeInBytes);   //coping from value.data() to m_danBuffer

        actualTime = m_DANCP.getTimestamp(); 
        currentTimeNs = actualTime.tv_sec*S2NS + actualTime.tv_nsec; //Time translates in nanoseconds

        usleep(0.0000001);
        m_DANCP.putDataBlock(currentTimeNs);
           
    
  }else{
          ndsErrorStream(m_DANCP) << "DAN buffer size is different to received vector." << std::endl;
          ndsErrorStream(m_DANCP) << "DAN buffer size is "<< blockSizeInBytes << " and received vector size is " << vectorSize << std::endl;
  }

}

void*
DeviceNDSDANCopy::dancp_releaseData (void* dataStructure){
  free(dataStructure);
  return NULL;
}

char*
DeviceNDSDANCopy::dancp_setHeader (const ndsdan::dataDAN_t& dataDAN){
  return NULL;
}

void DeviceNDSDANCopy::initializePVs(void){

  timespec timestamp = {0, 0};

  std::string initSourceName("FlexRIO");
  std::string initICProgramName("nds-dan");

  std::int32_t initSamplingRate(800000000); //8MGHz (ADC parameter)

 //Output 

  std::int32_t initSampleSize(2); //number of Bytes in int16

  std::int32_t initBlockNumberOfSamples(SIZE); // 1.5 Mb the size of the vector

  //std::uint64_t initTimeBetweenBlocks(10);
  //std::uint64_t initMaxAcquisitionTime(1000000000000);

  std::int32_t initNumberOfBlocks(100); // it can be tested manually 

  std::int32_t initMaxChannels(1);

  std::int32_t initBufferSize(initBlockNumberOfSamples*initSampleSize*initNumberOfBlocks);

  std::string initPathXML("/home/codac-dev/55.B2_Prototype/b-52-DAQ-DAN-V3/src/main/c++/nds3-dan/DeviceDANCopy/danConfig.xml");

  m_DANCP.setSourceName(timestamp, initSourceName);
  m_DANCP.setICProgramName(timestamp, initICProgramName);
  m_DANCP.setSamplingRate(timestamp, initSamplingRate);

  m_DANCP.setBufferSize(timestamp, initBufferSize);
  m_DANCP.setSampleSize(timestamp, initSampleSize);
  m_DANCP.setBlockNumberOfSamples(timestamp, initBlockNumberOfSamples);

 //m_DANCP.setTimeBetweenBlocks(timestamp, static_cast<double>(initTimeBetweenBlocks));
  //m_DANCP.setMaxAcquisitionTime(timestamp, static_cast<double>(initMaxAcquisitionTime));

  m_DANCP.setNumberOfBlocks(timestamp, initNumberOfBlocks);

  m_DANCP.setPathXML(timestamp, initPathXML);
  
  setMaxChannels(timestamp, initMaxChannels);

  //Inititalization of the DAN buffer
  m_danBuffer = (std::int16_t*)malloc(initBufferSize);
  memset(m_danBuffer, 0, initBufferSize);

}
