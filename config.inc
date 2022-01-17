MFEM_DIR=../mfem_build
CONTRIB_DIR=./contrib/hephaestus
MFEM_INC= -I$(MFEM_DIR) -I$(CONTRIB_DIR) -I../mfem/miniapps/common
MFEM_LIB= -L$(MFEM_DIR) -lmfem -lrt -L../mfem_build/miniapps/common -lmfem-common

