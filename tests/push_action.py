#!/usr/bin/env python3

#!Author: Sacco Giuseppe

import eospy.cleos
import eospy.keys
from eospy.types import Abi, Action
from eospy.utils import parse_key_file
import os
import sys
import pytz
import json
import yaml
import time

def main():
    #check parameters
    #python push_action.py action_name number_of_transaction_to_send
    if len(sys.argv) != 3 :
        exit("use: python push_action.py action_name number_of_transaction_to_send")

    dicAction = ('payperm' , 'payitem' , 'addrate')
    if(sys.argv[1] not in dicAction):
        exit("use: " + str(dicAction))

    dicNumTransaction = (1, 5, 10, 25, 50, 100, 500, 1000)
    if(int(sys.argv[2]) not in dicNumTransaction):
        exit("use: " + str(dicNumTransaction))

    action = sys.argv[1]
    numAction = int(sys.argv[2])
    pay_with_token = bool(False)
    if action == 'payitem':
        res = input('Do you want to pay with tokens? [y/n]\n')
        if res in ('y', 'yes', 'Y', 'YES'):
            pay_with_token = bool(True)
    
    if action in ('payitem', 'addrate'):
      firstBill = int(input('first bill: \n'))

    if action == 'payperm':
        performedAction = payperm(numAction)
    elif action == 'payitem':
        performedAction = payitem(numAction, pay_with_token, firstBill)
    else:
        performedAction = addrate(numAction, firstBill)

    actionList = performedAction[0]
    blocksNum = []
    with open("actions.log", "a") as log:
      if (action == "payitem" and pay_with_token):
        log.writelines(["##### ", str(numAction) , " " , action , " with coupon performed ##### \n"])
      else:
        log.writelines(["##### ", str(numAction) , " " , action , " performed ##### \n"])
      for act in actionList:
          blocksNum.append(act['processed']['block_num'])
          delta_ram = act['processed']['action_traces'][0]['account_ram_deltas']
          for delta in delta_ram:
              log.writelines([delta['account'], ": " , "{}".format(delta['delta']), "\t" ])
          log.write("\n")
      blocksElapsed = blocksNum[len(blocksNum) - 1] - blocksNum[0]
      log.writelines(["first block: ", str(blocksNum[0]) , " - last block: ", str(blocksNum[len(blocksNum) - 1]) ,"\n"])
      log.writelines(["block elapsed: ", str(blocksElapsed) , "\n"])
      #no duplicates in list 
      #to achieve the number of different blocks
      blocksNum = list(dict.fromkeys(blocksNum))
      log.writelines(["number of different blocks: ", str(len(blocksNum)) , "\n"])
      log.writelines(["time elapsed: ", str(performedAction[1]) , "\n"])
      log.write("######################### \n")


##############################################
### payperm 1 1 5 5 5 5 5 5 5
##############################################
def payperm(num_action):
    bill=0.50
    start = time.time()
    resp = []
    for i in range(num_action):
        bill = ((bill + 0.5)%1)+1
        arguments = {
         "item": "sushi",
         "owner": "alicersfacco",
         "client": "bobrsfaccoun",
         "bill": "{price:.4f} RSF".format(price=bill)
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
        data = jungle.abi_json_to_bin(payload['account'], payload['name'], arguments)
        #Inserting payload binary form as "data" field in original payload
        payload['data'] = data['binargs']
        #final transaction formed
        trx = {"actions": [payload]}
        import datetime as dt
        trx['expiration'] = str(
         (dt.datetime.utcnow() + dt.timedelta(seconds=60)).replace(tzinfo=pytz.UTC))

        key = eospy.keys.EOSKey('5Keb8LCEwfSCBinCn2AcN8KQBKSjJ7inoJnUJo6aSHMMJvGSiN7')

        resp.append(jungle.push_transaction(trx, key, broadcast=True))

    return [resp, time.time() - start]


##############################################
### payitem
##############################################
def payitem(num_action, token, firstBill):
    bill=0.50
    start = time.time()
    resp = []
    for i in range(num_action):
        bill = ((bill + 0.5)%1)+1
        arguments = {
          "idpay": "{}".format(firstBill),
          "user": "bobrsfaccoun",
          "quantity": "{price:.4f} RSF".format(price=bill),
          "pay_with_token": "{coupon}".format(coupon=int(token))
        }
        firstBill = firstBill + 1
        payload = {
          "account": "ratingsystem",
          "name": "payitem",
          "authorization": [{
              "actor": "bobrsfaccoun",
              "permission": "active",
          }],
        }

        #Converting payload to binary
        data = jungle.abi_json_to_bin(payload['account'], payload['name'], arguments)
        #Inserting payload binary form as "data" field in original payload
        payload['data'] = data['binargs']
        #final transaction formed
        trx = {"actions": [payload]}
        import datetime as dt
        trx['expiration'] = str(
         (dt.datetime.utcnow() + dt.timedelta(seconds=60)).replace(tzinfo=pytz.UTC))

        key = eospy.keys.EOSKey('5JJjc6cb9zCn8tD7AZNtri1fvQ4gg7C7Q8r4MgHijKrJJxcqMsN')

        resp.append(jungle.push_transaction(trx, key, broadcast=True))

    return [resp, time.time() - start]


##############################################
### addrate
##############################################
def addrate(num_action, firstBill):
  start = time.time()
  resp = []
  for i in range(num_action):
      rate = (i%8)+1
      arguments = {
          "idpayment": "{}".format(firstBill),
          "user": "bobrsfaccoun",
          "score": "{}".format(rate) 
      }
      firstBill = firstBill + 1
      payload = {
          "account": "ratingsystem",
          "name": "addrate",
          "authorization": [{
              "actor": "bobrsfaccoun",
              "permission": "active",
          }],
      }

      #Converting payload to binary
      data = jungle.abi_json_to_bin(payload['account'], payload['name'], arguments)
      #Inserting payload binary form as "data" field in original payload
      payload['data'] = data['binargs']
      #final transaction formed
      trx = {"actions": [payload]}
      import datetime as dt
      trx['expiration'] = str(
        (dt.datetime.utcnow() + dt.timedelta(seconds=60)).replace(tzinfo=pytz.UTC))

      key = eospy.keys.EOSKey('5JJjc6cb9zCn8tD7AZNtri1fvQ4gg7C7Q8r4MgHijKrJJxcqMsN')

      resp.append(jungle.push_transaction(trx, key, broadcast=True))

  return [resp, time.time() - start]

if __name__ == "__main__":
    jungle = eospy.cleos.Cleos(url='https://jungle3.cryptolions.io:443')
    main()
