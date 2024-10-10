CC=g++
CFLAGS=-I../include/ -I/usr/include/opencv4/  
LD=../lib/x64/
LDIR=-L$(LD)
LIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio

test: test.cpp
	$(CC) $< -o $@ $(CFLAGS) $(LIBS) $(LDIR)

clean:
	rm -f test

run:
	LD_LIBRARY_PATH=${LD} ./test
