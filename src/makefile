
all : MDxMD-M_alpha

install :
	cd ../lib && tar xzf ../lib/simplesim-3.0_acx.tgz
	cd ../lib/simplesim-3.0_acx && make config-alpha && make
	cd ../lib && tar xzf ../lib/gcc-cross-alpha_AC.tgz
	cd ../lib/gcc-cross-alpha && echo "export GCC_CROSS_ALPHA=$$(pwd)" >> ~/.bashrc
	
uninstall :
	rm -rf ../lib/simplesim-3.0_acx
	rm -rf ../lib/gcc-cross-alpha
	
MDxMD-M : MDxMD-M_refactored.c
	gcc -O3 -Wall MDxMD-M_refactored.c -o MDxMD-M

run: MDxMD-M
	time ./MDxMD-M 100

MDxMD-M_alpha : MDxMD-M_refactored.c
	cd ../lib/gcc-cross-alpha/local/bin && ./alpha-dec-osf4-gcc -O ../../../../src/MDxMD-M_refactored.c -o ../../../../src/MDxMD-M_alpha

runalpha : MDxMD-M_alpha
	../lib/simplesim-3.0_acx/sim-outorder MDxMD-M_alpha 100

.PHONY : clean
clean :
	rm -f MDxMD-M
	rm -f MDxMD-M_alpha
	rm -f stdout.txt
	rm -f stderr.txt
	

