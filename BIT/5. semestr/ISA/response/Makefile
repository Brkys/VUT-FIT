main: main.o controller.o smuggler.o
	g++ -std=c++11 objects/main.o objects/controller.o objects/smuggler.o -o ../myripresponse

main.o: main.cpp
	mkdir objects
	g++ -std=c++11 -o objects/main.o -c main.cpp

controller.o: controller.cpp controller.h
	g++ -std=c++11 -o objects/controller.o -c controller.cpp

smuggler.o: smuggler.cpp smuggler.h
	g++ -std=c++11 -o objects/smuggler.o -c smuggler.cpp

clean:
	rm -r objects
	rm ../myripresponse
