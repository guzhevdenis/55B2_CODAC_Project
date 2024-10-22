/**
 * @file DANCP.h
 * 
 * @author Javier Moreno (GMV)
 * @date 6 Sep 2018
 */

#ifndef NDS_DAN_INCLUDE_DANCPIMPL_H_
#define NDS_DAN_INCLUDE_DANCPIMPL_H_

#include "../../nds-dan/definitions.h"
#include "../../nds-dan/impl/DANImpl.h"

namespace ndsdan
{

class DANCPImpl : public ndsdan::DANImpl
{
public:

  DANCPImpl(const std::string& name,
        genericDANFunction_t dataStructureInitializer,
        genericFunction_t dataStructureReleaser,
        charDANFunction_t blockHeaderSetter);

  ~DANCPImpl();

  std::uint64_t getTimeBetweenBlocks();
  void readTimeBetweenBlocks(timespec* timestamp, double* timeBetweenBlocks);
  double getMaxAcquisitionTime();
  void readMaxAcquisitionTime(timespec* timestamp, double* maxAcquisitionTime);
  std::int32_t getNumberOfBlocks();
  void readNumberOfBlocks(timespec* timestamp, std::int32_t* numberOfBlocks);

  void setTimeBetweenBlocks(const timespec& timestamp, const double& timeBetweenBlocks);
  void setMaxAcquisitionTime(const timespec& timestamp, const double& maxAcquisitionTime);
  void setNumberOfBlocks(const timespec& timestamp, const std::int32_t& numberOfBlocks);



protected:

  dataDANCP_t m_DataDANCP;

  std::shared_ptr<nds::PVDelegateOutImpl <double> >m_TimeBetweenBlocks_PV;
  std::shared_ptr<nds::PVDelegateInImpl <double> >m_TimeBetweenBlocks_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <double> >m_MaxAcquisitionTime_PV;
  std::shared_ptr<nds::PVDelegateInImpl <double> >m_MaxAcquisitionTime_RBVPV;

  std::shared_ptr<nds::PVDelegateOutImpl <std::int32_t> >m_NumberOfBlocks_PV;
  std::shared_ptr<nds::PVDelegateInImpl <std::int32_t> >m_NumberOfBlocks_RBVPV;

  statusDAN_t publishSource(void);

  void putDataBlock(const std::uint64_t& timestamp);

private:

  std::atomic<bool> m_canAcquire;
  std::atomic<bool> m_canPublish;


};

}



#endif /* NDS_DAN_INCLUDE_DANCPIMPL_H_ */
