#!/bin/bash
#rm -rf result*
g++ neverlost_util.cpp  rabin_asm.S rabin.cpp main1.cpp -std=c++0x -o fragmentation1
g++ neverlost_util.cpp  rabin_asm.S rabin.cpp main2.cpp -std=c++0x -o fragmentation2
g++ neverlost_util.cpp  rabin_asm.S rabin.cpp main3.cpp -std=c++0x -o fragmentation3
python runjob_qinghao.py
