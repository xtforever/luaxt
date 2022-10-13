PREFIX=/home/jens/git
LUA=$(PREFIX)/lua
XTCW=$(PREFIX)/xtcw2/build
XTCW_LIB=$(XTCW)/lib/libxtcw.a
XTCW_INC=$(XTCW)/include


SWIG=swig
CC=gcc

CFLAGS+=-I$(LUA) -I$(XTCW_INC) -I/usr/include/freetype2 -fms-extensions
XORGLIB=-lXaw -lX11 -lXt -lXpm -lXext -lXmu -lXft -lfontconfig -lXrender -lutil 
LOADLIBES+=$(LUA)/liblua.a $(XTCW_LIB) $(XORGLIB) -lm -ldl

CFLAGS+=-g -DMLS_DEBUG

ALL: luaxt-main c-if-lua

%_wrap.c: %.i
	swig -lua -o $@ $<

luaxt-main: luaxt_wrap.c luaxt.c var5.c m_tool.c lua-var5.c

luaxt_wrap.c: luaxt.c  luaxt.i

c-if-lua: var5.c m_tool.c

clean:
	-rm *_wrap.c *.o luaxt-main c-if-lua

