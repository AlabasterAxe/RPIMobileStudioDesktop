lib_LTLIBRARIES = librpims.la
lib_LIBRARIES = librpims.a
bin_PROGRAMS = test objtest

librpims_la_CFLAGS = ${GL_CFLAGS}
librpims_la_CPPFLAGS = ${GL_CPPFLAGS} -Wall -fPIC -I${srcdir} -I${srcdir}/RED2 -I${srcdir}/../lib/libusb-0.1.12
librpims_la_LDFLAGS = ${GL_LDFLAGS}
librpims_la_SOURCES = IOBoard.c IOBoard_libusb.c AnalogStream.c FuncGen.c DigitalStream.c DigitalIO.c RED2/RED2.c RED2/RED2_AnalogStream.c RED2/RED2_FuncGen.c RED2/RED2_DigitalStream.c RED2/RED2_DigitalIO.c
librpims_la_SOURCES += ObjTable.c

librpims_a_CFLAGS = ${GL_CFLAGS}
librpims_a_CPPFLAGS = ${GL_CPPFLAGS} -Wall -fPIC -I${srcdir} -I${srcdir}/RED2 -I${srcdir}/../lib/libusb-0.1.12
librpims_a_SOURCES = IOBoard.c AnalogStream.c FuncGen.c DigitalStream.c DigitalIO.c RED2/RED2.c RED2/RED2_AnalogStream.c RED2/RED2_FuncGen.c RED2/RED2_DigitalStream.c RED2/RED2_DigitalIO.c
librpims_a_SOURCES += ObjTable.c

test_CFLAGS = ${GL_CFLAGS}
test_CPPFLAGS = ${GL_CPPFLAGS} -Wall -I${srcdir} -I${srcdir}/RED2 -I${srcdir}/../lib/libusb-0.1.12
test_LDFLAGS = ${GL_LDFLAGS} -L${srcdir}/../lib/libusb-0.1.12 -lrpims -lusb
#test_LDADD = ${top_builddir}/lib/test/libtest.la
test_SOURCES = test.c

objtest_CFLAGS = ${GL_CFLAGS}
objtest_CPPFLAGS = ${GL_CPPFLAGS} -Wall -I${srcdir} -I${srcdir}/RED2 -I${srcdir}/../lib/libusb-0.1.12
objtest_LDFLAGS = ${GL_LDFLAGS} -L${srcdir}/../lib/libusb-0.1.12 -lrpims -lusb
#objtest_LDADD = ${top_builddir}/lib/test/libtest.la
objtest_SOURCES = objtest.c
