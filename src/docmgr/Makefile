LIB=../libsrc/libdocmgr.a
PROG=docmgr
CXXFLAGS=-g -I../libsrc
LDFLAGS=-L../libsrc
LIBS=-ldocmgr -lsqlite3 -lncurses

SRCS=docmgr.cpp \
     Widget.cpp \
     InteractorList.cpp \
     IDList.cpp \
     Menu.cpp \
     TopLine.cpp \
     ArticleForm.cpp \
     ArticleViewForm.cpp \
     ArticleEditForm.cpp \
     ArticleTypeDialogue.cpp \
     QuickSearchDialogue.cpp \
     OptionsDialogue.cpp \
     ExportDialogue.cpp \
     ImportDialogue.cpp \
     ConfirmDialogue.cpp \
     EditField.cpp \
     TextField.cpp \
     MultiLineTextField.cpp \
     SpinField.cpp \
     Configuration.cpp \
     Importer.cpp \
     Filter.cpp \
     FilterList.cpp \
     FilterCombinationDialogue.cpp \
     JumpToDocDialogue.cpp

OBJS=$(addprefix obj/,$(SRCS:.cpp=.o))

all: obj $(PROG)

obj:
	if [ ! -d obj ]; then mkdir obj ; fi

docmgr: $(OBJS)
	$(CXX) -g $(LDFLAGS) -o $@ $^ $(LIBS)

depend:
	makedepend -Y -pobj/ -- $(CXXFLAGS) -- $(SRCS) 2> /dev/null

clean:
	rm -f docmgr $(OBJS)

obj/%.o: %.cpp
	$(COMPILE.cpp) -o $@ $<


# DO NOT DELETE THIS LINE -- make depend depends on it.

obj/docmgr.o: ../libsrc/DocMgr.hh InteractorList.hh Widget.hh Menu.hh
obj/docmgr.o: TopLine.hh IDList.hh ArticleViewForm.hh ArticleForm.hh
obj/docmgr.o: ArticleEditForm.hh TextField.hh EditField.hh SpinField.hh
obj/docmgr.o: QuickSearchDialogue.hh ArticleTypeDialogue.hh
obj/docmgr.o: OptionsDialogue.hh Configuration.hh ImportDialogue.hh
obj/docmgr.o: ConfirmDialogue.hh Filter.hh FilterList.hh JumpToDocDialogue.hh
obj/Widget.o: Widget.hh
obj/InteractorList.o: InteractorList.hh Widget.hh
obj/IDList.o: IDList.hh ../libsrc/DocMgr.hh Widget.hh ArticleViewForm.hh
obj/IDList.o: ArticleForm.hh InteractorList.hh
obj/Menu.o: Menu.hh Widget.hh InteractorList.hh
obj/TopLine.o: TopLine.hh Widget.hh
obj/ArticleForm.o: ArticleForm.hh ../libsrc/DocMgr.hh Widget.hh
obj/ArticleViewForm.o: ArticleViewForm.hh ArticleForm.hh ../libsrc/DocMgr.hh
obj/ArticleViewForm.o: Widget.hh InteractorList.hh
obj/ArticleEditForm.o: ArticleEditForm.hh ArticleForm.hh ../libsrc/DocMgr.hh
obj/ArticleEditForm.o: Widget.hh TextField.hh EditField.hh SpinField.hh
obj/ArticleEditForm.o: InteractorList.hh Menu.hh MultiLineTextField.hh
obj/ArticleTypeDialogue.o: ArticleTypeDialogue.hh ../libsrc/DocMgr.hh
obj/ArticleTypeDialogue.o: Widget.hh InteractorList.hh
obj/QuickSearchDialogue.o: QuickSearchDialogue.hh Widget.hh InteractorList.hh
obj/QuickSearchDialogue.o: TextField.hh EditField.hh
obj/OptionsDialogue.o: OptionsDialogue.hh Configuration.hh Widget.hh
obj/OptionsDialogue.o: TextField.hh EditField.hh SpinField.hh
obj/OptionsDialogue.o: InteractorList.hh
obj/ImportDialogue.o: ImportDialogue.hh ../libsrc/DocMgr.hh Widget.hh
obj/ImportDialogue.o: TextField.hh EditField.hh InteractorList.hh Importer.hh
obj/ConfirmDialogue.o: ConfirmDialogue.hh Widget.hh InteractorList.hh
obj/ConfirmDialogue.o: TextField.hh EditField.hh SpinField.hh
obj/EditField.o: EditField.hh Widget.hh
obj/TextField.o: TextField.hh EditField.hh Widget.hh InteractorList.hh
obj/MultiLineTextField.o: MultiLineTextField.hh EditField.hh Widget.hh
obj/MultiLineTextField.o: InteractorList.hh ArticleEditForm.hh ArticleForm.hh
obj/MultiLineTextField.o: ../libsrc/DocMgr.hh TextField.hh SpinField.hh
obj/SpinField.o: SpinField.hh EditField.hh Widget.hh InteractorList.hh
obj/Configuration.o: ../libsrc/DocMgr.hh Configuration.hh
obj/Importer.o: Importer.hh ../libsrc/DocMgr.hh
obj/Filter.o: Filter.hh ../libsrc/DocMgr.hh
obj/FilterList.o: FilterList.hh Widget.hh Filter.hh ../libsrc/DocMgr.hh
obj/FilterList.o: Configuration.hh Menu.hh TextField.hh EditField.hh
obj/FilterList.o: InteractorList.hh FilterCombinationDialogue.hh SpinField.hh
obj/FilterCombinationDialogue.o: FilterCombinationDialogue.hh Widget.hh
obj/FilterCombinationDialogue.o: TextField.hh EditField.hh SpinField.hh
obj/FilterCombinationDialogue.o: InteractorList.hh
obj/JumpToDocDialogue.o: JumpToDocDialogue.hh Widget.hh IDList.hh
obj/JumpToDocDialogue.o: ../libsrc/DocMgr.hh TextField.hh EditField.hh
obj/JumpToDocDialogue.o: InteractorList.hh
