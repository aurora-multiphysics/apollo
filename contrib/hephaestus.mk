############################### HEPHAESTUS #######################################h
###############################################################################
# source files

MFEM_INC= -I$(MFEM_DIR)  -I$(MFEM_SOURCE_DIR) -I$(MFEM_SOURCE_DIR)/miniapps/common
MFEM_LIB= -L$(MFEM_DIR) -lmfem -lrt -L$(MFEM_DIR)/miniapps/common -lmfem-common

HEPHAESTUS_INCDIR = $(HEPHAESTUS_DIR)/include
HEPHAESTUS_LIBDIR = $(HEPHAESTUS_DIR)/lib
HEPHAESTUS_INC = -I$(HEPHAESTUS_INCDIR)
HEPHAESTUS_LIB = -Wl,-rpath,$(HEPHAESTUS_LIBDIR) -L$(HEPHAESTUS_LIBDIR) -lhephaestus -lnetcdf -lhdf5

ADDITIONAL_INCLUDES += $(HEPHAESTUS_INC) $(MFEM_INC) -I$(MFEM_INC_DIR)/config
EXTERNAL_FLAGS += $(HEPHAESTUS_LIB) $(MFEM_LIB)

$(info ADDITIONAL_INCLUDES = $(ADDITIONAL_INCLUDES));
$(info EXTERNAL_FLAGS = $(EXTERNAL_FLAGS));