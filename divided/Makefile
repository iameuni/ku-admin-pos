all: pos
	
pos: pos.o table.o etcFunc.o 7_3.o 7_4.o orderItem.o paymentUnit.o
	gcc -o pos pos.o table.o etcFunc.o 7_3.o 7_4.o orderItem.o paymentUnit.o
pos.o: pos.c
	gcc -c pos.c
table.o: table.c
	gcc -c table.c
etcFunc.o: etcFunc.c
	gcc -c etcFunc.c
7_3.o: 7_3.c
	gcc -c 7_3.c
7_4.o: 7_4.c
	gcc -c 7_4.c
orderItem.o: orderItem.c
	gcc -c orderItem.c
paymentUnit.o: paymentUnit.c
	gcc -c paymentUnit.c
clean:
	rm -f *.o
	rm -f pos