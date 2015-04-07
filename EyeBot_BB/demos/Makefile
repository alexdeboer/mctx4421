#Makefile to make all the demos
DIR = $(shell pwd)

all:
	cd $(DIR)/eyebotDemo; make
	cd $(DIR)/EyeBotIRTestDemo; make
	cd $(DIR)/helloWorld; make	
	cd $(DIR)/libM6au; make
	cd $(DIR)/libM6ip; make
	cd $(DIR)/libM6thread; make
	cd $(DIR)/libM6time; make

install:
	cd $(DIR)/eyebotDemo; make install
	cd $(DIR)/EyeBotIRTestDemo; make install
	cd $(DIR)/helloWorld; make install

clean:
	cd $(DIR)/eyebotDemo; make clean
	cd $(DIR)/EyeBotIRTestDemo; make clean
	cd $(DIR)/helloWorld; make clean
	cd $(DIR)/libM6ip; make clean
	cd $(DIR)/libM6thread; make clean
	cd $(DIR)/libM6time; make clean
	cd $(DIR)/libM6au; make clean
