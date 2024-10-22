############################################################################
## User provided PLC or fast controller driver pre configuration
############################################################################

# Enable parallel callback threads to improve 'I/O Intr' record scanning
#    see https://bugzilla.iter.org/codac/show_bug.cgi?id=10413

ndsLoadDriver /home/codac-dev/test_HMI-project/b-52-DAQ-DAN-V3/src/main/c++/nds3/target/lib/libNdsDaq.so
ndsCreateDevice(Device,B-0,"INIT=YES")
callbackParallelThreads

ndsLoadDriver /home/codac-dev/test_HMI-project/b-52-DAQ-DAN-V3/src/main/c++/nds3-dan/target/lib/libnds-dan-device-Copy.so
ndsCreateDevice(DeviceNDSDANCopy,DAN-0, "INIT=YES")

nds subscribe DAN-0-buffer B-0-DataAcquisitionNode-Data

#- End-of-file marker - do not delete or add lines below!
