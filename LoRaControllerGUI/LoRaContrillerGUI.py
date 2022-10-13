from cgitb import text
from datetime import datetime
from email.header import Header
import os
import threading
import time
import tkinter as tk
from tkinter import Pack, ttk
from tkinter import messagebox
from tkinter.messagebox import NO
import serial
import serial.tools.list_ports


class MainWindows():
    
    def __init__(self):
        self.ResendNum = 0
        self.CurrentNum = 0
        self.errorNum = 0 
        self.i = 0 
         #獲取連接埠
        self.ports = self.GetUSBList()

        # 建立主視窗
        self.window = tk.Tk()
        self.window.title("LoRa Controller GUI")
        self.window.geometry("800x560")
        self.window.resizable(width=False,height=False)

        # 建立標題
        self.tittleFrame = tk.Frame(self.window,width=800,height=100)
        self.tittleFrame.pack()
        self.title = tk.Label(self.tittleFrame,text="LoRa Controller",font= ("Times New Roman",28))
        self.title.pack(anchor=tk.N)

        # 建立狀態列
        self.StatusFrame = tk.Frame(self.window,width=800,height=200)
        self.Text1 = tk.Label(self.StatusFrame,text="Serial Port :",font=("Times New Roman",20))
        self.Text1.pack(anchor=tk.N,side=tk.LEFT,padx= 10)
        self.SerialList = ttk.Combobox(self.StatusFrame,state="readonly",height=3,values=self.ports)
        self.SerialList.pack(anchor=tk.N,side=tk.LEFT,pady=10)
        self.SerialList.bind('<<ComboboxSelected>>',self.SetPort)
        self.StatusFrame.pack()

        # 建立提醒文字
        self.ContentText = tk.Label(self.window,text="Please Select One Port",font=("Times New Roman",20))
        self.ContentText.pack(anchor=tk.CENTER,side=tk.BOTTOM)

        # 定義視窗關閉事件
        self.window.protocol("WM_DELETE_WINDOW",self.CloseWindows)
        self.window.mainloop()

    def setupGUI(self):
        self.ContentText.configure(text="")
        # 建立接收封包視窗
        self.PacketFrame = tk.Frame(self.window,width=600,height=450,bg="lightgreen")
        self.PacketFrame.place(relx=0.02,rely=0.16)
        self.PacketWindow = ttk.Treeview(self.PacketFrame,columns= ("Time","Type","Packet"),height=20)
        self.PacketWindow.heading("#0",text="")
        self.PacketWindow.heading("Time",text="Time")
        self.PacketWindow.heading("Type",text="Type")
        self.PacketWindow.heading("Packet",text="Packet")
        self.PacketWindow.column("#0",width=0,stretch=NO)
        self.PacketWindow.column("Time",width=250,anchor="c")
        self.PacketWindow.column("Type",width=250,anchor="c")
        self.PacketWindow.column("Packet",width=250,anchor="c")
        self.PacketWindow.pack()
    
    def GetUSBList(self):
        ports = []
        portList = list(serial.tools.list_ports.comports())
        for port in portList:
            ports.append(port.name)
        return ports

    def SetPort(self,event):
        self.setupGUI()
        portName = self.SerialList.get()
        self.CurrentPort = serial.Serial(portName,timeout=3)
        t = threading.Thread(target=self.ParaeSerialBackData)
        t.start()
    
    # 加入TreeView
    def addTree(self,data):
        self.PacketWindow.insert('','end',values=data)
        
    def ParaeSerialBackData(self):
        # 從arduino 接收資料
        while True:
            self.nowTime = str(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
            respone = str(self.CurrentPort.readline())
            if respone != '' and respone != ' ' and respone != "b''":
                print(str(self.i) + '. ')
                print(respone)
                self.i += 1
                try:
                    respone_data = respone.split(':')[1].replace(r'\r\n','').replace("'",'') # Get Packet Content : 0x1 0x2 0x0 0x1 0x5 0x1 0x2 0x3 0x4 0x5 0x6 
                    respone_Type = respone.split(':')[0].replace(r'\r\n','').replace("'",'').replace('b','')                                
                    self.addTree([self.nowTime,respone_Type,respone_data])
                    self.SaveLogs(self.nowTime,respone_Type,respone_data)
                except:
                    pass
    
    # 存入檔案
    def SaveLogs(self,timestream,Type,rdata):
        with open("Logs.txt","a+",encoding="utf-8") as f :
            data = timestream + "           " + Type + "           " + rdata + "\n"
            f.write(data)

    # 關閉視窗
    def CloseWindows(self):
        os._exit(0)
        

if __name__ == "__main__":
    window = MainWindows()
    #window.setupGUI()