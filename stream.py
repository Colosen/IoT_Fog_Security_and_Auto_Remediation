from scapy.all import *
import os
import converter
import requests
import json

url = "http://localhost:3000/api/submit"

f = open('file.txt', 'w')

with PcapReader('/home/jitheesh/contiki-ng/tools/cooja/radiolog-1693066099646.pcap') as pkts:
    for pkt in pkts:
        # print(pkt.summary())
        x = pkt.summary()
        f.write(pkt.summary() + '\n')
        l = converter.convert(pkt.summary())
        if l:
            data_json = json.dumps({"packet": l})
            print(data_json)
            headers = {"Content-Type": "application/json"}
            response = requests.post(url, data=data_json, headers=headers)
            print(response)
        # os.system('curl -X POST -H "Content-Type: application/json" -d ' + data_json + ' http://localhost:3000/api/submit')
