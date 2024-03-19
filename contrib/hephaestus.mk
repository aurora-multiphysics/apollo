############################### HEPHAESTUS ######################################
#################################################################################
MFEM_DIR				:=$(APPLICATION_DIR)/../mfem/build
HYPRE_DIR				:=$(APPLICATION_DIR)/../hypre-2.28.0/src/
HEPHAESTUS_DIR 			:=$(APPLICATION_DIR)/contrib/hephaestus

include $(MFEM_DIR)/config/config.mk
MFEM_INCLUDE 			:= -I$(MFEM_DIR) -I$(MFEM_SOURCE_DIR) -I$(MFEM_SOURCE_DIR)/miniapps/common
MFEM_LIB 				:= $(MFEM_DIR) -L$(MFEM_DIR) -lmfem -lrt $(MFEM_DIR)/miniapps/common/ -L$(MFEM_DIR)/miniapps/common -lmfem-common

HYPRE_INCLUDE 			:= -I$(HYPRE_DIR)
HYPRE_LIB 				:= -L$(HYPRE_DIR)/lib/ -lHYPRE

HEPHAESTUS_LIB_DIR		:= $(HEPHAESTUS_DIR)/lib
HEPHAESTUS_LIB 			:= $(HEPHAESTUS_LIB_DIR)/libhephaestus.so

HEPHAESTUS_INCLUDE_LIB	:= $(sort $(dir $(shell find $(HEPHAESTUS_DIR)/src/ -name "*.hpp")))
HEPHAESTUS_INCLUDE		:= $(foreach d, $(HEPHAESTUS_INCLUDE_LIB), -I$d) -I$(APPLICATION_DIR)/contrib/hephaestus/build/_deps/spdlog-src/include/

HEPHAESTUS_CXX_FLAGS 	:= -Wall $(HEPHAESTUS_INCLUDE) $(MFEM_INCLUDE) -I$(MFEM_INC_DIR)/config $(HYPRE_INCLUDE)
HEPHAESTUS_LDFLAGS		:= -Wl,-rpath,$(HEPHAESTUS_LIB_DIR) -L$(HEPHAESTUS_LIB_DIR) -lhephaestus $(MFEM_LIB) ${HYPRE_LIB} -L$(APPLICATION_DIR)/contrib/hephaestus/build/_deps/spdlog-build/ -lspdlogd

libmesh_CXXFlags 		+= $(HEPHAESTUS_CXX_FLAGS)
libmesh_LDFLAGS 		+= $(HEPHAESTUS_LDFLAGS)

ADDITIONAL_CPPFLAGS		+= -DHEPHAESTUS_ENABLED
ADDITIONAL_INCLUDES 	+= $(HEPHAESTUS_CXX_FLAGS)
ADDITIONAL_LIBS 		+= $(HEPHAESTUS_LDFLAGS)

$(info ADDITIONAL_INCLUDES = $(ADDITIONAL_INCLUDES));
$(info ADDITIONAL_LIBS     = $(ADDITIONAL_LIBS));
