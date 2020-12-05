from eospy.cleos import Cleos
import eospy.keys

#ce = Cleos(url='http://localhost:8888') 
ce = Cleos(url='https://jungle3.cryptolions.io:443')
#ce = eospy.cleos.Cleos(url='http://api.pennstation.eosnewyork.io:7001')

#print(ce.get_account("alicersfacco"))


# use a string or EOSKey for push_transaction
#key = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"

# use EOSKey:
key = eospy.keys.EOSKey('5J68cwLYxk6QWE3kdnYBbW1eNYKYTT2g7U6QFBY1eh3p5zo87Sc')

resp = ce.create_account('ratingsystem', key, 'testrsfaccou', 
  'EOS7DuzbxnV2uKw31tynT9DnGaHszn3R1cZbELZwxUFWUTpZZkjKV', 
  'EOS7DuzbxnV2uKw31tynT9DnGaHszn3R1cZbELZwxUFWUTpZZkjKV',
  stake_net='1.0000 EOS', stake_cpu='1.0000 EOS', ramkb=8, 
  permission='active', transfer=False, broadcast=True)

print('------------------------------------------------')
print(resp)
print('------------------------------------------------')