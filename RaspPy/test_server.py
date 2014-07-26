#!/usr/bin/python

import urllib

if __name__ == "__main__":
    url = r'http://dev.thebedroomlaboratory.com:9000/gal/all'
    urlPost = r'http://dev.thebedroomlaboratory.com:9000/zone/powerstrip'
    urlPull = r'http://dev.thebedroomlaboratory.com:9000/zone/id/1'
    serialRead = "230 12.53 701 23 40 0 0 0 0 0 0 0";    
    vals = serialRead.split(' ');
    if vals[5] == 0:
        val = "false"
    else:
        val = "true"
    params0={"voltage":vals[0],"power":vals[1],"zone_g_lux":vals[2],"zone_g_temp":vals[3],"zone_g_humidity":vals[4],"zone_g_door":val,"zone_f_lux":vals[6],"zone_f_temp":vals[7],"zone_f_humidity":vals[8],"zone_d_lux":vals[9],"zone_d_temp":vals[10],"zone_d_humidity":vals[11]}
    params1 = urllib.urlencode(params0)
    serverWrite = urllib.urlopen(url, params1).read()
    print serverWrite

    params0={"socket1":"false","socket2":"true","socket3":"false","socket4":"true"}
    params1 = urllib.urlencode(params0)
    try:        
        serverWrite = urllib.urlopen(urlPost, params1).read()
    except:
        print "Shit"
    # print server reply
    print serverWrite

    serverWrite = urllib.urlopen(url).read()
    print serverWrite

    serverWrite = urllib.urlopen(urlPull).read()
    print serverWrite
    
    
