############################### HEPHAESTUS #######################################h
###############################################################################
# source files

MFEM_INC= -I$(MFEM_DIR)  -I$(MFEM_SOURCE_DIR) -I$(MFEM_SOURCE_DIR)/miniapps/common
MFEM_LIB= -L$(MFEM_DIR) -lmfem -lrt -L$(MFEM_DIR)/miniapps/common -lmfem-common

HEPHAESTUS_INCDIR = $(sort $(dir $(shell find $(HEPHAESTUS_DIR)/src/ -name "*.hpp")))
HEPHAESTUS_INC=$(foreach d, $(HEPHAESTUS_INCDIR), -I$d)
HEPHAESTUS_LIBDIR = $(HEPHAESTUS_DIR)/lib
HEPHAESTUS_LIB = -Wl,-rpath,$(HEPHAESTUS_LIBDIR) -L$(HEPHAESTUS_LIBDIR) -lhephaestus -lnetcdf

ADDITIONAL_INCLUDES += $(HEPHAESTUS_INC) $(MFEM_INC) -I$(MFEM_INC_DIR)/config
ADDITIONAL_LIBS += $(HEPHAESTUS_LIB) $(MFEM_LIB)

$(info ADDITIONAL_INCLUDES = $(ADDITIONAL_INCLUDES));
$(info ADDITIONAL_LIBS = $(ADDITIONAL_LIBS));
