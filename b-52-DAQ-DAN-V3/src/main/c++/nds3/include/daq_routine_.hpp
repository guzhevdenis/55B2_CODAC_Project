#include <nifpga.hpp>
#include <vector>
#include <algorithm>


/* Data acquisition functor */
template<typename DataType>
struct daq_routine {
    daq_routine(ni::fpga::vi_interface& vi, uint32_t fifo, size_t fifo_depth);
    ~daq_routine();

    void reset(); // 

    void operator()(
        std::vector<DataType>& destination,
        size_t sample_size
    );

    enum class call_result {
        never_called = -1,
        success,
        fail
    }; // !!! list of possible values is subject to change

    private:

    ni::fpga::vi_interface& vi;
    uint32_t                fifo;
    size_t                  fifo_depth;

    bool running; // indicates that the routine is currently active
    bool    done; // indicates that the routine has finished executing

    // currently for debug
    ni::fpga::status last_fifo_read_status;

    call_result result;
    
}; // struct daq_routine


/* DAQ routine constructor */
template<typename DataType>
daq_routine<DataType>::daq_routine(
    ni::fpga::vi_interface& vi,
    uint32_t fifo,
    size_t fifo_depth
)
: vi(vi)
, fifo(fifo)
, fifo_depth(fifo_depth)
, running(false)
, done(false)
, last_fifo_read_status(NiFpga_Status_Success)
, result(call_result::never_called) {}


/* Reset routine */
template<typename DataType>
void daq_routine<DataType>::reset() {
    if (running) return;
    done = false;
}

template<typename DataType>
void daq_routine<DataType>::operator()(
    std::vector<DataType>& destination,
    size_t sample_size
) {
    // signal the parent thread about starting DAQ
    done = false;
    running = true;

    // limit maximum FIFO memory block reservation
    // to avoid timeouts. Currently arbitrary (!)
    size_t max_reserve_size = fifo_depth / 2;

    DataType* fifo_buffer = nullptr;

    size_t elements_requested = 0;
    size_t elements_acquired = 0;
    size_t elements_remaining = 0;

    // reserve memory to avoid costly allocations in run-time    
    destination.reserve(std::min(sample_size, destination.max_size()));

    // read data from FIFO
    while (destination.size() < sample_size) {
        // acquire elements of the buffer for reading
        last_fifo_read_status =
            vi.acquire_fifo_read_elements(
                fifo,
                &fifo_buffer,
                elements_requested,
                5000,
                &elements_acquired,
                &elements_remaining
            );

        // copy all acquired elements
        if (NiFpga_IsNotError(last_fifo_read_status)) {
            destination.insert(destination.end(), fifo_buffer, fifo_buffer+elements_acquired);
        }

        // release acquired elements
        vi.release_fifo_elements(fifo, elements_acquired);

        // set the number of elements to be requested on the next iteration
        elements_requested = std::min(elements_remaining, max_reserve_size);

    } // while loop

    running = false;
    done = true;

    if (last_fifo_read_status == NiFpga_Status_Success) {
        result = call_result::success;
    } else {
        result = call_result::fail;
    }
} // daq_routine<DataType>::operator()