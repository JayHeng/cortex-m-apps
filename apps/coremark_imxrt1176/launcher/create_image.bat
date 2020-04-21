rem Pass in full path to iar project directory
cd %1
python ..\..\..\..\launcher\bin2hex.py coremark_loader.bin ..\..\..\..\launcher\cm4_loader.h
