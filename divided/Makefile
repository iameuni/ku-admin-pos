OBJDIR := obj

OBJS := $(OBJDIR)/main.o $(OBJDIR)/table.o $(OBJDIR)/etcFunc.o $(OBJDIR)/7_2l7_5l7_6l7_7.o \
        $(OBJDIR)/7_3.o $(OBJDIR)/7_4.o $(OBJDIR)/7_8l7_9.o $(OBJDIR)/7_10l7_13.o \
        $(OBJDIR)/7_12.o $(OBJDIR)/orderItem.o $(OBJDIR)/paymentUnit.o \
        $(OBJDIR)/tableMove.o $(OBJDIR)/paymentCancel.o

# 운영 체제 감지
ifeq ($(OS),Windows_NT)
    RM := del /Q
    RM_DIR := rmdir /Q /S
    MKDIR := mkdir
    TARGET := pos.exe
else
    RM := rm -f
    RM_DIR := rm -rf
    MKDIR := mkdir -p
    TARGET := pos
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	gcc -c $< -o $@

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	$(RM) $(subst /,\,$(OBJDIR)/*.o)
	$(RM) $(TARGET)
	$(RM_DIR) $(OBJDIR)