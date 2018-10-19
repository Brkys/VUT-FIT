BINPATH = src/bin
.ONESHELL:

all: $(BINPATH)

$(BINPATH):
	mkdir $(BINPATH)
	cd $(BINPATH)
	qmake ../..
	make

clean:
	rm -r doc
	rm -r $(BINPATH)

doxygen:
	doxygen

run: $(BINPATH)
	$(BINPATH)/blockeditor

zip:
	zip -r xbrhel04_xborko02.zip README.txt icp.pro Makefile Doxyfile examples src/Conf* src/gui src/Model src/CMakeLists.txt src/main.cpp
