.PHONY:	all SDL TestApp clean

all: SDL TestApp

SwDL:
	$(MAKE) -C SDL

TestApp: SwDL
	$(MAKE) -C TestApp

clean:
	$(MAKE) -C SDL clean
	$(MAKE) -C TestApp clean
