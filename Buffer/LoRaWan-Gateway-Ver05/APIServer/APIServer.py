from crypt import methods
import json
from sqlite3 import Time
from flask import Flask
from flask import request
from flask_cors import CORS
import pymysql
import time

# # api setting
app = Flask(__name__)
CORS(app)

# connect db
try:
    db = pymysql.connect(host="127.0.0.1",port=3306,user="root",passwd="raspberry",database="LoRaWanGatewayDB")
except Exception as e:
    print(e)

# 特殊函數區 
# ===================================================================================================================================================================
# 檢查資料是否已存在
def SearchHistory(f,t,sq):
    cmd = "select * from PacketLog where _From='" + f + "' and _To= '" + t + "' and SuqenceNumber='" + sq + "';"
    cursor = db.cursor() 
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    if(len(resluts) > 0):
        return True
    else:
        return False

# API 撰寫區
# ===================================================================================================================================================================
@app.route('/addUser',methods=['POST'])
def addUser():
    ID = request.values['ID']
    userName = request.values['userName']
    password = request.values['password']
    cursor = db.cursor() 
    cmd = "insert into Users(ID,UserName,Password) values("+ str(ID) +",'" + str(userName) +"','" + str(password) +"');"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/getUsers',methods=['GET'])
def getUsers():
    cursor = db.cursor()
    cmd = "select * from Users;" 
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    data =[]
    for reslut in resluts:
        data.append(reslut)
    return json.dumps({"status" : True,"UserNum":len(data),"data":data})

@app.route('/deleteUser',methods=['DELETE'])
def deleteUser():
    ID = request.values['ID']
    cursor = db.cursor() 
    cmd = "delete from Users where ID = "  + ID +  ";"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/login',methods=['GET'])
def login():
    userName = request.args.get('userName')
    password = request.args.get('password')
    print(userName)
    cursor = db.cursor()
    cmd = "select * from Users where UserName='"+ userName +"' and Password='"+ password +"';" 
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    for reslut in resluts:
        print(reslut)
        return json.dumps({"status" : True})
    return json.dumps({"status" : False})

@app.route('/addSensorNode',methods=['POST'])
def addSersorNode():
    ID = request.values['ID']
    Name = request.values['Name']
    Address = request.values['Address']
    FieldNum = request.values['FieldNum']
    Write_API_Key = request.values['Write_API_Key']
    MQTT_Publish_Path = request.values['MQTT_Publish_Path']
    cursor = db.cursor() 
    cmd = "insert into SensorNode(ID,Name,Address,FieldNum,Write_API_Key,MQTT_Publish_Path) values("+ str(ID) +",'" + str(Name) +"','" + str(Address) +"'," + str(FieldNum) + ",'" + str(Write_API_Key) + "','" + str(MQTT_Publish_Path) + "');"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/updateSensorNode',methods=['PUT'])
def updateSensorNode():
    ID = request.values['ID']
    Name = request.values['Name']
    FieldNum = request.values['FieldNum']
    Write_API_Key = request.values['Write_API_Key']
    MQTT_Publish_Path = request.values['MQTT_Publish_Path']
    cursor = db.cursor() 
    cmd = "update SensorNode set Name = '" + str(Name) + "',FieldNum=" + str(FieldNum) + " ,Write_API_Key='" + str(Write_API_Key) + "',MQTT_Publish_Path='" + str(MQTT_Publish_Path) + "' where ID = "  +  str(ID) + " ;"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/getSensorNode',methods=['GET'])
def getSensorNode():
    try:
        Address = request.args.get('Address')
        cmd = "select * from SensorNode where Address='"+ Address +"';" 
    except:
        cmd = "select * from SensorNode;" 
    cursor = db.cursor()
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    num = 0
    data = []
    for reslut in resluts:
        num += 1
        data.append(reslut)
    return json.dumps({"status" : True,"SensorNodeNum":num,"data":data})

@app.route('/deleteSensorNode',methods=['DELETE'])
def deleteSensorNode():
    ID = request.values['ID']
    cursor = db.cursor() 
    cmd = "delete from SensorNode where ID = "  + ID +  ";"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/getLoRaWanServerInfo',methods=['GET'])
def getLoRaWanServerInfo():
    try:
        Provider = request.args.get('Provider')
        cmd = "select * from LoRaWanServerInfo where Provider='"+ Provider +"';" 
    except:
        cmd = "select * from LoRaWanServerInfo;"
    print(Provider) 
    cursor = db.cursor()
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    num = 0
    data = []
    for reslut in resluts:
        num += 1
        data.append(reslut)
    return json.dumps({"status" : True,"LoRaWanServerNum":num,"data":data})

@app.route('/getPacketHistory',methods=['GET'])
def getPacketHistory():
    cmd = "select * from PacketHistory;" 
    cursor = db.cursor()
    cursor.execute(cmd)
    db.commit()
    resluts = cursor.fetchall()
    num = 0
    data = []
    for reslut in resluts:
        num += 1
        data.append(reslut)
    return json.dumps({"status" : True,"PacketHistoryNum":num,"data":data})

@app.route('/updatePacketHistory',methods=['PUT'])
def updatePacketHistory():
    ID = 1
    IN_Packet = request.values['IN_Packet']
    OUT_Packet = request.values['OUT_Packet']
    REJ_Packet = request.values['REJ_Packet']
    cursor = db.cursor() 
    cmd = "update PacketHistory set IN_Packet = '" + str(IN_Packet) + "',OUT_Packet=" + str(OUT_Packet) + " ,REJ_Packet='" + str(REJ_Packet) + "' where ID = "  +  str(ID) + " ;"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/changeMainServer',methods=['PUT'])
def changeMainServer():
    Provider = request.args.get('Provider')
    print(Provider)
    cursor = db.cursor() 
    if(Provider == "ThingSpeak"):
        cmd = "update LoRaWanServerInfo set IsMain='Yes' where Provider='ThingSpeak';"
        cursor.execute(cmd)
        cmd= "update LoRaWanServerInfo set IsMain='No' where Provider='NTOU_Broker';"
        cursor.execute(cmd)
    if(Provider == "NTOU_Broker"):
        cmd = "update LoRaWanServerInfo set IsMain='No' where Provider='ThingSpeak';"
        cursor.execute(cmd)
        cmd ="update LoRaWanServerInfo set IsMain='Yes' where Provider='NTOU_Broker';"
        cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

@app.route('/SetPacketLog',methods=['POST'])
def SetPacketLog():
    global db
    ID = 0
    PacketFrom = request.values['PacketFrom']
    PacketTo = request.values['PacketTo']
    PacketRelay = request.values['PacketRelay']
    PacketControl = request.values['PacketControl']
    PacketSN = request.values['PacketSN']
    ResendSN = request.values['ResendSN']
    PacketData = request.values['PacketData']
    Time = time.strftime("%m-%d-%Y,%H:%M:%S",time.localtime())
    if not (SearchHistory(PacketFrom,PacketTo,PacketSN)):
        cmd = "insert into PacketLog(ID,Time,_From,_To,_Relay,Control,SuqenceNumber,ResendSN,Data) values (0,'" + Time + "','" + PacketFrom + "','" + PacketTo + "','" + PacketRelay + "','" + PacketControl + "','" + PacketSN + "','" + ResendSN + "','" + PacketData + "')"
        cursor = db.cursor() 
        try:
            cursor.execute(cmd)
            db.commit()
        except:
            db = pymysql.connect(host="127.0.0.1",port=3306,user="root",passwd="raspberry",database="LoRaWanGatewayDB")
            cursor.execute(cmd)
            db.commit()
        return json.dumps({"state":True})
    else:
        return json.dumps({"state":False})

@app.route('/GetPacketLog',methods=['GET'])
def GetPacketLog():
    global db
    cmd = "select * from PacketLog;" 
    cursor = db.cursor()
    try:
        cursor.execute(cmd)
        db.commit()
    except:
        db = pymysql.connect(host="127.0.0.1",port=3306,user="root",passwd="raspberry",database="LoRaWanGatewayDB")
        cursor.execute(cmd)
        db.commit()
    resluts = cursor.fetchall()
    num = 0
    data = []
    for reslut in resluts:
        num += 1
        data.append(reslut)
    return json.dumps({"status" : True,"PacketLogNum":num,"data":data})

@app.route('/DeletetPacketLog',methods=['DELETE'])
def DeletetPacketLog():
    cursor = db.cursor() 
    cmd = "delete from PacketLog;"
    cursor.execute(cmd)
    db.commit()
    return json.dumps({"state":True})

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0',port=5000)


