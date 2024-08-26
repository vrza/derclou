RM ?= rm -f

native:
	$(MAKE) -f makefile.gcc

win-x64:
	$(MAKE) -f makefile.mingw

clean:
	$(RM) -r ./target

.PHONY : native gcc win-x64 clean
