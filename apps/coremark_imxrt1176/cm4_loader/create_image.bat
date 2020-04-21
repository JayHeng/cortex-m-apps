rem Pass in full path to iar project directory
cd %1
python ..\..\..\..\cm4_loader\bin2hex.py coremark.bin ..\..\..\..\cm4_loader\cm4_coremark.h
