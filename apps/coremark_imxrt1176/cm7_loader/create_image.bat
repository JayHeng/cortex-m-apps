rem Pass in full path to iar project directory
cd %1
python ..\..\..\..\cm7_loader\bin2hex.py coremark.bin ..\..\..\..\cm7_loader\cm7_coremark.h
