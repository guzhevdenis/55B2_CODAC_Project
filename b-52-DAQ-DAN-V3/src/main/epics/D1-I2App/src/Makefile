TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

#========================================
# Build the IOC application

PROD_IOC = D1-I2
# D1-I2.dbd will be created and installed
DBD += D1-I2.dbd

# D1-I2.dbd will be made up from these files:
-include $(EPICS_ROOT)/mk/codac-common.mk
-include $(EPICS_ROOT)/mk/asyn.mk
-include $(EPICS_ROOT)/mk/nds3-epics.mk

# Add all the support libraries needed by this IOC
-include $(EPICS_ROOT)/mk/picmg.mk

D1-I2_DBD += $(CODAC_DBD)
D1-I2_LIBS += $(CODAC_LIBS)

# # SNCSEQ
# <sncProgram>_SNCFLAGS += +r -c +d
# D1-I2_DBD += <sncProgram>.dbd
# D1-I2_SRCS += <sncProgram>.stt
# D1-I2_LIBS += seq pv


# D1-I2_registerRecordDeviceDriver.cpp derives from D1-I2.dbd
D1-I2_SRCS += D1-I2_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
D1-I2_SRCS_DEFAULT += D1-I2Main.cpp
D1-I2_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#D1-I2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#=============================
# Include SNL program makefile snippets

-include ../*.snlprog

#=============================
#=============================
# Include user.mk makefile snippets

-include ../user.mk

#=============================

# Finally link to the EPICS Base libraries
D1-I2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
#----------------------------------------

