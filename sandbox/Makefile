all:
	../build.py sandbox

run: all
	cd build && ./bin/sandbox

clean:
	rm -r build
	make -C ../engine -f ../engine/Makefile clean
