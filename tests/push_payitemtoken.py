import eospy.cleos
import eospy.keys
from eospy.types import Abi, Action
from eospy.utils import parse_key_file
import os
import pytz
import json
import yaml

ce = eospy.cleos.Cleos(url='https://jungle3.cryptolions.io:443')

bill=0.5
for i in range(1027,1032):
  bill = ((bill + 0.5)%1)+1

  arguments = {
    "idpay": "{}".format(i),
    "user": "bobrsfaccoun",
    "quantity": "{price:.2f} RSF".format(price=bill),
    "pay_with_token": "1"
  }

  payload = {
    "account": "ratingsystem",
    "name": "payitem",
    "authorization": [{
        "actor": "bobrsfaccoun",
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

  key = eospy.keys.EOSKey('5JJjc6cb9zCn8tD7AZNtri1fvQ4gg7C7Q8r4MgHijKrJJxcqMsN')

  with open("payitemtoken.log", "a") as log:
    resp = ce.push_transaction(trx, key, broadcast=True)
    print (resp)
    delta_ram = resp['processed']['action_traces'][0]['account_ram_deltas']
    for delta in delta_ram:
      # Append text at the end of file
      log.writelines([delta['account'], ": " , "{}".format(delta['delta']), "\t" ])
    log.write("\n")
