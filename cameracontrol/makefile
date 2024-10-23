CC=g++
CFLAGS=-I./ASI_SDK_V1.36/include/ -I/usr/include/opencv2/  
LD=./ASI_SDK_V1.36/lib/x64/
LDIR=-L$(LD)
LIBS=-lASICamera2 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio 

live_camera: live_camera.cpp
	$(CC) $< -o $@ $(CFLAGS)  $(LDIR) $(LIBS)

clean:
	rm -f live_camera

run:
	LD_LIBRARY_PATH=${LD} ./live_camera
