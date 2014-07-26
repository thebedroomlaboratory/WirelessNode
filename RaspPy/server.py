#!/usr/bin/python
from serial import Serial
import re
import urllib
import json
import time


serial_pattern = r"T: (\d+\.\d*)\n";
serial_port = '/dev/ttyACM0';
serial_bauds = 57600;
url = r'http://dev.thebedroomlaboratory.com:9000/gal/all'
urlPost = r'http://dev.thebedroomlaboratory.com:9000/zone/powerstrip/'
last_sent=0
def open_serial_port() :
    s = Serial(serial_port, serial_bauds);
    return s;

def readSerial(s) :
    return s.readline();
#{"socket1":true,"socket2":true,"socket3":true,"socket4":true,"zone1":{"lightingStatus":true,"door":false},"zone2":{"lightingStatus":true},"zone3":{"lightingStatus":true}}       


def writeSerial(s, what):
    return s.write(what);

if __name__ == "__main__":
    interval = 2000;
    now = int(round(time.time() * 1000))
    if ( (now - last_sent) >= interval  ):
        last_sent = now; 
        seperator = ","
        print "Opening serial port"
        s = open_serial_port();
        print "Reading first line from port"
        line = readSerial(s);
        print "Initializing communication"
        while 1:
            # read arduino
            serialRead = readSerial(s);
            # read server
            serverRead = urllib.urlopen(url).read()
            # print out arduino
            print serialRead
            # print out server
            print serverRead
            # write to arduino
            data = json.loads(serverRead)
            serialData=str(data["socket1"]) + seperator + str(data["socket2"]) + seperator + str(data["socket3"]) + seperator + str(data["socket4"]) + seperator + str(data["zone1"]["lightingStatus"]) +  seperator + str(data["zone2"]["lightingStatus"]) + seperator + str(data["zone3"]["lightingStatus"])
            print serialData
            serialWrite = writeSerial(s, serialData);
            # print arduino reply
            print serialWrite
            # write to server
            params0 = "socket4=false"
            vals = serialRead.split(' ');
            print len(vals), vals
            if vals[5] == 0:
                val = "false"
            else:
                val = "true"
                try:
                    params0={"voltage":vals[0],"power":vals[1],"zone_g_lux":vals[2],"zone_g_temp":vals[3],"zone_g_humidity":vals[4],"zone_g_door":val,"zone_f_lux":vals[6],"zone_f_temp":vals[7],"zone_f_humidity":vals[8],"zone_d_lux":vals[9],"zone_d_temp":vals[10],"zone_d_humidity":vals[11]}
                    params1 = urllib.urlencode(params0)
                    serverWrite = urllib.urlopen(url, params1).read()
                    # print server reply
                    print serverWrite

                    if vals[12] == 0:
                        val1 = "false"
                    else:
                        val1 = "true"
                    if vals[13] == 0:
                        val2 = "false"
                    else:
                        val2 = "true"
                    if vals[14] == 0:
                        val3 = "false"
                    else:
                        val3 = "true"
                    if vals[15] == 0:
                        val4 = "false"
                    else:
                        val4 = "true"

                    params0={"socket1":val1,"socket2":val2,"socket3":val3,"socket4":val4}
                    params1 = urllib.urlencode(params0)
                    try:
                        serverWrite = urllib.urlopen(urlPost, params1).read()
                    except:
                        print "Eek!"
                    # print server reply
                    print serverWrite
            
                    serverWrite = urllib.urlopen(url).read()
                    # print server reply
                    print serverWrite
                except:
                    print "Eeek-2!"



        

