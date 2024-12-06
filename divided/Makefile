OBJDIR := obj

OBJS := $(OBJDIR)/main.o $(OBJDIR)/table.o $(OBJDIR)/etcFunc.o $(OBJDIR)/7_2l7_5l7_6l7_7.o \
        $(OBJDIR)/7_3.o $(OBJDIR)/7_4.o $(OBJDIR)/7_8l7_9.o $(OBJDIR)/7_10l7_13.o \
        $(OBJDIR)/7_12.o $(OBJDIR)/orderItem.o $(OBJDIR)/paymentUnit.o \
        $(OBJDIR)/tableMove.o $(OBJDIR)/paymentCancel.o

all: pos

pos: $(OBJS)
	gcc -o pos $(OBJS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	gcc -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f pos
	rmdir $(OBJDIR)