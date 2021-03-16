import common, argparse, os, partition
from common import *
from config import Config
import tkinter as tk
from tkinter import ttk

class Editor:
    def __init__(self, root):
        root.title('Sming Hardware Profile Editor')
        self.main = ttk.Frame(root)
        self.main.grid()
        self.main.columnconfigure(0, weight=1)
        self.main.columnconfigure(1, weight=3)
        self.createWidgets()

    def createWidgets(self):
        quitButton = tk.Button(self.main, text='Quit', command=self.main.quit)
        quitButton.grid(row=100, sticky=tk.EW, columnspan=2)

    def selectDevice(self, devname):
        device = self.config.devices[devname]
        self.devices.set(device.name)

        if hasattr(self, 'partitions'):
            self.partitions.destroy()
        self.partitions = ttk.Frame(self.main)
        self.partitions.grid(row = 1)


        i = 1

        # Partition map (scrollable region)

        for p in self.config.map():
            if p.device != device:
                continue
            addr = tk.Label(self.partitions, text = p.address_str())
            addr.grid(row = i, column = 0, sticky=tk.W)
            label = tk.Label(self.partitions, text = p.name)
            label.grid(row = i, column = 1, sticky=tk.W, pady=5)
            # notes = tk.Label(self.main, text = "abc")
            # notes.grid(row = i, column = 2, sticky=tk.W)
            i += 1


    def setConfig(self, config):
        self.config = config

        # Combo box with devices

        self.devices = ttk.Combobox(self.main, values = [dev.name for dev in config.devices])
        self.devices.grid(row = 0)
        btn = tk.Button(self.main, text='...', command = lambda: self.selectDevice(self.devices.get()))
        btn.grid(row = 0, column = 2, sticky=tk.W)

        # Option checkboxes

        i = 2
        for k, v in self.config.option_library.items():
            btn = tk.Checkbutton(self.main, text = k + ': ' + v['description'])
            btn.grid(row = i, sticky=tk.W, columnspan=2)
            i += 1

        self.selectDevice(config.devices[0].name)


def main():
    parser = argparse.ArgumentParser(description='Sming hardware profile editor')
    parser.add_argument('input', help='Name of existing hardware profile')
    args = parser.parse_args()
    config = Config.from_name(args.input)

    root = tk.Tk()
    Editor(root).setConfig(config)
    root.mainloop()


if __name__ == '__main__':
    try:
        print("TCL version %s" % tk.TclVersion)
        main()
    except InputError as e:
        print(e, file=sys.stderr)
        sys.exit(2)
