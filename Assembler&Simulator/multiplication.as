lw 1 0 mcand
lw 2 0 mplier
lw 3 0 one
lw 6 0 sixt
lw 5 0 negone
start    nand 4 3 2 
	beq 4 5 skip
	add 7 7 1
skip add 1 1 1
     add 3 3 3 
     beq 3 6 done
     beq 0 0 start
done halt 
mcand .fill 29562
mplier .fill 11834
sixt .fill 32768
one .fill 1
negone .fill -1
