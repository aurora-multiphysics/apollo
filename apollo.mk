#
# Check the existence of the contrib submodules and build accordingly
#
MFEM_DIR?=$(APOLLO_DIR)/../mfem/build
include $(MFEM_DIR)/config/config.mk


HEPHAESTUS_DIR?=$(APOLLO_DIR)/contrib/hephaestus
ADDITIONAL_CPPFLAGS += -DHEPHAESTUS_ENABLED
include $(APOLLO_DIR)/contrib/hephaestus.mk