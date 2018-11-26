main: main.o controller.o sniffer.o
	g++ -std=c++11 objects/main.o objects/controller.o objects/sniffer.o -o ../myripsniffer -lpcap

main.o: main.cpp
	mkdir objects
	g++ -std=c++11 -o objects/main.o -c main.cpp

controller.o: controller.cpp controller.h
	g++ -std=c++11 -o objects/controller.o -c controller.cpp

sniffer.o: sniffer.cpp sniffer.h
	g++ -std=c++11 -o objects/sniffer.o -c sniffer.cpp

clean:
	rm -r objects
	rm ../myripsniffer
