# Informacion
CC=g++
CFLAGS=-I./src/ASICamera/include/ -I/usr/include/opencv4/ 
LD=./src/ASICamera/lib/x64/
LDIR=-L$(LD)
LIBS=-lASICamera2 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio 

# Nombre del ejecutable
TARGET = bin/live_camera

# Compilar el programa
$(TARGET): src/live_camera.cpp
	$(CC) $< -o $@ $(CFLAGS) $(LDIR) $(LIBS)

# Limpiar el ejecutable
clean:
	rm -f $(TARGET)

# Ejecución del programa
run: $(TARGET)
	LD_LIBRARY_PATH=$(LD) ./$(TARGET)
