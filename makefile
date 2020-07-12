# C Compiler
COMPILER = gcc

# folder containing the libraries
LIBRARY = ../

# additional include files
INCLUDE_FILES  = $(LIBRARY)_standard/*.h $(LIBRARY)mfet_calculator/*.h $(LIBRARY)algorithms/*.h $(LIBRARY)read_write_image_file/*.h $(LIBRARY)graphplotter3d/*h

# additional include folders
INCLUDE_FOLDER = -I $(LIBRARY)_standard/ -I $(LIBRARY)mfet_calculator/ -I $(LIBRARY)algorithms/ -I $(LIBRARY)read_write_image_file/ -I $(LIBRARY)graphplotter3d/


# Compiler flags (all warnings -Wall, optimization level -O3)
COMPILER_FLAGS = $(INCLUDE_FOLDER) -DNOSTDLIB -DARCH_X86 -static -m32 -march=i386 -ffreestanding -Wall -Wno-address

# all libraries (math library is -lm)
LINKER_FLAGS = -Wl,--nmagic,-melf_i386 -s -static-libgcc -lm


# object files
OBJECT_FILES = kernel.o \
               hal/all.o \
               src/screen.o \
               src/terminal.o \
               src/mcursor.o \
               src/files.o \
               src/userinterface.o \
               src/drawing_window.o \
               $(LIBRARY)_standard/_standard.o \
               $(LIBRARY)mfet_calculator/mfet_calculator.o \
               $(LIBRARY)graphplotter3d/graphplotter3d.o

# software output path and name
PROGRAM_NAME = ./kernel.elf


# Build software: link object files and library files into the final software
default: $(OBJECT_FILES)
	$(COMPILER) $(OBJECT_FILES) $(LINKER_FLAGS) -o $(PROGRAM_NAME)

# Compile .c files to .o files
%.o: %.c $(INCLUDE_FILES)
	$(COMPILER) $(COMPILER_FLAGS) -c -o $@ $<


# remove created object files only
clean:
	rm -f $(OBJECT_FILES)

# remove all created files
clean_all:
	rm -f $(OBJECT_FILES)
	rm -f $(PROGRAM_NAME)

# run software
run:
	$(PROGRAM_NAME)

