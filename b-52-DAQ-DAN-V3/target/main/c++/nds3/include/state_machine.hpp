#pragma once

#include "nifpga.hpp"
#include <chrono>
#include <thread>


namespace ni {
namespace fpga {

enum stream_state : uint8_t {
    idle,
    finite_transfer,
    continuous,
    done,
    error,
    not_ready,
    cancel
};


struct state_machine {
    state_machine(
        vi_interface& vi,
        uint32_t indicator,
        uint32_t control,
        bool periodic_state_sync = false,
        std::chrono::microseconds sync_interval = std::chrono::microseconds(1));
    ~state_machine();

    stream_state get_state();

    stream_state go_to_state(
        stream_state desired_state,
        std::chrono::microseconds timeout);

    private:
    vi_interface& vi;
    stream_state  state;
    uint32_t      indicator;
    uint32_t      control;

    bool periodic_state_sync;
    std::chrono::microseconds sync_interval;
    std::thread sync_routine;
};


/**
 * @brief The constructor of a state machine object.
 * 
 * @param vi reference to a valid ni::fpga::vi_interface
 * @param indicator handle to state indicator on the VI
 * @param control handle to state control on the VI
 * @param periodic_state_sync if 'true' - a state sync
 *                            routine is launched
 * @param sync_interval specifies time interval for
 *                      regular state synchronization;
 *                      default is 1 microsecond
 * @return state_machine:: 
 */
state_machine::
state_machine(vi_interface& vi,
              uint32_t indicator,
              uint32_t control,
              bool periodic_state_sync,
              std::chrono::microseconds sync_interval)
: vi(vi)
, indicator(indicator)
, control(control)
, periodic_state_sync(periodic_state_sync)
, sync_interval(sync_interval) {
    if (periodic_state_sync) {
        sync_routine =
            std::thread(
                [this] () {
                    while (this->periodic_state_sync) {
                        this->vi.read(this->indicator, (uint8_t*)&(this->state));
                        std::this_thread::sleep_for(std::chrono::microseconds(this->sync_interval));
                    }
                }
            ); // state sync routine
    }
}


/**
 * @brief Destructor. If a state sync routine is active,
 *        it is stopped
 * 
 */
state_machine::
~state_machine() {
    if (periodic_state_sync) {
        periodic_state_sync = false;
        sync_routine.join();
    }
}


/**
 * @brief Get the state of the stream
 * 
 * @return stream_state 
 */
stream_state state_machine::
get_state() {
    if (!periodic_state_sync) vi.read(indicator, (uint8_t*)&state);
    return state;
}


/**
 * @brief Command the state machine to transition to a specified
 *        state
 * 
 * @param desired_state 
 * @param timeout (required)
 * 
 * @return If the state change was successful, timed_out == false
 *         is returned; if the state of the stream hasn't changed
 *         to the desired value, timed_out == true is returned.
 *         To get the actual state, the get_state() method should
 *         be called.
 * 
 */
stream_state state_machine::
go_to_state(stream_state desired_state,
            std::chrono::microseconds timeout) {
    vi.write(control, (uint8_t)desired_state);
    // Start timeout clock
    bool timed_out = false;
    auto start = std::chrono::high_resolution_clock::now();
    auto curr  = std::chrono::high_resolution_clock::now();
    // Wait for state transition or timeout
    while (state != desired_state && !timed_out) {
        vi.read(indicator, (uint8_t*)&state);
        curr = std::chrono::high_resolution_clock::now();
        timed_out = (curr - start >= timeout);
        vi.write(control, (uint8_t)desired_state);
    }
    return state;
}

} // namespace fpga
} // namespace ni
