/**
 * @file definitions.h
 *
 * @brief Header file to define data structures, function prototypes and logging macros for @e nds-dan module
 *
 * @author Javier Moreno (GMV)
 * @version 1.0
 * @ingroup DataTypes
 *
 * Include @ref nds-dan.h instead of this file to have full access to the @e nds-dan API.
 */

#ifndef NDS_DAN_INCLUDE_DEFINITIONS_H_
#define NDS_DAN_INCLUDE_DEFINITIONS_H_

#include <mutex>
#include <condition_variable>

#include <log.h>
#include <danApi.h>

#include <nds3/nds.h>


/**
 * @mainpage
 *
 * This module creates the @e NDS support for @e %DAN integration. It allows extending @e NDS drivers
 * with additional nodes that provides access to @e %DAN API. In such way, NDS drivers can archive
 * data without advanced acknowledge of @e %DAN. @n
 *
 * <b><a>Requirements</a></b> @n
 * <ul>
 * <li><b>DAN writer service</b> must be started before running IOCs that use this module. To do that,
 * run the command: <code>sudo dan_archiver_writer start</code></li>.
 * </ul>
 *
 * @defgroup NDS-DAN
 * Module that details information about general features that can be controlled for a @e NDS driver
 * that includes a @e nds-dan node.
 *
 * @defgroup CopyBuffer
 * @ingroup NDS-DAN
 * Module that is focused in @e %DAN features that can be controlled for a @e NDS driver that
 * includes a @e nds-dan node which deals with <b>copy memory buffer</b>.
 *
 * @defgroup SharedBuffer
 * @ingroup NDS-DAN
 * Module that is focused in @e %DAN features that can be controlled for a @e NDS driver that
 * includes a @e nds-dan node which deals with <b>shared memory buffer</b>.
 *
 * @defgroup Samples
 * @ingroup NDS-DAN
 * Module that contains @e NDS sample drivers that shows how @e nds-dan features can be used. @n
 * The same sample drivers included in this group are used for automatic @e C++ and @e EPICS validation tests.
 *
 * @defgroup DataTypes
 * @ingroup NDS-DAN
 * Definitions of data type structures and functions to be used by classes and methods.
 *
 * @defgroup Logging
 * @ingroup NDS-DAN
 * Module that defines special MACROS to log message not only by using the @e NDS methods but
 * also the @e log ones given by @e ITER.
 */

/**
 * @ingroup NDS-DAN
 * @brief Namespace that contains all the module classes, data types and declarations.
 */
namespace ndsdan
{

// ---------------------------------------------------------------------------------------------
// ------------------------------------- MACROs definition -------------------------------------
// ---------------------------------------------------------------------------------------------

/**
 * @ingroup Logging
 * @brief Log the text given in the debug stream.
 * @param node NDS node to stream the information.
 * @param text Text to be logged.
 */
#define ndsDANLogDebug(node, text) { \
    ndsDebugStream(node) << text << std::endl << std::flush; \
    log_debug(text); \
  }

/**
 * @ingroup Logging
 * @brief Log the text given in the information stream.
 * @param node NDS node to stream the information.
 * @param text Text to be logged.
 */
#define ndsDANLogInfo(node, text) { \
    ndsInfoStream(node) << text << std::endl << std::flush; \
    log_info(text); \
  }

/**
 * @ingroup Logging
 * @brief Log the text given in the warning stream.
 * @param node NDS node to stream the information.
 * @param text Text to be logged.
 */
#define ndsDANLogWarning(node, text) { \
    ndsWarningStream(node) << text << std::endl << std::flush; \
    log_warning(text); \
  }

/**
 * @ingroup Logging
 * @brief Log the text given in the error stream.
 * @param node NDS node to stream the information.
 * @param text Text to be logged.
 */
#define ndsDANLogError(node, text) { \
    ndsErrorStream(node) << text << std::endl << std::flush; \
    log_error(text); \
  }

// ---------------------------------------------------------------------------------------------
// ---------------------------------- End of MACROs definition ---------------------------------
// ---------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------
// ------------------------------ Functions prototypes definition ------------------------------
// ---------------------------------------------------------------------------------------------

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with generic pointers.
   * It can be used not only in data structures but also as arguments or
   * return data type in functions. @n
   * In practice, it is translated to: @code void* @endcode
   */
  typedef void *genericPtr_t;

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with generic functions.
   * This kind of functions receives a @ref genericPtr_t and returns another @ref genericPtr_t.
   * In practice, it is translated to: @code void* functionName(void*) @endcode
   */
  typedef std::function<void* (void*)> genericFunction_t;

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with generic functions that receives a
   * pointer to @ref dataDAN_t data type.
   * This kind of functions receives a @ref dataDAN_t pointer and returns a @ref genericPtr_t.
   * In practice, it is translated to: @code void* functionName(dataDAN_t*) @endcode
   */
  typedef std::function<void* (struct dataDAN_t*)> genericDANFunction_t;

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with functions that receives a
   * constant reference to @ref dataDAN_t data type and return a pointer to
   * character data type.
   * This kind of functions receives a @ref dataDAN_t constant reference and returns a
   * pointer to @e char data type.
   * In practice, it is translated to: @code char* functionName(const dataDAN_t&) @endcode
   */
  typedef std::function<char* (const struct dataDAN_t&)> charDANFunction_t;

  /**
   * @ingroup DataTypes SharedBuffer
   * @brief Data type definition to deal with functions that receives two
   * constant references to @ref dataDAN_t and @ref dataDANDA_t data types
   * and return a long  data type.
   * This kind of functions receives a @ref dataDAN_t constant reference and
   * a @ref dataDANDA_t constant reference, and it returns a @e std::int32_t value.
   * In practice, it is translated to:
   * @code std::int32_t functionName(const dataDAN_t&, const dataDANDA_t&) @endcode
   * @todo Change type to <b>std::uint64_t</b> or equivalent.
   */
  typedef std::function<std::int32_t (const struct dataDAN_t&, const struct dataDANDA_t&)> intDANDAFunction_t;

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with functions that receives a
   * pointer to @ref dataDAN_t data type and return a long  data type.
   * This kind of functions receives a @ref dataDAN_t pointer and
   * returns a @e std::int32_t value.
   * In practice, it is translated to:
   * @code std::int32_t functionName(dataDAN_t*) @endcode
   * @todo Change type to <b>std::uint64_t</b> or equivalent.
   */
  typedef std::function<std::int32_t (struct dataDAN_t*)> longDANFunction_t;

  /**
   * @ingroup DataTypes
   * @brief Data type definition to deal with functions that receives a
   * constant reference to @ref dataDAN_t data type and return a pointer boolean data type.
   * This kind of functions receives a @ref dataDAN_t constant reference and returns a
   * boolean value.
   * In practice, it is translated to: @code bool functionName(const dataDAN_t&) @endcode
   */
  typedef std::function<bool (const struct dataDAN_t&)> boolDANFunction_t;

// ---------------------------------------------------------------------------------------------
// --------------------------- End of Functions prototypes definition --------------------------
// ---------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------
// ----------------------------- Data types structures definition ------------------------------
// ---------------------------------------------------------------------------------------------


  /**
   * @ingroup DataTypes
   * @brief Enumeration data to handle the status of the DAN nodes.
   * According to the scope of each function, it may return a different code error to
   * be identified in the caller function.
   */
  enum class statusDAN_t : std::uint8_t
  {
    SUCCESS = 0      //!< Status is correct.
    , UNKNOWN = 1    //!< Status is unknown.
    , ERROR_XML = 2    //!< Error detected while loading XML configuration file, That is, while calling <i>dan_admin_api_load_fromFile</i> method.
    , ERROR_DAN_INIT = 3    //!< Error detected while initializing DAN library. That is, while calling <i>dan_initLibrary[_icprog]</i> method.
    , ERROR_DAN_CLOSE = 4    //!< Error detected while closing DAN library. That is, while calling <i>dan_closeLibrary[_icprog]</i> method.
    , ERROR_ICPROGRAM_UNLOAD = 5    //!< Error detected while closing ICProgram. IThat is, while calling <i>dan_admin_api_unload_icprog</i> method.
    , ERROR_TCN_INIT = 6    //!< Error detected while initializing TCN library. That is, while calling <i>tcn_init</i> method.
    , ERROR_TCN_CLOSE = 7    //!< Error detected while closing TCN library. That is, while calling <i>tcn_finalize</i> method.
    , ERROR_DATA_INIT = 8    //!< Error detected while user function to initialize the data structure.
    , ERROR_DATA_RELEASE = 9    //!< Error detected while user function to release the data structure.
    , ERROR_PUBLISH_SOURCE = 10    //!< Error detected while calling <i>dan_publisher_publishSource[_withDAQBuffer]</i> method.
    , ERROR_UNPUBLISH_SOURCE = 11    //!< Error detected while calling <i>dan_publisher_unpublishSource</i> method.
    , ERROR_OPEN_STREAM = 12    //!< Error detected while calling <i>dan_publisher_openStream</i> method.
    , ERROR_CLOSE_STREAM = 13    //!< Error detected while calling <i>dan_publisher_closeStream</i> method.
    , ERROR_PUT_DATABLOCK = 14    //!< Error detected while calling <i>putDataBlock</i> method.
    , ERROR_UNKNOWN = 15    //!< Error due to any other reason.
  };

  /**
   * @ingroup DataTypes
   * @brief Enumeration data to handle the status of the DAN buffer when data blocks are being pushed.
   */
  enum class statusStreamDAN_t : std::uint8_t
  {
    SUCCESS_BLOCK = 0      //!< Data block has been successfully put into the open stream.
    , UNKNOWN = 1    //!< Status is unknown.
    , CLOSED = 2 //!< Stream has not been open.
    , IDLE = 3 //!< Stream is open and waiting to put data blocks.
    , ERROR_OFFSET_OUT_RANGE = 4    //!< Block data cannot be put because the offset is out of the DAQ buffer range.
    , ERROR_NOT_OPEN_STREAM = 5    //!< Block data cannot be put because the stream has not been previously open.
    , ERROR_OVERFLOW_ALL = 6    //!< Block data cannot be put because buffer overflow is detected for all DAN streamers.
    , ERROR_OVERFLOW = 7    //!< Block data cannot be put because buffer overflow is detected but NOT for all DAN streamers.
    , ERROR_UNKNOWN = 8    //!< Block data cannot be put due to any other reason.
  };

  /**
   * @ingroup DataTypes
   * @brief Type defined to to gather data required by any standard usage of %DAN.
   * This structure is typically handled by DAN node (and its children nodes)
   * to deal with data more efficiently than using PVs.
   */
  struct dataDAN_t {
    /**
     * @brief Path to XML file required to initialise the @e danAPItool.
     * The configuration file pointed by this path is used by a DAN application
     * and its fields should be consistent with the values given by rest of data
     * fields within @ref dataDAN_t structure. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::string pathXML;

    /**
     * @brief Name of the data source specified in XML configuration file.
     * In <i>DAN User Manual</i>, this field is equivalent to the source tag
     * <b><i>source name</i></b> that is detailed section <i>%DAN API Configuration File</i>. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::string sourceName;

    /**
     * @brief Name of the ICrogram that defines the configuration model.
     * In <i>DAN User Manual</i>, this field is equivalent to the general tag
     * <b><i>ICprogram</i></b> that is detailed section <i>%DAN API Configuration File</i>. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::string ICProgramName;

    /**
     * @brief %DAN data core required by most of %DAN functions.
     * It is an internal data field that will be handled by %DAN functions. It is
     * typically obtained after initialising the <i>%DAN library</i>. @n
     * No modifications of this member field are required by the user.
     */
    genericPtr_t dataCore;

    /**
     * @brief %DAN data source required by most of %DAN functions.
     * It is an internal data field that will be handled by %DAN functions. It is
     * typically obtained after initialising the <i>DAN publisher</i>. @n
     * No modifications of this member field are required by the user.
     */
    genericPtr_t dataSource;

    /**
     * @brief Sampling Rate to publish data, in samples per second.
     * This parameter is used internally to open the data stream. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::int32_t samplingRate;

    /**
     * @brief Size of the internal DAQ buffer, in Bytes.
     * This size represents the total DAQ buffer size in Bytes.@n
     * For DANCP nodes, it must be provided before switching-on the state machine.
     * For instance, it can be the product of the @ref blockNumberOfSamples, @ref sampleSize
     * , @ref numberOfChannels and the @e numberOfBlocks in the DAQ buffer. @n
     * For DANDA nodes, it is automatically computed as the product of the @ref sampleSize 
     * and the <i> number of samples in DAN buffer </i> (provided by an user's function).
     */
    std::int32_t bufferSize;

    /**
     * @brief Initial offset in DAQ buffer associated with the %DAN data source, in Bytes.
     * This offset is required to open the %DAN stream and it is referred to the
     * position of payload.
     * It is an optional field that is set to 0 by default. It can only be modified with the
     * state machine is in @ref nds::state_t::OFF state.
     */
    std::int32_t offsetData; //Data offset in DAQ buffer

    /**
     * @brief Structure of data to access the DAQ buffer.
     * IN DANCP node, it usually points to the data structure allocated to store the data.
     * In DANDA node, it is the pointer given by the API's hardware to access
     * the shared buffer. @n
     * This field is usually initialised after the state machine is switched on, and it is
     * usually released when the state machine is in @ref nds::state_t::OFF state.
     */
    genericPtr_t dataStructure;

    /**
     * @brief Number of channels in the %DAN source.
     * It is the number of channels defined in the initialised %DAN configuration for
     * the underlying %DAN source. It is automatically obtained from the %DAN API
     * after initialising the source to publish data, that is, when the state
     * machine has been switched on. @n
     */
    std::int32_t numberOfChannels;

    /**
     * @brief Size of a single sample in the %DAN data buffer, in Bytes.
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::int32_t sampleSize;

    /**
     * @brief Size of a data block in the %DAN buffer, in number of samples.
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::int32_t blockNumberOfSamples;

    /**
     * @brief Number of samples available in %DAN buffer to be published.
     * By the moment, it is only used in DANDA node, and its value is provided
     * through a function by the driver developer. This function will typically
     * use the vendors API to provide the number of available data.
     */
    std::uint64_t numberOfSamples;

    /**
     * @brief Data block header to be included when data are published in %DAN.
     * This field is provided by the driver developer through a function that
     * should return NULL if no header has to be considered.
     */
    char* blockHeader;

    /**
     * @brief Counter of samples that have been published in the %DAN source, in number of samples.
     * This field is being updated while the publisher loop is iterating, that is,
     * while the state machine is in @ref nds::state_t::RUNNING state.
     * No modifications of this member field are required by the user.
     */
    std::uint64_t samplesCounter;

    /**
     * @brief Constructor to create an instance of the given data structure.
     * It initialises all its member variables with the default values: @n
     * <table>
     *  <tr> <td> <b><i> Field </i></b> </td> <td> <b> Value </b> </td> </tr>
     *  <tr> <td> <i> pathXML </i> </td> <td> "" </td> </tr>
     *  <tr> <td> <i> sourceName </i> </td> <td> "" </td> </tr>
     *  <tr> <td> <i> ICProgramName </i> </td> <td> "" </td> </tr>
     *  <tr> <td> <i> dataCore </i> </td> <td> NULL </td> </tr>
     *  <tr> <td> <i> dataSource </i> </td> <td> NULL </td> </tr>
     *  <tr> <td> <i> samplingRate </i> </td> <td> 10000 </td> </tr>
     *  <tr> <td> <i> bufferSize </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> offsetData </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> dataStructure </i> </td> <td> NULL </td> </tr>
     *  <tr> <td> <i> numberOfChannels </i> </td> <td> 1 </td> </tr>
     *  <tr> <td> <i> sampleSize </i> </td> <td> 1 </td> </tr>
     *  <tr> <td> <i> blockNumberOfSamples </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> blockHeader </i> </td> <td> NULL </td> </tr>
     *  <tr> <td> <i> samplesCounter </i> </td> <td> 0 </td> </tr>
     * </table>
     */
    dataDAN_t(void) :
      pathXML(""),
      sourceName(""),
      ICProgramName(""),
      dataCore(NULL),
      dataSource(NULL),
      samplingRate(10000),
      bufferSize(0),
      offsetData(0),
      dataStructure(NULL),
      numberOfChannels(1),
      sampleSize(1),
      blockNumberOfSamples(0),
      numberOfSamples(0),
      blockHeader(NULL),
      samplesCounter(0)
      {
      }
  };


  /**
   * @ingroup DataTypes SharedBuffer
   * @brief Type defined to to gather data required by those devices that provide
   * support for publishing data in %DAN by using its own <b>shared memory buffer</b>.
   * This structure is only handled by the @ref DANDA node to deal with data
   * more efficiently than using PVs.
   */
  struct dataDANDA_t {

    /**
     * @brief Number of the device descriptor of the device.
     * This field is used to connect with the device that is acquiring data.
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::int32_t deviceNumber;

    /**
     * @brief Path and name of the device descriptor of the source.
     * This field is used to identify the DAQ buffered area associated with the data source
     * and it is used by %DAN API to access the shared memory zone of such buffer. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::string deviceDescriptor;

    /**
     * @brief Mechanism used to access the external DAQ buffer.
     * It supports both @e NMAP and @e SHMGET modes. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    enum_daq_type shMemType;

    /**
     * @brief Size of the external DAQ buffer, in number of samples.
     * This field is a compulsory data provided by the driver developer through a function that
     * must return the buffer size, in number of samples.
     * @internal This field is equivalent to the one named <i>dma->ai.count</i>
     * in %DAN examples. @endinternal
     */
    std::uint64_t bufferNumberOfSamples;

    /**
     * @brief Initial offset in the external DAQ buffer, in Bytes.
     * It represents the offset if Bytes to start with the data information in the
     * external DAQ buffer, as vendors typically include reserved information at the
     * beginning of the buffer. @n
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     * @internal This field is the argument referred as @e refOffset in the %DAN API function
     * @e dan_publisher_publishSource. @endinternal
     */
    std::int64_t bufferInitialOffset;

    /**
     * @brief Current offset that is used to put data blocks in the %DAN source, in number of samples.
     * This field is being updated while the publisher loop is iterating, that is,
     * while the state machine is in @ref nds::state_t::RUNNING state.
     * No modifications of this member field are required by the user.
     * @internal This field is equivalent to the one named <i>offset_ai</i>
     * in %DAN examples. @endinternal
     */
    std::uint64_t bufferCurrentOffset;

    /**
     * @brief Constructor to create an instance of the given data structure.
     * It initialises all its member variables with the default values: @n
     * <table>
     *  <tr> <td> <b><i> Field </i></b> </td> <td> <b> Value </b> </td> </tr>
     *  <tr> <td> <i> deviceNumber </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> deviceDescriptor </i> </td> <td> "" </td> </tr>
     *  <tr> <td> <i> shMemType </i> </td> <td> DAN_DAQ_SHMGET </td> </tr>
     *  <tr> <td> <i> bufferNumberOfSamples </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> bufferInitialOffset </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> bufferCurrentOffset </i> </td> <td> 0 </td> </tr>
     * </table>
     */
    dataDANDA_t(void) :
      deviceNumber(0),
      deviceDescriptor(""),
      shMemType(enum_daq_type::DAN_DAQ_SHMGET),
      bufferNumberOfSamples(0),
      bufferInitialOffset(0),
      bufferCurrentOffset(0)
      {
      }
  };

  /**
   * @ingroup DataTypes CopyBuffer
   * @brief Type defined to to gather data required by publishers that <b>copy data</b>
   * to its own data buffer reserved in %DAN API.
   * This data type is required by publishers that do not provide its own shared memory
   * buffer, so an internal DAQ buffer must be allocated in the %DAN API. @n
   * This structure is only handled by the @ref DANCP node to deal with data
   * more efficiently than using PVs.
   */
  struct dataDANCP_t {

    /**
     * @brief Interval between two consecutive push operations to the %DAN source, in nanoseconds.
     * It is an optional field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state. @n
     * This field may be considered while the publisher loop is iterating, that is,
     * while the state machine is in @ref nds::state_t::RUNNING state. It can be used to
     * set the time slot between two consecutive push data operations to the %DAN API. @n
     */
    std::uint64_t timeBetweenBlocks;

    /**
     * @brief Maximum acquisition time to publish data in the %DAN source, in nanoseconds.
     * It is an optional field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state. @n
     * If its value is greater than zero, it defines the maximum time that the %DAN publisher
     * can be publishing data. This field is considered while the publisher loop is
     * iterating, that is, while the state machine is in @ref nds::state_t::RUNNING state. @n
     * If its value is lower or equal to zero, it does not affect to the %DAN publisher loop,
     * and data blocks will be being published while the state machine is in
     * @ref nds::state_t::RUNNING state.
     */
    double maxAcquisitionTimeNs;

    /**
     * @brief Size of the internal DAQ buffer reserved in the %DAN API, in number of data blocks.
     * It is a compulsory field. It can only be set when the state machine
     * is in @ref nds::state_t::OFF state.
     */
    std::int32_t numberOfBlocks;

    /**
     * @brief Constructor to create an instance of the given data structure.
     * It initialises all its member variables with the default values: @n
     * <table>
     *  <tr> <td> <b><i> Field </i></b> </td> <td> <b> Value </b> </td> </tr>
     *  <tr> <td> <i> timeBetweenBlocks </i> </td> <td> 0 </td> </tr>
     *  <tr> <td> <i> maxAcquisitionTime </i> </td> <td> -1.0 </td> </tr>
     *  <tr> <td> <i> maxAcquisitionTime </i> </td> <td> 0 </td> </tr>
     * </table>
     */
    dataDANCP_t(void) :
      timeBetweenBlocks(0),
      maxAcquisitionTimeNs(-1.0),
      numberOfBlocks(0)
    {
    }
  };

  // ---------------------------------------------------------------------------------------------
  // -------------------------- End of Data types structures definition --------------------------
  // ---------------------------------------------------------------------------------------------


}

#define NDS_DAN_API

#endif /* NDS_DAN_INCLUDE_DEFINITIONS_H_ */
