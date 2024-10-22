/**
 * @file DeviceNDSDANCopy.h
 * 
 * @author Javier Moreno (GMV)
 * @date 10 Sep 2018
 */

#ifndef NDS_DAN_EXAMPLES_INCLUDE_DeviceNDSDANCopy_H_
#define NDS_DAN_EXAMPLES_INCLUDE_DeviceNDSDANCopy_H_

#include <atomic>

#include <nds3/nds.h>

#include "./../nds-dan/nds-dan.h"


/**
 * @brief Sample class to publish data with %DAN by means of an internal buffer where data are copied.
 * This class provides access to the corresponding %DAN API when a data buffer is reserved in %DAN
 * API in order to copy data there before publishing datablocks by means of its underlying PVs. @n
 * The data will be published by the device driver with %DAN while the state machine of the
 * %DAN's node is running. @n
 * This NDS device driver is designed to get the data from another NDS device driver by means of the
 * NDS function <code>subscribe</code>. In this way, this sample driver may be used to publish
 * data with %DAN for any NDS device driver that generates data in a buffer of <code>int32_t</code>. @n
 *
 * <b><a>Requirements</a></b> @n
 * <ul>
 * <li><b>DAN writer service</b> must be started before running IOCs that use this module. To do that,
 * run the command: <code> sudo dan_archiver_writer start</code></li>.
 * <li><b>NDS data-source device driver</b>. This device driver must generate <code>int32_t</code> data.
 * </ul>
 *
 * <b>References</b> @n
 * This sample drivers is based on the use case <em>DAN API client program to copy new data in %DAN API
 * DAQBuffer</em> of the document
 * <a href=https://user.iter.org/default.aspx?uid=Q6GULS&version=v3.2>DAN User manual (Q6GULS v3.2)</a>. @n
 * Instead of generating data in a dedicated thread, the data are received from a different NDS device driver. @n
 * This device driver has been tested with <em>nds-irio</em> device driver. @n
 *
 * <b>Usage</b> @n
 * This example is focused on the %EPICS Control System for NDS.
 * <ol>
 * <li><em>ndsDataGenerator</em>: It is the name to reference the NDS device driver that generates
 * <code>int32_t</code> data and shall be provided by the user.</li>
 * <li><em>Start and connect both device drivers</em>. The <code>st.cmd</code> shall contain equivalent lines to:
 * @code
 * ## Load the NDS data generator driver
 * ndsLoadDriver("libnds3-ndsDataGenerator.so")
 * ndsCreateDevice(ndsDataGenerator, "DataGen-0"
 * ## Load the NDS-DAN driver that copies data in its internal buffer
 * ndsLoadDriver("libnds-dan-device-Copy.so")
 * ndsCreateDevice(DeviceNDSDANCopy, "DAN-0"
 * ...
 * ## Load record instances and database templates
 * ...
 * ##
 * ## Subscribe "buffer" PV of "DAN-0" to receive data from "Data" PV of "DataGen-0"
 * nds subscribe DAN-0-buffer DataGen-0-Data
 * @endcode </li>
 * <li><em>Set "DataGen-0" PVs to publish data</em>. Typically, a PV for decimation is used
 * to avoid publishing every data on the control system. In this way, subscriber PV receive
 * all data but the control system is not overloaded.</li>
 * <li><em>Configure "DAN-0" PVs to stream %DAN data</em>. The following PVs could be configured:
 *   <ul>
 *      <li><em>DAN-0-DAN-SourceName</em>: <code>caput -S DAN-0-DAN-SourceName NdsDanSource </code> </li>
 *      <li><em>DAN-0-DAN-ICProgramName</em>: <code>caput -S DAN-0-DAN-ICProgramName nds-dan </code> </li>
 *      <li><em>DAN-0-DAN-PathXML</em>: <code>caput -S DAN-0-DAN-PathXML PathTo/DeviceDANCopy/conf/danConfig.xml </code> </li>
 *      <li><em>DAN-0-DAN-SamplingRate</em>: <code>caput DAN-0-DAN-SamplingRate 100000 </code> </li>
 *      <li><em>DAN-0-DAN-BufferSize</em>: <code>caput DAN-0-DAN-BufferSize 1310720 </code> </li>
 *      <li><em>DAN-0-DAN-BlockNumberOfSamples</em>: <code>caput DAN-0-DAN-BlockNumberOfSamples 40960 </code> </li>
 *      <li><em>DAN-0-DAN-TimeBetweenBlocks</em>: <code>caput DAN-0-DAN-TimeBetweenBlocks 1000000 </code> </li>
 *      <li><em>DAN-0-DAN-MaxAcquisitionTime</em>: <code>caput DAN-0-DAN-MaxAcquisitionTime 3000000000 </code> </li>
 *      <li><em>DAN-0-DAN-NumberOfBlocks</em>: <code>caput DAN-0-DAN-NumberOfBlocks 100 </code> </li>
 *   </ul>
 * </li>
 * <li><em>Start streaming %DAN data with "DAN-0"</em>. The following PVs shall be set:
 *   <ul>
 *      <li><em>DAN-0-DAN-StateMachine-setState</em>: <code>caput DAN-0-DAN-StateMachine-setState ON </code> </li>
 *      <li><em>DAN-0-DAN-StateMachine-setState</em>: <code>caput DAN-0-DAN-StateMachine-setState RUNNING </code> </li>
 *   </ul>
 * </li>
 * <li><em>Stop streaming %DAN data with "DAN-0"</em>. The following PVs shall be set:
 *   <ul>
 *      <li><em>DAN-0-DAN-StateMachine-setState</em>: <code>caput DAN-0-DAN-StateMachine-setState ON </code> </li>
 *      <li><em>DAN-0-DAN-StateMachine-setState</em>: <code>caput DAN-0-DAN-StateMachine-setState OFF </code> </li>
 *   </ul>
 * </li>
 * </ol>
 *
 * <b>Output</b> @n
 * Output files are generated in <code>/tmp/data</code>. @n
 *
 * @ingroup Samples CopyBuffer
 */
class DeviceNDSDANCopy
{

public:

  /**
   * @brief Constructor to instantiate a device driver.
   * @param factory Control System's factory where the device driver will belong to.
   * @param deviceName Name of the device's parent root node inside the control system.
   * @param namedParameters Optional parameters. Not used in this example.
   */
  DeviceNDSDANCopy(nds::Factory& factory, const std::string& deviceName, const nds::namedParameters_t& namedParameters);

  /**
   * Destructor to release resources.
   */
  ~DeviceNDSDANCopy();

#ifndef EPICS

  /**
   * @brief Function required by NDS to allocate device driver's resources.
   * @param factory Control System's factory where the device driver will belong to.
   * @param deviceName Name of the device's parent root node inside the control system.
   * @param namedParameters Optional parameters. Not used in this example.
   */
  static void* allocateDevice(nds::Factory& factory, const std::string& deviceName, const nds::namedParameters_t& namedParameters);

  /**
   * @brief Function required by NDS to release device driver's resources.
   * @param deviceName Name of the device's parent root node inside the control system.
   */
  static void deallocateDevice(void* deviceName);

#endif

  /**
   * @brief Function to set the maximum number of channels of the device.
   * @param timestamp Timestamp to indicate when the function is called.
   * @param maxNumberOfChannels Maximum number of channels to be set.
   * It can only be called when the state machine is in state OFF.
   */
  void setMaxChannels(const timespec& timestamp, const std::int32_t& maxNumberOfChannels);

  /**
   * @brief Switch on the device driver.
   * This method is required by the StateMachine's constructor and its implementation is empty in this example.
   */
  void stm_SwitchOn(void);
  /**
   * @brief Switch off the device driver.
   * This method is required by the StateMachine's constructor and its implementation is empty in this example.
   */
  void stm_SwitchOff(void);
  /**
   * @brief Start the device driver.
   * This method is required by the StateMachine's constructor and its implementation is empty in this example.
   */
  void stm_Start (void);
  /**
   * @brief Stop the device driver.
   * This method is required by the StateMachine's constructor and its implementation is empty in this example.
   */
  void stm_Stop(void);
  /**
   * @brief Recover the device driver from a FAULT state.
   * This method is required by the StateMachine's constructor and its implementation is empty in this example.
   */
  void stm_recover(void);

  /**
   * @brief Check if state transitions is allowed.
   * @return <code>TRUE</code> when transition is allowed, <code>FALSE</code> otherwise.
   * This method is required by the StateMachine's constructor and it verifies if the required state transition is allowed.
   */
  bool stm_allowChange(const nds::state_t currentState, const nds::state_t currentGlobalState, const nds::state_t desiredState);

  /**
   * @brief Initialise the structure required for publishing data with %DAN.
   * @param dataDAN Pointer to the data structure.
   * @return The pointer to the internal memory buffer to store the data, or <code>NULL</code> if the buffer cannot be allocated.
   * It allocates the internal memory buffer according to the size provided by @ref ndsdan::DANCP::getBufferSize() function.
   */
  void* dancp_initData(ndsdan::dataDAN_t* dataDAN);
  
  /**
   * @brief Release the structure required for publishing data with %DAN.
   * @param dataStructure Pointer to the data structure.
   * @return <code>NULL</code> if the data structure is successfully released.
   */
  void* dancp_releaseData(void* dataStructure);
  /**
   * @brief Set the header of the  data block published in %DAN.
   * @param dataDAN Data structure.
   * @return Pointer to the text header text, or <code>NULL</code> if the header is empty.
   * This method is required by the DANDA's constructor and it always returns <code>NULL</code> in this example.
   */
  char* dancp_setHeader(const ndsdan::dataDAN_t& dataDAN);


private:

  /**
   * @brief Root name of the device driver.
   */
  std::string m_deviceName;
  /**
   * @brief Internal counter of the published samples.
   */
  std::uint64_t m_samplesCounter;
  /**
   * @brief: Pointer to the internal buffer to store the data.
   */
  std::int16_t* m_danBuffer;
  /**
   * @brief Root node of the device driver.
   */
  nds::Port rootNode;
  /**
   * @brief PV to deal with the data that are received from other device.
   * Whenever new data is received in this PV, the function @ref is called.
   */
  nds::PVDelegateOut<std::vector<std::int16_t>> m_data_PV; //PV: Using this datatype to manage a generic buffer with Data
  /**
   * @brief PV to set the Maximum number of channels.
   */
  nds::PVDelegateOut<std::int32_t> m_MaxChannels_PV; //PV to set the maximum number of channels allowed
  /**
   * @brief Readback-value PV of the Maximum number of channels.
   */
  nds::PVVariableIn<std::int32_t> m_MaxChannels_RBVPV; //Readback value PV to set the maximum number of channels allowed
  /**
   * @brief State machine to control the device
   */
  nds::StateMachine m_stateMachine;
  /**
   * @brief Internal structure to handle the %DAN data.
   */
  ndsdan::DANCP m_DANCP;
  /**
   * @brief Internal method to initialise PVs with a default value.
   */
  void initializePVs(void);

  /**
   *
   * @param timestamp
   * @param value
   * It copies the received data into the local storage buffer to be published with %DAN.
   */
  void PV_buffer_writer(const timespec& timestamp, const std::vector<std::int16_t>& value);



};

#endif /* NDS_DAN_EXAMPLES_INCLUDE_DeviceNDSDANCopy_H_ */
