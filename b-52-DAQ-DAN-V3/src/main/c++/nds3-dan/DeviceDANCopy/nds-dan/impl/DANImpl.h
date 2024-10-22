/*
 * DANimpl.h
 *
 *  Created on: 3 Sep 2018
 *      Author: jmoreno
 */

#ifndef NDS_DAN_INCLUDE_DANIMPL_H_
#define NDS_DAN_INCLUDE_DANIMPL_H_

#include <atomic>

#include "../definitions.h"
#include "nds3/impl/stateMachineImpl.h"
#include "nds3/impl/pvDelegateOutImpl.h"
#include "nds3/impl/pvDelegateInImpl.h"
#include "nds3/impl/pvVariableInImpl.h"



namespace ndsdan
{

class DANImpl : public nds:: NodeImpl
{

public:

  DANImpl(const std::string& name,
          genericDANFunction_t dataStructureInitializer,
          genericFunction_t dataStructureReleaser,
          charDANFunction_t blockHeaderSetter);
  ~DANImpl();

  std::string getSourceName();
  void readSourceName(timespec* timestamp, std::string* sourceName);
  std::string getICProgramName();
  void readICProgramName(timespec* timestamp, std::string* ICProgramName);
  std::int32_t getSamplingRate();
  void readSamplingRate(timespec* timestamp, std::int32_t* samplingRate);
  std::int32_t getBufferSize();
  void readBufferSize(timespec* timestamp, std::int32_t* bufferSize);
  std::int32_t getNumberOfChannels();
  void readNumberOfChannels(timespec* timestamp, std::int32_t* numberOfChannels);
  std::int32_t getSampleSize();
  void readSampleSize(timespec* timestamp, std::int32_t* sampleSize);
  std::int32_t getBlockNumberOfSamples();
  void readBlockNumberOfSamples(timespec* timestamp, std::int32_t* pathXML);
  std::string getPathXML();
  void readPathXML(timespec* timestamp, std::string* pathXML);

  void setSourceName(const timespec& timestamp, const std::string& sourceName);
  void setICProgramName(const timespec& timestamp, const std::string& ICProgramName);
  void setSamplingRate(const timespec& timestamp, const std::int32_t& samplingRate);
  void setBufferSize(const timespec& timestamp, const std::int32_t& bufferSize);
  void setSampleSize(const timespec& timestamp, const std::int32_t& sampleSize);
  void setBlockNumberOfSamples(const timespec& timestamp, const std::int32_t& blockNumberOfSamples);
  void setPathXML(const timespec& timestamp, const std::string& pathXML);

  virtual void putDataBlock(const std::uint64_t& timestamp) = 0;

  void cleanUpDANResources(void);

protected:

  std::recursive_mutex m_Mutex;

  std::atomic<bool> m_ICProgramLoaded;
  std::atomic<bool> m_DANStreamerOpen;
  std::atomic<bool> m_CanPublishDataBlocks;
  std::atomic<bool> m_InitialisedTCN;

  dataDAN_t m_DataDAN;

  std::shared_ptr<nds::ThreadBaseImpl> m_PublisherThread;

  // ---------------------------------------- PVs ----------------------------------------------

  std::shared_ptr<nds::PVVariableInImpl <std::int32_t> >m_Status_PV;
  std::shared_ptr<nds::PVVariableInImpl <std::int32_t> >m_StatusStream_PV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::string> >m_SourceName_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::string> >m_SourceName_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::string> >m_ICProgramName_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::string> >m_ICProgramName_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_SamplingRate_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_SamplingRate_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_BufferSize_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_BufferSize_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_SampleSize_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_SampleSize_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_BlockNumberOfSamples_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_BlockNumberOfSamples_RBVPV;

  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_NumberOfChannels_PV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::string> >m_PathXML_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::string> >m_PathXML_RBVPV;

  // -------------------------------------- End PVs --------------------------------------------



  // --------------------------------------- Nodes ---------------------------------------------

  std::shared_ptr<nds::StateMachineImpl> m_StateMachine;

  // ------------------------------------- End Nodes -------------------------------------------



  void STMswitchOn(void);
  void STMswitchOff(void);
  void STMstart(void);
  void STMstop(void);
  void STMrecover(void);
  bool STMallowStateChange(const nds::state_t currentState, const nds::state_t currentGlobalState, const nds::state_t desiredState);

  genericDANFunction_t m_InitializeDataStructure;
  genericFunction_t m_ReleaseDataStructure;
  charDANFunction_t m_SetBlockHeader;

  virtual statusDAN_t publishSource(void) = 0;

  void updateStatus(const statusDAN_t& status);
  void updateStatus(const timespec& timestamp, const statusDAN_t& status);
  std::string parseStatus(const statusDAN_t& status);
  void updateStatusStream(const statusStreamDAN_t& status);
  void updateStatusStream(const timespec& timestamp, const statusStreamDAN_t& status);
  std::string parseStatusStream(const statusStreamDAN_t& status);

};

}

#endif /* NDS_DAN_INCLUDE_DANIMPL_H_ */
