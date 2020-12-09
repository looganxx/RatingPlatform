import eospy.cleos
import eospy.keys
from eospy.types import Abi, Action
from eospy.utils import parse_key_file
import os
import pytz
import json
import yaml

ce = eospy.cleos.Cleos(url='https://jungle3.cryptolions.io:443')

bill=0.50
for _ in range(1000):
	bill = ((bill + 0.5)%1)+1
	arguments = {
		"item": "sushi",
		"owner": "alicersfacco",
		"client": "bobrsfaccoun",
		"bill": "{price:.2f} RSF".format(price=bill)
	}
	payload = {
		"account": "ratingsystem",
		"name": "payperm",
		"authorization": [{
				"actor": "alicersfacco",
				"permission": "active",
		}],
	}
	#Converting payload to binary
	data = ce.abi_json_to_bin(payload['account'], payload['name'], arguments)
	#Inserting payload binary form as "data" field in original payload
	payload['data'] = data['binargs']
	#final transaction formed
	trx = {"actions": [payload]}
	import datetime as dt
	trx['expiration'] = str(
		(dt.datetime.utcnow() + dt.timedelta(seconds=60)).replace(tzinfo=pytz.UTC))

	key = eospy.keys.EOSKey('5Keb8LCEwfSCBinCn2AcN8KQBKSjJ7inoJnUJo6aSHMMJvGSiN7')

	with open("payperm.log", "a") as log:
		resp = ce.push_transaction(trx, key, broadcast=True)
		#print (resp)
		delta_ram = resp['processed']['action_traces'][0]['account_ram_deltas'][0]
		# Append text at the end of file
		log.writelines([delta_ram['account'], ": " , "{}".format(delta_ram['delta']), "\n" ])

"""
print('------------------------------------------------')
print(resp)
print (yaml.dump(resp, default_flow_style=False))
print('------------------------------------------------')
"""