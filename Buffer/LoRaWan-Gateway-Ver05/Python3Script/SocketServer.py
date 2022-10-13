import socket
import threading
import requests
from requests import put
import paho.mqtt.client as mqtt

host ="0.0.0.0"
port = 8080

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
s.bind((host,port))
s.listen(5)
n = 0

def getData():
    while (True):
        try:
            data = conn.recv(1024)
            mode = getDefalutServer()
            oldData = getOldPacketSum()
            G = oldData[1]
            S = oldData[2]
            R = oldData[3]
            # for d in data:
            #    print(str(hex(d)),end=",")
            # print('')

            if(mode == "TS"):
                if data != None and data != "" and len(data) != 0:
                    # 紀錄封包
                    PacketLog(data)
                    G += 1
                    num, response = SearchSensorNode(str(hex(data[0])))
                    if(num > 0):
                        try:
                            SendPacketToThingSpeak(response[0][3],response[0][4],data[6:])
                            S += 1
                        except:
                            pass
                    else:
                        R += 1
            if(mode == "NB"):
                if data != None and data != "" and len(data) != 0:
                    # 紀錄封包
                    PacketLog(data)
                    G += 1
                    num, response = SearchSensorNode(str(hex(data[0])))
                    if(num > 0):
                        try:
                            SendPacketToMQTTServer('140.121.197.228',1884,response[0][5],data[6])
                            S += 1
                        except:
                            pass
                    else:
                        R += 1
            updatePacketHistory(G,S,R)
        except:
            pass

def SearchSensorNode(NodeAddress):
    r = requests.get("http://127.0.0.1:5000/getSensorNode?Address=" + NodeAddress)
    #print(r.json())
    return r.json()['SensorNodeNum'],r.json()['data']

def SendPacketToThingSpeak(field,key,value=[]):
    url_field = ""
    for i in range(field):
        url_field += "&field" + str(i+1) + "=" + str(int(str(value[i])))
    r = requests.get("https://api.thingspeak.com/update?api_key=" + key + url_field)
    return r.status_code

def updatePacketHistory(G,S,R):
    r = requests.put("http://127.0.0.1:5000/updatePacketHistory?IN_Packet="  + str(G) + "&OUT_Packet="  + str(S) + "&REJ_Packet="  + str(R))
    return r.status_code

def getOldPacketSum():
    r = requests.get("http://127.0.0.1:5000/getPacketHistory")
    oldData = r.json()['data'][0]
    #print(oldData)
    return oldData

def getDefalutServer():
    r = requests.get("http://127.0.0.1:5000/getLoRaWanServerInfo")
    if(r.json()['data'][0][4] == 'Yes'):
        return "TS"
    elif(r.json()['data'][1][4] == 'Yes'):
        return "NB"

def SendPacketToMQTTServer(ip,port,topic,data):
    client = mqtt.Client()
    client.connect(ip,port,60)
    client.publish(topic,data)

def PacketLog(data):
    string = ""
    for i in range(6,len(data)):
        string += str(hex(data[i])) + ","
    parms = {"ID":"","PacketFrom":str(hex(data[0])),"PacketTo":str(hex(data[1])),"PacketRelay":str(hex(data[2])),"PacketControl":str(hex(data[3])),"PacketSN":str(hex(data[4])),"ResendSN":str(hex(data[5])),"PacketData":string}
    r = requests.post("http://127.0.0.1:5000/SetPacketLog",data=parms)

# Main Function
while (True):
    global conn
    global addr
    conn , addr = s.accept()
    print("Connect by " + str(addr))
    if(n == 0):
        t = threading.Thread(target=getData)
        t.start()
    n = n + 1

    

