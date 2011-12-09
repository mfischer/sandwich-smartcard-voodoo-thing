#!/usr/bin/python
try:
    from gi.repository import Gtk
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
        for i in range(0,10):
            l = "label"+str((i+1)*3)
            lab = interface.get_object(l)            
            self.sand_lab.append(lab)
            
        self.sand_date = []
        for i in range(0,10):
            l = "label"+str(1+(i+1)*3)
            lab = interface.get_object(l)            
            self.sand_date.append(lab)

        
    def on_mainWindow_destroy(self, widget):
        Gtk.main_quit()

    def readCard(self, widget):
        self.updateInfoText("There is lot's of things on this card!")
        print("I read the card")

    def addSandwich(self, widget):
        self.updateInfoText("Ok sandwich added")
        print("Bon appetit")
        
    def updateInfoText(self, txt):
        self.label.set_label(txt)

    def updateSandwichText(self, num, txt):
        label = self.sand_lab[num]
        label.set_label(txt)
        
    def updateSandwichDate(self, num, date):
        label = self.sand_date[num]
        label.set_label(date)


win = SandwichesWindow()
win.updateSandwichText(0,"Chez Vincent")
win.updateSandwichText(4,"Delifrance")
Gtk.main()

