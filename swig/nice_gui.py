#!/usr/bin/env python2
import argparse
import os
import sys
from sandwich import Log
from sandwich import swig_shop

try:
    from gi.repository import Gtk, Gdk, GLib
except:
    try:
        import gtk as Gtk
    except:
        print("You need gtk")

class SandwichesWindow(Gtk.Window):

    def __init__(self, global_private, shop_private, keydir, shop_name):
        interface = Gtk.Builder()
        interface.add_from_file('interface.glade')
        
        self.label = interface.get_object("info_label")
        self.progress = interface.get_object("progressbar")
        self.global_private = global_private
        self.shop_private = shop_private
        self.keydir = keydir
        self.shop_name = shop_name
        self.progress.set_text("")

        interface.connect_signals(self)
        
        self.sand_lab = []
        for i in range(10):
            l = "shop%u_label" % (i+1)
            lab = interface.get_object(l)
            self.sand_lab.append(lab)
            
        self.sand_date = []
        for i in range(10):
            l = "date%u_label" % (i+1)
            lab = interface.get_object(l)
            self.sand_date.append(lab)

        self.sand_signatures = []
        for i in range(10):
            l = "sig%u_label" % (i+1)
            lab = interface.get_object(l)
            self.sand_signatures.append(lab)

        
    def on_mainWindow_destroy(self, widget):
        Gtk.main_quit()

    def readCard(self, widget):
        self.resetProgressBar()
        self.progress.set_text("Reading...")
        logs = Log.get_entries(self.global_private, self.keydir, self.incrementProgressBar)
        self.updateInfoText("Reading the card ...")
        for i in logs:
            self.updateSandwichText (i.num, i.shop_name)
            self.updateSandwichDate (i.num, '%02u/%02u/%02u %02u:%02u' % (i.month, i.day, i.year, i.hour, i.minute))
            self.updateSandwichSignature (i.num, i.sigok_str)
        counter = swig_shop.read_counter_python(self.global_private, self.shop_private)

        self.updateInfoText(str(counter) + " sandwiches on the card!")
        self.resetProgressBar()

    def addSandwich(self, widget):
        sandwich.buy_python (self.global_private, self.shop_private, self.shop_name)
        self.updateInfoText("Ok, sandwich added")

    def incrementProgressBarButton(self,widget):
        self.incrementProgressBar()
        
        
    def updateInfoText(self, txt):
        self.label.set_label(txt)

    def updateSandwichText(self, num, txt):
        self.sand_lab[num].set_label(txt)
        
    def updateSandwichDate(self, num, date):
        label = self.sand_date[num].set_label(date)

    def updateSandwichSignature(self, num, sig):
        label = self.sand_signatures[num].set_label(sig)
        
    def resetProgressBar(self):
        self.progress.set_fraction(0.0)
        while Gtk.events_pending():
            Gtk.main_iteration()
        self.progress.set_text("")

    def incrementProgressBar(self):
        current = self.progress.get_fraction()
        self.progress.set_fraction(current + 0.1)
       
        while Gtk.events_pending():
            Gtk.main_iteration()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Shop's GUI")
    parser.add_argument ('--keydir', type=str, default='../keys')
    parser.add_argument ('--global-private-key', type=str, default='../keys/global_private.pem')
    parser.add_argument ('--shop-private-key', type=str, default='../keys/shop_private.pem')
    parser.add_argument ('--shop-name', type=str, default='group_1')
    args = parser.parse_args()

    if not os.path.exists(os.path.join(args.keydir,'public_keys')):
        print ('[Warn]  Public key dir was not found at %s, did you extract them there? Signatures cannot be verified like this!'
                % os.path.join(args.keydir, 'public_keys'))
    for i in [args.global_private_key, args.shop_private_key]:
        if not os.path.exists(i):
            print ("[Error] Could not find %s" % i)
            sys.exit(1)

    win = SandwichesWindow(args.global_private_key, args.shop_private_key, args.keydir, args.shop_name)
    Gtk.main()

