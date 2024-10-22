/**
 * @file DANCP.hpp
 * 
 * @author Javier Moreno (GMV)
 * @date 7 Sep 2018
 */

#ifndef NDS_DAN_INCLUDE_DANCP_H_
#define NDS_DAN_INCLUDE_DANCP_H_

#include "../nds-dan/definitions.h"
#include "../nds-dan/DAN.h"

namespace ndsdan
{

class DANCPImpl;



/**
 * @brief Class to deal with %DAN by copying buffer created by the user.
 * This class provides access to the corresponding %DAN API when a buffer to copy
 * data is required by means of its underlying PVs. @n
 * @ingroup CopyBuffer
 */
class NDS_DAN_API DANCP : public ndsdan::DAN
{

public:


  /**
   * @brief Default constructor to initialise the DANCP node by means of its parent constructor.
   */
  DANCP();

  /**
   * @brief Constructor to initialise the DANCP node from another to be copied by the parent constructor.
   * @param right Reference node to be copied in instantiation.
   */
  DANCP(const DANCP& right);

  /**
   * @brief Constructor to initialise the DANCP with the required parameters to use %DAN features.
   * @param name Name to identify the node in the driver node's structure.
   * It must be an unique name within its parent node in the driver.
   * @param dataStructureInitializer Delegate function to allocate resources for the internal data buffer.
   * This function shall return the pointer to the internal data buffer on success, or NULL cannot be allocated.
   * See in @ref genericDANFunction_t the prototype of the function to be provided.
   * @param dataStructureReleaser Delegate function to free the resources of the internal data buffer.
   * See in @ref genericFunction_t the prototype of the function to be provided.
   * This function shall return NULL on success, or the pointer to the internal buffer if it cannot be deallocated.
   * @param blockHeaderSetter Delegate function to provide the header to be set in every block data pushed.
   * If no header is required, this function shall return NULL.
   * See in @ref charDANFunction_t the prototype of the function to be provided.
   */
  DANCP(const std::string& name,
        genericDANFunction_t dataStructureInitializer,
        genericFunction_t dataStructureReleaser,
        charDANFunction_t blockHeaderSetter);

  /**
   * @brief Destructor to release allocated resources.
   */
  ~DANCP();

  /**
   * @brief Operator to create a copy of the given object.
   * @param right Reference object to be copied.
   * @return Reference to the new object.
   */
  DANCP& operator=(const DANCP& right);

  /**
   * @brief Get the interval time between two consecutive push operations to the %DAN source.
   * @return Time between consecutive push operations, in nanoseconds.
   * This parameter may have not been set. See @ref dataDANCP_t::timeBetweenBlocks for further details.
   */
  std::uint64_t getTimeBetweenBlocks();

  /**
   * @brief Get the maximum time length to the publish data in the %DAN source.
   * @return Maximum time to acquire data, in nanoseconds.
   * This parameter may have not been set. See @ref dataDANCP_t::maxAcquisitionTimeNs for further details.
   */
  double getMaxAcquisitionTime();

  /**
   * Get the size number of data blocks of the internal DAQ buffer allocated in the %DAN API.
   * @return Size of the internal DAQ buffer in %DAN API, in number of blocks.
   * See @ref dataDANCP_t::numberOfBlocks for further details.
   */
  std::int32_t getNumberOfBlocks();

  /**
   * @brief Set the interval time between two consecutive push operations to the %DAN source.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param timeBetweenBlocks Interval time between push operations, in nanoseconds.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * This is an optional parameter. See @ref dataDANCP_t:timeBetweenBlocks for further details.
   */
  void setTimeBetweenBlocks(const timespec& timestamp, const double& timeBetweenBlocks);

  /**
   * Set the maximum time length to the publish data in the %DAN source.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param maxAcquisitionTime Maximum time length to acquire/publish data, in nanoseconds.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * This is an optional parameter. See @ref dataDANCP_t::maxAcquisitionTimeNs for further details.
   */
  void setMaxAcquisitionTime(const timespec& timestamp, const double& maxAcquisitionTime);

  /**
   * @brief Set the number of blocks in the internal DAQ buffer of the %DAN API.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param numberOfBlocks Number of data blocks of the internal DAQ buffer allocated in the %DAN API.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANCP_t::numberOfBlocks for further details.
   */
  void setNumberOfBlocks(const timespec& timestamp, const std::int32_t& numberOfBlocks);

};

}



#endif /* NDS_DAN_INCLUDE_DANCP_H_ */
