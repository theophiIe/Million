run: compile
	./scenario-test.sh

compile:
	gcc -Wall million.c -L. -liof -lm -g -o million
	
clean:
	rm million

LENOM=MOLINATTI_Theophile
zip:
	rm -rf ${LENOM}
	rm -rf ${LENOM}.zip
	mkdir ${LENOM}
	cp Makefile ${LENOM}
	cp *.h *.c *.sh *.cfg ${LENOM}
	cp README.md ${LENOM}
	zip -r ${LENOM}.zip ${LENOM}
	rm -rf ${LENOM}
