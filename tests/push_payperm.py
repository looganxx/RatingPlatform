import eospy.cleos
import eospy.keys
from eospy.types import Abi, Action
from eospy.utils import parse_key_file
import os
import pytz
import json

ce = eospy.cleos.Cleos(url='https://jungle3.cryptolions.io:443')

arguments = {
    "skill_name":"tradizionale"
}
payload = {
    "account": "ratingsystem",
    "name": "addskill",
    "authorization": [{
        "actor": "ratingsystem",
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
# use a string or EOSKey for push_transaction
# key = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"
# use EOSKey:
key = eospy.keys.EOSKey('5J68cwLYxk6QWE3kdnYBbW1eNYKYTT2g7U6QFBY1eh3p5zo87Sc')
resp = ce.push_transaction(trx, key, broadcast=True)
print('------------------------------------------------')
print(resp)
print('------------------------------------------------')

#ratingsystem key
script_dir = os.path.dirname(os.path.realpath(__file__))
key_file = os.path.join(script_dir, 'rsfkeys.key')
key = parse_key_file(key_file)

#find abi file of contract
os.chdir('../contracts/RatingSystem/')
abi_dir = os.getcwd()
abi_file = os.path.join(abi_dir, 'RatingSystem.abi')

with open(abi_file) as rf:
    abi_json = json.load(rf)
    token_abi = Abi(abi_json)

    # wait to send transaction
    import time
    print('Pushing a transaction after 2 seconds using abi file: {}'.format(
        abi_file))
    time.sleep(2)

    act_params = {
        "skill_name":"tradizionale"
    }
    #Converting payload to binary
    data = ce.abi_json_to_bin(payload['account'], payload['name'], act_params)
    print(data)
    act_data = token_abi.json_to_bin(payload['name'], act_params)
    print(act_data)
    #Inserting payload binary form as "data" field in original payload
    payload['data'] = act_data
    #final transaction formed
    trx = {"actions": [payload]}

    resp = ce.push_transaction(trx, eospy.keys.EOSKey(key), broadcast=True)
    print('------------------------------------------------')
    print(resp)
    print('------------------------------------------------')

########################################
# VOTE
########################################
"""
abi_file = os.path.join(script_dir, 'eosio.system.abi')
with open(abi_file) as rf:
    abi_json = json.load(rf)
    sys_abi = Abi(abi_json)

    # wait to send transaction
    import time
    print('Pushing a vote after 2 seconds using abi file: {}'.format(abi_file))
    time.sleep(2)

    vote_payload = {
        "account": "eosio",
        "name": "voteproducer",
        "authorization": [{
            "actor": "dspnewyorkio",
            "permission": "active",
        }],
    }

    vote_params = {
        "voter": "dspnewyorkio",  # sender
        "proxy": "",  # proxy
        "producers": ['bpa1', 'bpa2']  # producers
    }

    # Voting Converting payload to binary
    data = ce.abi_json_to_bin(vote_payload['account'], vote_payload['name'],
                              vote_params)
    print(data)
    vote_data = sys_abi.json_to_bin(vote_payload['name'], vote_params)
    print(vote_data)
    # Inserting payload binary form as "data" field in original payload
    vote_payload['data'] = vote_data

    # final transaction formed
    trx = {"actions": [vote_payload]}

    resp = ce.push_transaction(trx, eospy.keys.EOSKey(key), broadcast=True)
    print('------------------------------------------------')
    print(resp)
    print('------------------------------------------------')
    """