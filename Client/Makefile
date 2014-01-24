all: build_deamon build_gui

build_deamon:
	cd ./deamon; make build

deamon:
	cd ./deamon; make

build_gui:
	cd ./gui; make build

gui:
	cd ./gui; make
	
clean_deamon:
	cd ./deamon; make clean
	
clean_gui:
	cd ./gui; make clean
	
clean: clean_deamon clean_gui
	rm -f $(wildcard *.o) $(wildcard *.d)