############################### HEPHAESTUS ######################################
#################################################################################
MFEM_DIR				:=$(APPLICATION_DIR)/../mfem/build
HEPHAESTUS_DIR 			:=$(APPLICATION_DIR)/contrib/hephaestus

include $(MFEM_DIR)/config/config.mk
MFEM_INCLUDES 			:= -I$(MFEM_INC_DIR)/config -I$(MFEM_DIR)/ -I$(MFEM_DIR)/../miniapps/common
MFEM_LIBS 				:= -L$(MFEM_DIR) -lmfem -lrt -L$(MFEM_DIR)/miniapps/common -lmfem-common $(MFEM_LIB)

HEPHAESTUS_LIB		:= $(HEPHAESTUS_DIR)/lib

SPDLOG_LIB 			:=$(HEPHAESTUS_DIR)/build/_deps/spdlog-build/
SPDLOG_INCLUDE 		:=$(HEPHAESTUS_DIR)/build/_deps/spdlog-src/include/

HEPHAESTUS_INCLUDE_LIB	:= $(sort $(dir $(shell find $(HEPHAESTUS_DIR)/src/ -name "*.hpp")))
HEPHAESTUS_INCLUDE		:= $(foreach d, $(HEPHAESTUS_INCLUDE_LIB), -I$d) -I$(SPDLOG_INCLUDE)

HEPHAESTUS_CXX_FLAGS 	:= -Wall $(HEPHAESTUS_INCLUDE) $(MFEM_INCLUDES) -I$(MFEM_INC_DIR)/config
HEPHAESTUS_LDFLAGS		:= -Wl,-rpath,$(HEPHAESTUS_LIB) -L$(HEPHAESTUS_LIB) -lhephaestus $(MFEM_LIBS) -L$(SPDLOG_LIB) -lspdlog

libmesh_CXXFlags 		+= $(HEPHAESTUS_CXX_FLAGS)
libmesh_LDFLAGS 		+= $(HEPHAESTUS_LDFLAGS)

ADDITIONAL_CPPFLAGS		+= -DHEPHAESTUS_ENABLED
ADDITIONAL_INCLUDES 	+= $(HEPHAESTUS_CXX_FLAGS)
ADDITIONAL_LIBS 		+= $(HEPHAESTUS_LDFLAGS)

$(info ADDITIONAL_INCLUDES = $(ADDITIONAL_INCLUDES));
$(info ADDITIONAL_LIBS     = $(ADDITIONAL_LIBS));
