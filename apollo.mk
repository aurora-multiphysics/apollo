#
# Check the existence of the contrib submodules and build accordingly
#
<<<<<<< HEAD
MFEM_DIR?=$(APPLICATION_DIR)/../mfem/build
=======
MFEM_DIR?=$(APOLLO_DIR)/../mfem
>>>>>>> 9c2bd8d (Changed name of mesh parameter that MFEMProblem accesses, so that it accesses the one named file and not _file_name)
include $(MFEM_DIR)/config/config.mk


HEPHAESTUS_DIR?=$(APPLICATION_DIR)/contrib/hephaestus
ADDITIONAL_CPPFLAGS += -DHEPHAESTUS_ENABLED
<<<<<<< HEAD
include $(APPLICATION_DIR)/contrib/hephaestus.mk
=======
include $(APOLLO_DIR)/contrib/hephaestus.mk
>>>>>>> 9c2bd8d (Changed name of mesh parameter that MFEMProblem accesses, so that it accesses the one named file and not _file_name)
