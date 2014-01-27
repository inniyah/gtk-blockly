PROGRAM=gtk-blockly

all: $(PROGRAM)

DATA_DIR=$(shell pwd)

C_SOURCES = $(shell find . -name "*.c")
CPP_SOURCES = $(shell find . -name "*.cpp")
OBJS = $(C_SOURCES:.c=.o) $(CPP_SOURCES:.cpp=.o)

PKG_CONFIG=gtk+-3.0 webkitgtk-3.0 libxml-2.0
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG)`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG)`
DEFS=-DUSE_GETTEXT

CFLAGS=-Wall -g -O2
LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
EXTRA_CFLAGS=-I. -DDATA_DIR='"$(DATA_DIR)"'
EXTRA_LDFLAGS=
LIBS=

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $(EXTRA_LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(CFLAGS) $(EXTRA_CFLAGS) $(PKG_CONFIG_CFLAGS) $(DEFS)

%.o: %.c
	gcc -o $@ -c $+ $(CFLAGS) $(EXTRA_CFLAGS) $(PKG_CONFIG_CFLAGS) $(DEFS)

i18n/gtk-blockly.pot:
	xgettext -LJavaScript -kSys.gettext --from-code=utf8  -o i18n/gettext_messages.pot blockly/language/gettext_messages.js
	msgcat -o $< i18n/gettext_messages.pot

gettext: i18n/gtk-blockly.pot
	cd i18n; for F in *.po; do echo "Converting $$F"; msgfmt "$$F" -o `basename "$$F" .po`.mo; done

clean:
	rm -fv $(OBJS)
	rm -fv $(PROGRAM)
	rm -fv *.o *.a *~
	rm -fv i18n/*.pot i18n/*.mo

