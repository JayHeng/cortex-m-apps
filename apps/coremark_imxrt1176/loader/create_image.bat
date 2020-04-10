rem Pass in full path to iar project directory
cd %1
python ..\..\..\..\loader\bin2hex.py coremark.bin ..\..\..\..\loader\cm7_coremark.h
