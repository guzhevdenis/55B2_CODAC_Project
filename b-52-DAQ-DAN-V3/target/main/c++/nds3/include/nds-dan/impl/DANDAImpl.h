/**
 * @file DAN_DAImpl.h
 * 
 * @author Javier Moreno (GMV)
 * @date 5 Sep 2018
 */

#ifndef NDS_DAN_INCLUDE_DAN_DAIMPL_H_
#define NDS_DAN_INCLUDE_DAN_DAIMPL_H_

#include "../../nds-dan/definitions.h"
#include "../../nds-dan/impl/DANImpl.h"


namespace ndsdan
{

class DANDAImpl : public ndsdan::DANImpl
{

public:
  DANDAImpl(const std::string& name,
        genericDANFunction_t dataStructureInitializer,
        genericFunction_t dataStructureReleaser,
        charDANFunction_t blockHeaderSetter,
        longDANFunction_t buffserSizeInSamplesGetter,
        intDANDAFunction_t numberOfSamplesGetter);
  ~DANDAImpl();

  std::int32_t getDeviceNumber();
  void readDeviceNumber(timespec* timestamp, std::int32_t* deviceNumber);
  std::string getDeviceDescriptor();
  void readDeviceDescriptor(timespec* timestamp, std::string* deviceDescriptor);
  std::int32_t getSharedMemoryType();
  void readSharedMemoryType(timespec* timestamp, std::int32_t* sharedMemoryType);
  std::int32_t getBufferInitialOffset();
  void readBufferInitialOffset(timespec* timestamp, std::int32_t* bufferInitialOffset);
  std::int32_t getBufferCurrentOffset();
  void readBufferCurrentOffset(timespec* timestamp, std::int32_t* bufferCurrentOffset);
  std::int32_t getBufferSizeInSamples();
  void readBufferSizeInSamples(timespec* timestamp, std::int32_t* bufferSize);

  //Llama a m_GetNumberOfSamples (numberOfSamplesGetter del constructor)
  std::int32_t getNumberOfAvailableSamples();

  void setDeviceNumber(const timespec& timestamp, const std::int32_t& deviceNumber);
  void setDeviceDescriptor(const timespec& timestamp, const std::string& deviceDescriptor);
  void setSharedMemoryType(const timespec& timestamp, const std::int32_t& sharedMemoryType);
  void setBufferInitialOffset(const timespec& timestamp, const std::int32_t& bufferInitialOffset);
  void setBufferCurrentOffset(const timespec& timestamp, const std::uint64_t& currentOffset);

protected:

  dataDANDA_t m_DataDANDA;

  longDANFunction_t m_GetBufferSizeInSamples;
  intDANDAFunction_t m_GetNumberOfSamples;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_DeviceNumber_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_DeviceNumber_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::string> >m_DeviceDescriptor_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::string> >m_DeviceDescriptor_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_SharedMemoryType_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_SharedMemoryType_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_BufferInitialOffset_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_BufferInitialOffset_RBVPV;


  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_BufferCurrentOffset_PV;

  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_BufferSizeInSamples_PV;

  statusDAN_t publishSource(void);

  void putDataBlock(const std::uint64_t& timestamp);

};

}



#endif /* NDS_DAN_INCLUDE_DAN_DAIMPL_H_ */
