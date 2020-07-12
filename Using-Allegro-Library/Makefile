# C Compiler used
CC = gcc

# software output name
PROGRAM = ./graphplotter.bin

# source files path
SRCPATH = ./

# C flags (all warnings -Wall, optimization level -O3, additional include paths -I)
CFLAGS = -Wall -I./$(SRCPATH)include -I./$(SRCPATH)library

# libraries (math library -lm, )
LDFLAGS = -lm

# Allegro library
ALLEGRO = `allegro-config --cflags --libs`

# source files
SRC_FILES = $(SRCPATH)main.c $(SRCPATH)graphplotter/graphplotter.c $(SRCPATH)graphplotter/camera.c $(SRCPATH)graphplotter/object3D.c $(SRCPATH)graphplotter/graph.c $(SRCPATH)graphplotter/parsedsignature.c $(SRCPATH)graphplotter/maincomponent.c $(SRCPATH)graphplotter/component.c $(SRCPATH)graphplotter/expression.c $(SRCPATH)terminal/terminal.c $(SRCPATH)mcursor/mcursor.c $(SRCPATH)library/my_stdio.c $(SRCPATH)library/my_string.c $(SRCPATH)library/my_math.c $(SRCPATH)hardware/hardware.c $(SRCPATH)hardware/screen.c $(SRCPATH)hardware/mouse.c $(SRCPATH)hardware/keyboard.c

# Build software
compile_source_code:
	$(CC) $(CFLAGS) -g $(SRC_FILES) $(LDFLAGS) $(ALLEGRO) -o $(PROGRAM)

# remove all created files
clean:
	rm -f $(PROGRAM)

# run software
run:
	$(PROGRAM)

