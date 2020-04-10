rem Pass in full path to iar project directory
cd %1
python ..\..\..\..\loader\bin2hex.py dhrystone.bin ..\..\..\..\loader\cm7_dhrystone.h
