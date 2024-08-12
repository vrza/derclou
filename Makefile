.POSIX:
RM ?= rm

native:
	$(MAKE) -C src -f makefile.gcc

win-x64:
	$(MAKE) -C src -f makefile.mingw

clean:
	$(RM) -rf ./target

.PHONY : native gcc win-x64 clean
