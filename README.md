# RatingPlatform

The Rating Platform is a degree internship. The purpose was to analize the EOS.IO blockchain, then develop a DApp consists of smart contract and deploy it on a testnet that performs the consensus protocol of EOS.IO.\
This is my final project for Bachelor degree's thesis.

## contracts

In contracts folder there is the code of two smart contract which togheter are the smart contract.\
**RatingSystem** controls the whole application logic.\ 
**rsf.token** insted is the result of an alteration of the main contract that manages tokens in EOS.IO: "eosio.token".

To compile and deploy it, you first need to install **eosio.cdt**, the Contract Development Toolkit of EOS.IO which provides **eosio-cpp**, the contract compiler.

```
  eosio-cpp -abigen -I include -I ../rsf.token/include/ -R resource -contract RatingSystem -o RatingSystem.wasm src/RatingSystem.cpp 

  eosio-cpp -abigen -I include -I ./include -R resource -contract rsf.token -o rsf.token.wasm src/rsf.token.cpp 
```

## deployment

Then you need to deploy the contract using two different account, because each account can deploy only one smart contract.\
The deploy command is the one provided by eosio.cdt.

```
cleos set contract
```

## tests

Tests fase use the **[eospy](https://github.com/eosnewyork/eospy)** which provides the basics functions for the connection with the testnet and the action performer.\\

Using the below command you can perform one of the main action of the RatingSystem contract using as actor one between alice or bob (two dummy accounts).

```
python push_action.py action_name number_of_transaction_to_send

action_name:
  payperm
  payitem
  addrate

number_of_transaction_to_send:
  1
  5
  10
  25
  50
  100
  500
  1000
```

The result will write in a _.log_ file with similar output:

```
actions.log

##### 5 payperm performed ##### 
alicersfacco: 409	
alicersfacco: 409	
alicersfacco: 409	
alicersfacco: 409	
alicersfacco: 409	
first block: 57431902 - last block: 57431908
block elapsed: 6
number of different blocks: 5
time elapsed: 4.034296989440918
######################### 
```

First there are the cost of RAM in byte at each iteration payed by the account according to the application logic.\
The range of the blocks where the transaction went saved.\
The distance of this blocks and the number of different blocks which contain the actions performed.\
Last there is the time elapsed between first and last iteration.

