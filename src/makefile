# For 100 checksum of C is 265617
# For 1000 checksum of C is 
N = 10000  


all : MDxMD-M_alpha


install :
	cd ../lib && tar xzf ../lib/simplesim-3.0_acx.tgz
	cd ../lib/simplesim-3.0_acx && make config-alpha && make
	cd ../lib && tar xzf ../lib/gcc-cross-alpha_AC.tgz
	cd ../lib/gcc-cross-alpha && echo "export GCC_CROSS_ALPHA=$$(pwd)" >> ~/.bashrc
	
uninstall :
	rm -rf ../lib/simplesim-3.0_acx
	rm -rf ../lib/gcc-cross-alpha
	
	
normal : MDxMD-M.c
	gcc -O3 -Wall MDxMD-M.c -o MDxMD-M
	
refactored : MDxMD-M_refactored.c
	gcc -O3 -Wall MDxMD-M_refactored.c -o MDxMD-M_refactored

MDxMD-M_alpha : MDxMD-M_refactored.c
	cd ../lib/gcc-cross-alpha/local/bin && ./alpha-dec-osf4-gcc -O ../../../../src/MDxMD-M_refactored.c -o ../../../../src/MDxMD-M_alpha


run: MDxMD-M
	time ./MDxMD-M $(N)
	
runRefactored: MDxMD-M_refactored
	time ./MDxMD-M_refactored $(N)	

runalpha : MDxMD-M_alpha
	../lib/simplesim-3.0_acx/sim-outorder MDxMD-M_alpha $(N)


.PHONY : clean
clean :
	rm -f MDxMD-M
	rm -f MDxMD-M_refactored
	rm -f MDxMD-M_alpha
	rm -f stdout.txt
	rm -f stderr.txt
	

