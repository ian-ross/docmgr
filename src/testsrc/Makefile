LIB=../libsrc/libdocmgr.a
TEST_PROGS=test-small-classes test-connection test-query \
           test-doc-record-1 test-doc-record-2 test-doc-record-3

all: $(TEST_PROGS)

%: %.cpp $(LIB)
	$(CXX) -g -I../libsrc -L../libsrc -o $@ $^ -ldocmgr -lpq
