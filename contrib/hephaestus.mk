############################### HEPHAESTUS ######################################
#################################################################################
MFEM_DIR				:=$(APPLICATION_DIR)/../mfem/build

include $(MFEM_DIR)/config/config.mk

MFEM_INCLUDE 			:= -I$(MFEM_DIR) -I$(MFEM_SOURCE_DIR) -I$(MFEM_SOURCE_DIR)/miniapps/common
MFEM_LIB 				:= -L$(MFEM_DIR) -lmfem -lrt -L$(MFEM_DIR)/miniapps/common -lmfem-common

HEPHAESTUS_DIR 			:= $(APPLICATION_DIR)/contrib/hephaestus

HEPHAESTUS_LIB_DIR		:= $(HEPHAESTUS_DIR)/lib
HEPHAESTUS_LIB 			:= $(HEPHAESTUS_LIB_DIR)/libhephaestus.so 

HEPHAESTUS_INCLUDE_LIB	:= $(sort $(dir $(shell find $(HEPHAESTUS_DIR)/src/ -name "*.hpp")))
HEPHAESTUS_INCLUDE		:= $(foreach d, $(HEPHAESTUS_INCLUDE_LIB), -I$d)

HEPHAESTUS_CXX_FLAGS 	:= -Wall $(HEPHAESTUS_INCLUDE) $(MFEM_INCLUDE) -I$(MFEM_INC_DIR)/config
HEPHAESTUS_LDFLAGS		:= -Wl,-rpath,$(HEPHAESTUS_LIB_DIR) -L$(HEPHAESTUS_LIB_DIR) -lhephaestus $(MFEM_LIB) -lnetcdf

libmesh_CXXFlags 		+= $(HEPHAESTUS_CXX_FLAGS)
libmesh_LDFLAGS 		+= $(HEPHAESTUS_LDFLAGS)

ADDITIONAL_CPPFLAGS		+= -DHEPHAESTUS_ENABLED
ADDITIONAL_INCLUDES 	+= $(HEPHAESTUS_CXX_FLAGS)
ADDITIONAL_LIBS 		+= $(HEPHAESTUS_LDFLAGS)

$(info ADDITIONAL_INCLUDES = $(ADDITIONAL_INCLUDES));
$(info ADDITIONAL_LIBS     = $(ADDITIONAL_LIBS));
