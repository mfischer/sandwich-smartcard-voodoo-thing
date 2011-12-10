#!/usr/bin/env python2
from sandwich import Log

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
        self.progress = interface.get_object("progressbar")
        self.progress.set_show_text("")


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
        self.progress.set_show_text("Reading...")
        logs = Log.get_entries('../keys/global_private.pem', '../keys', self.incrementProgressBar)
        self.updateInfoText("Reading the card ...")
        for i in logs:
            self.updateSandwichText (i.num, i.shop_name)
            self.updateSandwichDate (i.num, '%02u/%02u/%02u %02u:%02u' % (i.month, i.day, i.year, i.hour, i.minute))
            self.updateSandwichSignature (i.num, i.sigok_str)
            
        self.updateInfoText("Done reading the card")
        self.resetProgressBar()

    def addSandwich(self, widget):
        sandwich.buy_python ('../keys/global_private.pem', '../keys/shop_private.pem', 'group_1')
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
        self.progress.set_show_text("")

    def incrementProgressBar(self):
        # current = self.progress.get_fraction()
        # print("Fraction step %d" % current)
        #self.progress.pulse()
        current = self.progress.get_fraction()
        self.progress.set_fraction(current + 0.1)
       
        while Gtk.events_pending():
            Gtk.main_iteration()

if __name__ == '__main__':
    win = SandwichesWindow()
    Gtk.main()

