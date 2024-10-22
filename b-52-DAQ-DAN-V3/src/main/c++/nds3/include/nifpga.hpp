#ifndef NIFPGA_HPP
#define NIFPGA_HPP

#include "NiFpga.h"

namespace ni
{
namespace fpga 
{
    /**
     * Aliases
     */
    const auto initialize = NiFpga_Initialize;
    const auto finalize   = NiFpga_Finalize;

    using status          = NiFpga_Status;
    using session         = NiFpga_Session;
    using open_attribute  = NiFpga_OpenAttribute;
    using close_attribute = NiFpga_CloseAttribute;
    using run_attribute   = NiFpga_RunAttribute;
    using irq_context     = NiFpga_IrqContext;
    enum class Bool : uint8_t {
        True = NiFpga_True,
        False = NiFpga_False
    };



    /**
     * @brief A class for interfacing LabVIEW FPGA VIs
     * 
     */
    class vi_interface {

        public:
        
        vi_interface(const char*     bitfile,
                     const char*     signature,
                     const char*     resource,
                     open_attribute  open_attr,
                     close_attribute close_attr,
                     run_attribute   run_attr);
        ~vi_interface();
    
        // Get session open status
        inline status get_session_open_status() const;

        // Get/set the close attribute
        inline close_attribute get_close_attr() const;
        inline void set_close_attr(close_attribute new_close_attr);

        /* ------------------------------------------------------------ */
        /* --------------- Firmware interface functions --------------- */
        /* ------------------------------------------------------------ */

        inline status run() const;
        inline status abort() const;
        inline status reset() const;
        inline status download() const;

        // Get/set the run attribute
        inline run_attribute get_run_attr() const;
        inline void set_run_attr(run_attribute new_run_attr);


        /* ------------------------------------------------------------ */
        /* ------------ VI front panel interface functions ------------ */
        /* ------------------------------------------------------------ */

        /**
         * @brief Functions below support only following numeric types:
         *      Bool     - boolean type (True/False)
         *      int8_t   - signed 8-bit integer
         *      uint8_t  - unsigned 8-bit integer
         *      int16_t  - signed 16-bit integer
         *      uint16_t - unsigned 16-bit integer
         *      int32_t  - signed 32-bit integer
         *      uint32_t - unsigned 32-bit integer
         *      int64_t  - signed 64-bit integer
         *      uint64_t - unsigned 64-bit integer
         *      float    - single precision float
         *      double   - double precision float
         */


        /**
         * @brief A function for reading
         * generic controls and indicators
         * 
         * @param resource 
         * @param value 
         * @return result of the call 
         */
        template<typename T>
        status read(uint32_t resource, T* value);


        // Read generic control/indicator array
        template<typename T>
        status read_array(uint32_t resource, T* arr, size_t size);


        // Read data from a generic FIFO
        template<typename T>
        status read_fifo(uint32_t       resource,
                         T*             data,
                         size_t         numberOfElements,
                         uint32_t       timeout,
                         size_t*        elementsRemaining);
                         

        // Read data from a generic FIFO by acquiring its elements
        template<typename T>
        status acquire_fifo_read_elements(uint32_t       fifo,
                                          T**            elements,
                                          size_t         elementsRequested,
                                          uint32_t       timeout,
                                          size_t*        elementsAcquired,
                                          size_t*        elementsRemaining);

        
        /**
         * @brief A function for writing
         * generic controls and indicators
         * 
         * @param resource 
         * @param value 
         * @return result of the call
         */
        template<typename T>
        status write(uint32_t resource, T value);


        // Write generic control/indicator array
        template<typename T>
        status write_array(uint32_t resource, const T* arr, size_t size);


        // Write data to a generic FIFO
        template<typename T>
        status write_fifo(uint32_t       resource,
                          const T*       data,
                          size_t         numberOfElements,
                          uint32_t       timeout,
                          size_t*        emptyElementsRemaining);


        // Write data to a generic FIFO by acquiring its elements
        template<typename T>
        status acquire_fifo_write_elements(uint32_t       fifo,
                                           T**            elements,
                                           size_t         elementsRequested,
                                           uint32_t       timeout,
                                           size_t*        elementsAcquired,
                                           size_t*        elementsRemaining);


        // Configure generic FIFO
        inline status configure_fifo(uint32_t fifo, size_t depth);
        inline status configure_fifo2(uint32_t fifo, size_t requestedDepth, size_t* actualDepth);


        // Start FIFO
        inline status start_fifo(uint32_t fifo);


        // Stop FIFO
        inline status stop_fifo(uint32_t fifo);


        // Release FIFO elements
        inline status release_fifo_elements(uint32_t fifo, size_t elements);


        // Reserve IRQ context
        inline status reserve_irq_context(irq_context* context);


        // Unreserve IRQ context
        inline status unreserve_irq_context(irq_context context);


        // Wait on IRQs
        inline status wait_on_irqs(irq_context       context,
                                   uint32_t          irqs,
                                   uint32_t          timeout,
                                   uint32_t*         irqsAsserted,
                                   NiFpga_Bool*      timedOut);


        // Acknowledge IRQs
        inline status acknowledge_irqs(uint32_t irqs);

        private:
        // Private members
        status              open_status;
        session             session_id;
        close_attribute     close_attr;
        run_attribute       run_attr;
        vi_interface& operator=(vi_interface&);
        vi_interface(const vi_interface&);
        //static unsigned int number_of_sessions;
    };



    /* ---------------------------------------------------------------- */
    /* ----------------------- Implementations ------------------------ */
    /* ---------------------------------------------------------------- */



    vi_interface::
    vi_interface(const char*     bitfile,
                 const char*     signature,
                 const char*     resource,
                 open_attribute  open_attr  = NiFpga_OpenAttribute_NoRun,
                 close_attribute close_attr = NiFpga_CloseAttribute_NoResetIfLastSession,
                 run_attribute   run_attr   = static_cast<run_attribute>(0))
    : close_attr(close_attr)
    , run_attr(run_attr) {
        //if(number_of_sessions == 0) {
            initialize();
        //    number_of_sessions++;
        //}
        open_status = NiFpga_Open(bitfile, signature, resource, open_attr, &session_id);
    }

    vi_interface::
    ~vi_interface() {
        abort();
        NiFpga_Close(session_id, close_attr);
        //if(number_of_sessions == 0) {
        //    number_of_sessions--;
            finalize();
        //}
    }

    status vi_interface::run()      const { return NiFpga_Run(session_id, run_attr); }
    status vi_interface::abort()    const { return NiFpga_Abort(session_id); }
    status vi_interface::reset()    const { return NiFpga_Reset(session_id); }
    status vi_interface::download() const { return NiFpga_Download(session_id); }

    // Get or set close attribute
    close_attribute vi_interface::get_close_attr() const { return close_attr; }
    void vi_interface::set_close_attr(NiFpga_CloseAttribute new_close_attr) { close_attr = new_close_attr; }

    // Get or set run attribute
    run_attribute vi_interface::get_run_attr() const { return run_attr; }
    void vi_interface::set_run_attr(NiFpga_RunAttribute new_run_attr) { run_attr = new_run_attr; }
        

    // Get session open status
    status vi_interface::get_session_open_status() const { return open_status; }


    // Configure generic FIFO
    status vi_interface::configure_fifo(uint32_t fifo, size_t depth) {
        return NiFpga_ConfigureFifo(session_id, fifo, depth);
    }
    status vi_interface::configure_fifo2(uint32_t fifo, size_t requestedDepth, size_t* actualDepth) {
        return NiFpga_ConfigureFifo2(session_id, fifo, requestedDepth, actualDepth);
    }
            
    // Start FIFO
    status vi_interface::start_fifo(uint32_t fifo) { return NiFpga_StartFifo(session_id, fifo); }

    // Stop FIFO
    status vi_interface::stop_fifo(uint32_t fifo)  { return NiFpga_StopFifo(session_id, fifo); }
            
        
    // Release FIFO elements
    status vi_interface::release_fifo_elements(uint32_t fifo, size_t elements) {
        return NiFpga_ReleaseFifoElements(session_id, fifo, elements);
    }

    // Reserve IRQ context
    status vi_interface::reserve_irq_context(NiFpga_IrqContext* context) {
        return NiFpga_ReserveIrqContext(session_id, context);
    }

    // Unreserve IRQ context
    status vi_interface::unreserve_irq_context(NiFpga_IrqContext context) {
        return NiFpga_UnreserveIrqContext(session_id, context);
    }

    // Wait on IRQs
    status vi_interface::wait_on_irqs(
        irq_context       context,
        uint32_t          irqs,
        uint32_t          timeout,
        uint32_t*         irqsAsserted,
        NiFpga_Bool*      timedOut
    ) {
        return NiFpga_WaitOnIrqs(session_id, context, irqs, timeout, irqsAsserted, timedOut);
    }

    // Acknowledge IRQs
    status vi_interface::acknowledge_irqs(uint32_t irqs) { return NiFpga_AcknowledgeIrqs(session_id, irqs); }


    /// Control and Indicator #read methods

    template<>
    status vi_interface::read<Bool>(uint32_t resource, Bool* value) {
        return NiFpga_ReadBool(session_id, resource, reinterpret_cast<NiFpga_Bool*>(value));
    }

    template<>
    status vi_interface::read<int8_t>(uint32_t resource, int8_t* value) {
        return NiFpga_ReadI8(session_id, resource, value);
    }

    template<>
    status vi_interface::read<uint8_t>(uint32_t resource, uint8_t* value) {
        return NiFpga_ReadU8(session_id, resource, value);
    }

    template<>
    status vi_interface::read<int16_t>(uint32_t resource, int16_t* value) {
        return NiFpga_ReadI16(session_id, resource, value);
    }

    template<>
    status vi_interface::read<uint16_t>(uint32_t resource, uint16_t* value) {
        return NiFpga_ReadU16(session_id, resource, value);
    }
    
    template<>
    status vi_interface::read<int32_t>(uint32_t resource, int32_t* value) {
        return NiFpga_ReadI32(session_id, resource, value);
    }

    template<>
    status vi_interface::read<uint32_t>(uint32_t resource, uint32_t* value) {
        return NiFpga_ReadU32(session_id, resource, value);
    }

    template<>
    status vi_interface::read<int64_t>(uint32_t resource, int64_t* value) {
        return NiFpga_ReadI64(session_id, resource, value);
    }

    template<>
    status vi_interface::read<uint64_t>(uint32_t resource, uint64_t* value) {
        return NiFpga_ReadU64(session_id, resource, value);
    }

    template<>
    status vi_interface::read<float>(uint32_t resource, float* value) {
        return NiFpga_ReadSgl(session_id, resource, value);
    }

    template<>
    status vi_interface::read<double>(uint32_t resource, double* value) {
        return NiFpga_ReadDbl(session_id, resource, value);
    }


    /// Control and Indicator #write methods

    template<>
    status vi_interface::write<Bool>(uint32_t resource, Bool value) {
        return NiFpga_WriteBool(session_id, resource, (NiFpga_Bool)value);
    }

    template<>
    status vi_interface::write<int8_t>(uint32_t resource, int8_t value) {
        return NiFpga_WriteI8(session_id, resource, value);
    }

    template<>
    status vi_interface::write<uint8_t>(uint32_t resource, uint8_t value) {
        return NiFpga_WriteU8(session_id, resource, value);
    }

    template<>
    status vi_interface::write<int16_t>(uint32_t resource, int16_t value) {
        return NiFpga_WriteI16(session_id, resource, value);
    }

    template<>
    status vi_interface::write<uint16_t>(uint32_t resource, uint16_t value) {
        return NiFpga_WriteU16(session_id, resource, value);
    }
    
    template<>
    status vi_interface::write<int32_t>(uint32_t resource, int32_t value) {
        return NiFpga_WriteI32(session_id, resource, value);
    }

    template<>
    status vi_interface::write<uint32_t>(uint32_t resource, uint32_t value) {
        return NiFpga_WriteU32(session_id, resource, value);
    }

    template<>
    status vi_interface::write<int64_t>(uint32_t resource, int64_t value) {
        return NiFpga_WriteI64(session_id, resource, value);
    }

    template<>
    status vi_interface::write<uint64_t>(uint32_t resource, uint64_t value) {
        return NiFpga_WriteU64(session_id, resource, value);
    }

    template<>
    status vi_interface::write<float>(uint32_t resource, float value) {
        return NiFpga_WriteSgl(session_id, resource, value);
    }

    template<>
    status vi_interface::write<double>(uint32_t resource, double value) {
        return NiFpga_WriteDbl(session_id, resource, value);
    }


    /// Control and Indicator Array #read methods

    template<>
    status vi_interface::read_array<Bool>(uint32_t resource, Bool* arr, size_t size) {
        return NiFpga_ReadArrayBool(session_id, resource, reinterpret_cast<NiFpga_Bool*>(arr), size);
    }

    template<>
    status vi_interface::read_array<int8_t>(uint32_t resource, int8_t* arr, size_t size) {
        return NiFpga_ReadArrayI8(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<uint8_t>(uint32_t resource, uint8_t* arr, size_t size) {
        return NiFpga_ReadArrayU8(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<int16_t>(uint32_t resource, int16_t* arr, size_t size) {
        return NiFpga_ReadArrayI16(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<uint16_t>(uint32_t resource, uint16_t* arr, size_t size) {
        return NiFpga_ReadArrayU16(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<int32_t>(uint32_t resource, int32_t* arr, size_t size) {
        return NiFpga_ReadArrayI32(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<uint32_t>(uint32_t resource, uint32_t* arr, size_t size) {
        return NiFpga_ReadArrayU32(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<int64_t>(uint32_t resource, int64_t* arr, size_t size) {
        return NiFpga_ReadArrayI64(session_id, resource, arr, size);
    }
    
    template<>
    status vi_interface::read_array<uint64_t>(uint32_t resource, uint64_t* arr, size_t size) {
        return NiFpga_ReadArrayU64(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<float>(uint32_t resource, float* arr, size_t size) {
        return NiFpga_ReadArraySgl(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::read_array<double>(uint32_t resource, double* arr, size_t size) {
        return NiFpga_ReadArrayDbl(session_id, resource, arr, size);
    }


    /// Control and Indicator Array #write methods

    template<>
    status vi_interface::write_array<Bool>(uint32_t resource, const Bool* arr, size_t size) {
        return NiFpga_WriteArrayBool(session_id, resource, reinterpret_cast<const NiFpga_Bool*>(arr), size);
    }

    template<>
    status vi_interface::write_array<int8_t>(uint32_t resource, const int8_t* arr, size_t size) {
        return NiFpga_WriteArrayI8(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<uint8_t>(uint32_t resource, const uint8_t* arr, size_t size) {
        return NiFpga_WriteArrayU8(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<int16_t>(uint32_t resource, const int16_t* arr, size_t size) {
        return NiFpga_WriteArrayI16(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<uint16_t>(uint32_t resource, const uint16_t* arr, size_t size) {
        return NiFpga_WriteArrayU16(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<int32_t>(uint32_t resource, const int32_t* arr, size_t size) {
        return NiFpga_WriteArrayI32(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<uint32_t>(uint32_t resource, const uint32_t* arr, size_t size) {
        return NiFpga_WriteArrayU32(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<int64_t>(uint32_t resource, const int64_t* arr, size_t size) {
        return NiFpga_WriteArrayI64(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<uint64_t>(uint32_t resource, const uint64_t* arr, size_t size) {
        return NiFpga_WriteArrayU64(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<float>(uint32_t resource, const float* arr, size_t size) {
        return NiFpga_WriteArraySgl(session_id, resource, arr, size);
    }

    template<>
    status vi_interface::write_array<double>(uint32_t resource, const double* arr, size_t size) {
        return NiFpga_WriteArrayDbl(session_id, resource, arr, size);
    }


    /// Target-to-Host FIFO #read methods

    template<>
    status vi_interface::
    read_fifo<Bool>(
        uint32_t       resource,
        Bool*          data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoBool(session_id, resource, reinterpret_cast<NiFpga_Bool*>(data), numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<int8_t>(
        uint32_t       resource,
        int8_t*        data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoI8(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<uint8_t>(
        uint32_t       resource,
        uint8_t*       data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining 
        ) {
        return NiFpga_ReadFifoU8(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<int16_t>(  
        uint32_t       resource,
        int16_t*       data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoI16(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<uint16_t>( 
        uint32_t       resource,
        uint16_t*      data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoU16(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<int32_t>(  
        uint32_t       resource,
        int32_t*       data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoI32(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<uint32_t>( 
        uint32_t       resource,
        uint32_t*      data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoU32(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<int64_t>(  
        uint32_t       resource,
        int64_t*       data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoI64(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<uint64_t>( 
        uint32_t       resource,
        uint64_t*      data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoU64(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<float>(   
         uint32_t       resource,
        float*         data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoSgl(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    template<>
    status vi_interface::
    read_fifo<double>(   
        uint32_t       resource,
        double*        data,
        size_t         numberOfElements,
        uint32_t       timeout,
        size_t*        elementsRemaining
        ) {
        return NiFpga_ReadFifoDbl(session_id, resource, data, numberOfElements, timeout, elementsRemaining);
    }
    
    
    /// Target-to-Host FIFO #write methods

    template<>
    status vi_interface::
    write_fifo<Bool>(    
        uint32_t        resource,
        const Bool*     data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoBool(session_id, resource, reinterpret_cast<const NiFpga_Bool*>(data), numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<int8_t>(  
        uint32_t        resource,
        const int8_t*   data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoI8(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<uint8_t>( 
        uint32_t        resource,
        const uint8_t*  data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoU8(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<int16_t>( 
        uint32_t        resource,
        const int16_t*  data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoI16(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<uint16_t>(
        uint32_t        resource,
        const uint16_t* data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoU16(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<int32_t>( 
        uint32_t        resource,
        const int32_t*  data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoI32(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<uint32_t>(
        uint32_t        resource,
        const uint32_t* data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoU32(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<int64_t>( 
        uint32_t        resource,
        const int64_t*  data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoI64(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<uint64_t>(
        uint32_t        resource,
        const uint64_t* data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoU64(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<float>(   
        uint32_t        resource,
        const float*    data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoSgl(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }
    
    template<>
    status vi_interface::
    write_fifo<double>(  
        uint32_t        resource,
        const double*   data,
        size_t          numberOfElements,
        uint32_t        timeout,
        size_t*         emptyElementsRemaining
    ) {
        return NiFpga_WriteFifoDbl(session_id, resource, data, numberOfElements, timeout, emptyElementsRemaining);
    }


    /// In-place FIFO #read methods

    template<>
    status vi_interface::
    acquire_fifo_read_elements<Bool>(
        uint32_t       fifo,
        Bool**         elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsBool(
            session_id,
            fifo,
            reinterpret_cast<NiFpga_Bool**>(elements),
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<int8_t>(
        uint32_t       fifo,
        int8_t**       elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsI8(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<uint8_t>(
        uint32_t       fifo,
        uint8_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsU8(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<int16_t>(
        uint32_t       fifo,
        int16_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsI16(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<uint16_t>(
        uint32_t       fifo,
        uint16_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsU16(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<int32_t>(
        uint32_t       fifo,
        int32_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsI32(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<uint32_t>(
        uint32_t       fifo,
        uint32_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsU32(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<int64_t>(
        uint32_t       fifo,
        int64_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsI64(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<uint64_t>(
        uint32_t       fifo,
        uint64_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsU64(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<float>(
        uint32_t       fifo,
        float**        elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsSgl(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_read_elements<double>(
        uint32_t       fifo,
        double**         elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoReadElementsDbl(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }


    /// In-place FIFO #write methods

    template<>
    status vi_interface::
    acquire_fifo_write_elements<Bool>(
        uint32_t       fifo,
        Bool**         elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsBool(
            session_id,
            fifo,
            reinterpret_cast<NiFpga_Bool**>(elements),
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<int8_t>(
        uint32_t       fifo,
        int8_t**       elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsI8(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<uint8_t>(
        uint32_t       fifo,
        uint8_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsU8(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<int16_t>(
        uint32_t       fifo,
        int16_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsI16(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<uint16_t>(
        uint32_t       fifo,
        uint16_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsU16(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<int32_t>(
        uint32_t       fifo,
        int32_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsI32(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<uint32_t>(
        uint32_t       fifo,
        uint32_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsU32(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<int64_t>(
        uint32_t       fifo,
        int64_t**      elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsI64(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<uint64_t>(
        uint32_t       fifo,
        uint64_t**     elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsU64(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<float>(
        uint32_t       fifo,
        float**        elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsSgl(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    template<>
    status vi_interface::
    acquire_fifo_write_elements<double>(
        uint32_t       fifo,
        double**         elements,
        size_t         elementsRequested,
        uint32_t       timeout,
        size_t*        elementsAcquired,
        size_t*        elementsRemaining
    ) {
        return NiFpga_AcquireFifoWriteElementsDbl(
            session_id,
            fifo,
            elements,
            elementsRequested,
            timeout,
            elementsAcquired,
            elementsRemaining
        );
    }

    

} // namespace fpga
} // namespace ni

#endif