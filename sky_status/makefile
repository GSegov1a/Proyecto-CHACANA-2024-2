CXX = g++
CXXFLAGS = -I./include
LDFLAGS = -lcurl
SRC = src/main.cpp src/sky_status_api_request.cpp
TARGET = bin/sky_status

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f ./$(TARGET)

run: $(TARGET)
	./$(TARGET)

kill:
	@if [ -f ./logs/pid.log ]; then \
		id=$$(cat ./logs/pid.log); \
		echo "Terminando proceso con PID: $$id"; \
		kill $$id; \
	else \
		echo "El archivo pid.log no existe."; \
	fi