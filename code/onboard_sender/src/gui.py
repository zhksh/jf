import serial
import time
import tkinter
import sys


def quit():
    global tkTop
    ser.write(bytes('L', 'UTF-8'))
    tkTop.destroy()

def set_button1_state():
        global b
        b += 1
        varLabel.set("LED ON ")
        ser.write(bytes('H', 'UTF-8'))
        varLabel2.set(b)
        print(b)

def set_button2_state():
        varLabel.set("LED OFF")
        ser.write(bytes('L', 'UTF-8'))

# tcl = tkinter.Tcl()
# print(tcl.call("info", "patchlevel"))
#exit()
# ser = serial.Serial('/dev/cu.usbmodem144401', 9600)
# print("Reset Arduino")
# time.sleep(3)
# ser.write(bytes('L', 'UTF-8'))
import sys
from PyQt5.QtWidgets import (QApplication, QWidget,
QPushButton, QGridLayout, QLabel, QVBoxLayout)

from PyQt5.QtCore import QTimer

class SubUI(QWidget):
    update_interval = 200
    port = '/dev/cu.usbmodem144401'
    baud_rate = 9600

    sensor_names = ['tb', 'tmp0', 'tmp1', "aux"]

    def __init__(self):
        super().__init__()
        self.initUI()

    def connect(self):
        print("Connect to Arduino")
        self.cnx = serial.Serial(self.port, self.baud_rate)
        time.sleep(3)


    def setup_loop(self):
        self.qTimer = QTimer()
        self.qTimer.setInterval(self.update_interval) # 1000 ms = 1 s
        self.qTimer.timeout.connect(self.getSensorValues)
        self.qTimer.start()


    def initUI(self):   
        grid = QGridLayout()  
        self.setLayout(grid)
        self.connect()
        self.sensor_map = {}

        positions = [(i, j) for i in range(2) for j in range(2)]
        for position, name in zip(positions, self.sensor_names):
            val = QLabel("0")
            layout = QVBoxLayout()
            layout.addWidget(QLabel(name))
            layout.addWidget(val)
            grid.addLayout(layout, *position)
            self.sensor_map[name] = val

        self.move(400, 500)
        self.setWindowTitle('SUB Ui')  
        self.show()
        self.setup_loop()


    def getSensorValues(self):
        pl = self.cnx.readline().decode()
        print(pl)
        if (pl):
            for s, v in self.decode_(pl): self.sensor_map[s].setText(v)

    def decode_(self, bytestr):
        sv_pairs = bytestr.rstrip().split(";")
        data = {tuple(pair.split(":")) for pair in sv_pairs}
        
        return data


    def setVal(self, name, val):
        self.map[name].setText(val)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    subUI = SubUI()
    sys.exit(app.exec_())
    self.ser.close()
