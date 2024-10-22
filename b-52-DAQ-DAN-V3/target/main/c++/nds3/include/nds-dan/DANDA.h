/**
 * @file DANDA.h
 * 
 * @author Javier Moreno (GMV)
 * @date 6 Sep 2018
 */

#ifndef NDS_DAN_INCLUDE_DANDA_H_
#define NDS_DAN_INCLUDE_DANDA_H_


#include "../nds-dan/definitions.h"
#include "../nds-dan/DAN.h"

namespace ndsdan
{

class DANDAmpl;


/**
 * @brief Class to deal with %DAN by means of an external shared buffer provided by the low-level driver API.
 * This class provides access to the corresponding %DAN API when a shared buffer of data is
 * required by means of its underlying PVs. @n
 * @ingroup SharedBuffer
 */
class NDS_DAN_API DANDA : public ndsdan::DAN
{
public:

  /**
   * @brief Default constructor to initialise the DANDA node by means of its parent constructor.
   */
  DANDA();

  /**
   * @brief Constructor to initialise the DANDA node from another to be copied by the parent constructor.
   * @param right Reference node to be copied in instantiation.
   */
  DANDA(const DANDA& right);

  /**
   * @brief Constructor to initialise the DANDA with the required parameters to use %DAN features.
   * @param name Name to identify the node in the driver node's structure.
   * It must be an unique name within its parent node in the driver.
   * @param dataStructureInitializer Delegate function to retrieve a pointer to the external data buffer.
   * This function shall return the pointer to the external data buffer on success, or NULL cannot be provided.
   * See in @ref genericDANFunction_t the prototype of the function to be provided.
   * @param dataStructureReleaser Delegate function to unlink from the external data buffer.
   * See in @ref genericFunction_t the prototype of the function to be provided.
   * This function shall return NULL on success, or the pointer to the external buffer if it cannot be released.
   * @param blockHeaderSetter Delegate function to provide the header to be set in every block data pushed.
   * If no header is required, this function shall return NULL.
   * See in @ref charDANFunction_t the prototype of the function to be provided.
   * @param buffserSizeInSamplesGetter Delegate function to get the size of the external data buffer,
   * in number of samples. This function shall return a positive number greater than zero on success.
   * This function updates the value of @ref dataDANDA_t::bufferNumberOfSamples when the publisher
   * is connected to the source.
   * See in @ref longDANFunction_t the prototype of the function to be provided.
   * @param numberOfSamplesGetter Delegate function to get the number of available samples (per channel)
   * in the external data buffer.
   * See in @ref intDANDAFunction_t the prototype of the function to be provided.
   */
  DANDA(const std::string& name,
        genericDANFunction_t dataStructureInitializer,
        genericFunction_t dataStructureReleaser,
        charDANFunction_t blockHeaderSetter,
        longDANFunction_t buffserSizeInSamplesGetter,
        intDANDAFunction_t numberOfSamplesGetter);

  /**
   * @brief Destructor to release allocated resources.
   */
  ~DANDA();

  /**
   * @brief Operator to create a copy of the given object.
   * @param right Reference object to be copied.
   * @return Reference to the new object.
   */
  DANDA& operator=(const DANDA& right);

  /**
   * @brief Get the number of the device descriptor.
   * @return Number of the device descriptor.
   * See @ref dataDANDA_t::deviceNumber for further details.
   */
  std::int32_t getDeviceNumber();

  /**
   * @brief Get the path and name of the device descriptor.
   * @return String with the absolute path of the device descriptor.
   * See @ref dataDANDA_t::deviceDescriptor for further details.
   */
  std::string getDeviceDescriptor();

  /**
   * @brief Get the mechanism used to access the external DAQ buffer.
   * @return Mechanism to access the external buffer.
   * See @ref dataDANDA_t::shMemType for further details.
   */
  std::int32_t getSharedMemoryType();

  /**
   * @brief Get the size of the external DAQ buffer.
   * @return Size of the external DAQ buffer, in number of samples.
   * See @ref dataDANDA_t::bufferNumberOfSamples for further details.
   */
  std::int32_t getBufferSizeInSamples();

  /**
   * @brief Get the number of samples per channel available to be published.
   * @return Number of samples per channel available to be published.
   * It gets the number of samples per channel available by means of the function provided
   * by the driver developer (@ref numberOfSamplesGetter). This function shall be used before
   * calling @ref putDataBlock function in order to ensure that data is available in the buffer.
   */
  std::int32_t getNumberOfAvailableSamples();

  /**
   * @brief Get the initial offset in the external DAQ buffer.
   * @return Initial offset in the external DAQ buffer, in Bytes.
   * See @ref dataDANDA_t::bufferInitialOffset for further details.
   */
  std::int32_t getBufferInitialOffset();

  /**
   * @brief Get the current offset in the external DAQ buffer.
   * @return Current offset in the external DAQ buffer, in number of samples.
   * See @ref dataDANDA_t::bufferCurrentOffset for further details.
   */
  std::int32_t getBufferCurrentOffset();

  /**
   * @brief Set the number of the device descriptor.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param deviceNumber Number of the device descriptor to be accesed.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANDA_t::deviceNumber for further details.
   */
  void setDeviceNumber(const timespec& timestamp, const std::int32_t& deviceNumber);

  /**
   * @brief Set the path and name of the device descriptor.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param deviceDescriptor Absolute path of the device descriptor.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANDA_t::deviceDescriptor for further details.
   */
  void setDeviceDescriptor(const timespec& timestamp, const std::string& deviceDescriptor);

  /**
   * @brief Set the mechanism used to access the external DAQ buffer.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param sharedMemoryType Mechanism to access the external buffer.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANDA_t::shMemType for further details.
   */
  void setSharedMemoryType(const timespec& timestamp, const std::int32_t& sharedMemoryType);

  /**
   * @brief Set the initial offset in the external DAQ buffer.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param bufferInitialOffset Initial offset in the external DAQ buffer, in Bytes.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANDA_t::bufferInitialOffset for further details.
   */
  void setBufferInitialOffset(const timespec& timestamp, const std::int32_t& bufferInitialOffset);

  /**
   * @brief Set the current offset in the external DAQ buffer.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param currentOffset Current offset in the external DAQ buffer, in number of samples.
   *
   * It can only be called when the state machine is in @ref nds::state_t::OFF state. @n
   * See @ref dataDANDA_t::bufferCurrentOffset for further details.
   */
  void setBufferCurrentOffset(const timespec& timestamp, const std::uint64_t& currentOffset);


};

}


#endif /* NDS_DAN_INCLUDE_DANDA_H_ */
