CC=python3

all: run

pack:
	cd .. && zip -r xbrhel04_xlukac10_xermak00_xletov00.zip * .git

clean:
	rm -r __pycache__ doxyoutput ../xbrhel04_xlukac10_xermak00_xletov00.zip

test:
	$(CC) -m unittest tests.py

doc:
	doxygen Doxyfile	

run:
	$(CC) Ui.py
