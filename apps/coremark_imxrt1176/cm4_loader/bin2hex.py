import os
import sys

def BinFile2HexFile(bin, hex):
    with open(bin, "rb") as fBin:
        data = fBin.read()
        length = len(data)
        fBin.close()

    with open(hex, "w") as fHex:
        fHex.write("#define APP_LEN {}\n\n".format(length))
        fHex.write("const uint8_t cm4_app_code[] = {\n")

        index = 0
        while index < length:
            line = '   '
            left = length - index
            if left > 16:
                for i in range(0, 16):
                    line = line + ' 0x{:0>2x},'.format(data[index])
                    index += 1
            else:
                for i in range(0, left):
                    line = line + ' 0x{:0>2x},'.format(data[index])
                    index += 1
            line = line + '\n'

            fHex.write(line)

        fHex.write("};")
        fHex.close()

        print('Conversion done!\n')

BinFile2HexFile(sys.argv[1], sys.argv[2])
