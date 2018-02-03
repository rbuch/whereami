CHARMC?=../../../../bin/charmc $(OPTS)

OBJS = whereami.o

all: whereami

whereami: $(OBJS)
	$(CHARMC) -language charm++ -o whereami $(OBJS)

whereami.decl.h: whereami.ci
	$(CHARMC)  whereami.ci

clean:
	rm -f *.decl.h *.def.h conv-host *.o whereami charmrun

whereami.o: whereami.C whereami.decl.h
	$(CHARMC) -c whereami.C


