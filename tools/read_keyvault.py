#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import struct
from binascii import hexlify, unhexlify

def parse_keytypes (kt):
    keylengths = []
    isCrypted = []
    for i in range (4):
        if (0x80 >> i) & kt:
            keylengths.append (16)
        else:
            keylengths.append(8)
        if (0x08 >> i) & kt:
            isCrypted.append (True)
        else:
            isCrypted.append (False)
    return (keylengths, isCrypted)

def main ():
    fmt = "B16s16s16s16sB"
    with open (sys.argv[1], 'rb') as f:
        data = f.read ()
        version, k_m1, k_w1, k_m2, k, kt = struct.unpack_from (fmt, data)
        ls, isCrypted = parse_keytypes (kt)

        print 8 * '-' + ' Keyvault (ver.: 0x%02x)' % version + 12 * '-'
        print '| K_M1:\t%032s |' % hexlify (k_m1[:ls[0]])
        print '| K_W1:\t%032s |' % hexlify (k_w1[:ls[1]])
        print '| K_M2:\t%032s |' % hexlify (k_m2[:ls[2]])
        print '| K:\t%32s |' % hexlify (k[:ls[3]])
        print 42 * '-'

if __name__ == '__main__':
    try:
        main ()
    except KeyboardInterrupt:
        pass
