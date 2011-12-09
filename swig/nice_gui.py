#!/usr/bin/env python2
import Log
import swig_shop

try:
    from gi.repository import Gtk, Gdk, GLib
except:
    try:
        import gtk as Gtk
    except:
        print("You need gtk")

class SandwichesWindow(Gtk.Window):

    def __init__(self):
        interface = Gtk.Builder()
        interface.add_from_file('interface.glade')
        
        self.label = interface.get_object("info_label")
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
        logs = Log.get_entries('../keys/global_private.pem', '../keys')
        self.updateInfoText("Reading the card ...")
        for i in logs:
            self.updateSandwichText (i.num, i.shop_name)
            self.updateSandwichDate (i.num, '%02u/%02u/%02u %02u:%02u' % (i.month, i.day, i.year, i.hour, i.minute))
            self.updateSandwichSignature (i.num, i.sigok_str)
        self.updateInfoText("Done reading the card")

    def addSandwich(self, widget):
        swig_shop.buy_python ('../keys/global_private.pem', '../keys/shop_private.pem', 'group_1')
        self.updateInfoText("Ok, sandwich added")
        
    def updateInfoText(self, txt):
        self.label.set_label(txt)

    def updateSandwichText(self, num, txt):
        self.sand_lab[num].set_label(txt)
        
    def updateSandwichDate(self, num, date):
        label = self.sand_date[num].set_label(date)

    def updateSandwichSignature(self, num, sig):
        label = self.sand_signatures[num].set_label(sig)

if __name__ == '__main__':
    win = SandwichesWindow()
    GLib.threads_init()
    Gdk.threads_init()
    Gtk.main()

