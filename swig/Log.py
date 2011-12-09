#!/usr/bin/env python2

import swig_shop


class LogEntry:
    def __init__(self, entry, sigok, num):
        self.year = int (entry[0:2]) + 2000
        self.month  = int (entry[2:4])
        self.day  = int (entry[4:6])
        self.hour = int (entry[6:8])
        self.minute  = int (entry[8:10])
        self.count = int (entry[10:14])
        self.shop_name = entry[14:]
        self.sigok = True if (sigok == 1) else False
        self.sigok_str = 'Valid' if sigok else 'Invalid'
        self.num = num

    def __str__(self):
        return "%02u/%u/%u - %02u:%02u - Shop '%s' - Count %u - Signature: %s" % (self.day,
                                                                                  self.month, self.year,
                                                                                  self.hour, self.minute,
                                                                                  self.shop_name, self.count,
                                                                                  self.sigok_str)

def get_entries (pkey_filename, keydir):
    ret = list()
    for i in range (10):
        print 'Reading #%u' % i
        entry, sigok, count = swig_shop.read_log_python (pkey_filename, keydir, i)
        ret.append(LogEntry(entry, sigok, i))
    return ret

def main ():
    log = get_entries ('../keys/global_private.pem', '../keys')
    for e in log:
        print e


if __name__ == '__main__':
    try:
        main ()
    except KeyboardInterrupt:
        pass
