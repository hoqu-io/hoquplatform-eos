#nettestacc
#EOS8BkzkguX539Vd6spJ2bxZEYcKVs7i6ZfkpYKwMp5vUh9erSn4k
#5KKrjPp7oTSpPWVh6gSDHasZEyukJnaEsK7UgiJF1SKf87D4nXP

#merchtestacc
#EOS88jtvMKNZsoMS5fAXA85HuooTmcthXHnRqFqfNK6mBSkonqxLS
#5J2ooSPULNDBvTcwxEoUYFjBR4MQcouvLMcXY6cGpuLCYwBqKz3

#afftestacc
#EOS8HPz1jXupskCpgf8GiRvxzhxWwdjxv6sdTbcft6E3oBPkew7RU
#5Jpb8aXdXCAqiH9PG3kaNoiYDQvVd15jk6PhVEEkttid1zyPucW

all: clean
	eosio-cpp -o HOQUPlatform.wasm HOQUPlatform.cpp --abigen
	eosio-cpp -o HOQUToken.wasm HOQUToken.cpp --abigen
	cleos set contract hoquplatform /root/contracts/HOQUPlatform -p hoquplatform@active
	cleos set contract hoqutoken /root/contracts/HOQUToken -p hoqutoken@active
	cleos push action hoqutoken create '["hoqutoken", "275854380.000000000 HQX"]' -p hoqutoken@active
	cleos set account permission hoquplatform active '{"threshold": 1, "keys":[{"key":"EOS6Vbm9emcdUheFJmPutmtPimLGBeeSgR26FzY6o6JAq1ke6CDoz", "weight":1}], "accounts":[{"permission":{"actor":"hoquplatform","permission":"eosio.code"},"weight":1}], "waits":[] }' owner -p hoquplatform

clean:
	rm -Rf *.abi *.wasm
	-ps -fea | grep nodeos | grep -v grep | awk {'print $$2'} | xargs kill -9
	cd /root/ && rm -Rf CONTRACTS_DIR nodeos.log && nodeos -e -p eosio --plugin eosio::producer_plugin --plugin eosio::chain_api_plugin --plugin eosio::http_plugin --plugin eosio::history_plugin --plugin eosio::history_api_plugin --data-dir CONTRACTS_DIR/eosio/data --config-dir CONTRACTS_DIR/eosio/config --access-control-allow-origin='*' --contracts-console --http-validate-host=false --verbose-http-errors --filter-on='*' >> nodeos.log 2>&1 &
	sleep 3
	-cleos wallet unlock --password PW5KQMbwwySCRtp182CUXd5gpAAvz8qcUo6n3SPth2FsardLA46gi
	cleos create account eosio hoquplatform EOS8Bqx38Uqa3oorZsxuDK2V8Dtodos5x5fEuQgqh7CKDawdRzMgf
	cleos create account eosio hoqutoken EOS6Vbm9emcdUheFJmPutmtPimLGBeeSgR26FzY6o6JAq1ke6CDoz

test: create_accounts test_users

create_accounts:
	-cleos wallet unlock --password PW5KQMbwwySCRtp182CUXd5gpAAvz8qcUo6n3SPth2FsardLA46gi
	-cleos create account eosio nettestacc EOS8BkzkguX539Vd6spJ2bxZEYcKVs7i6ZfkpYKwMp5vUh9erSn4k
	-cleos create account eosio merchtestacc EOS88jtvMKNZsoMS5fAXA85HuooTmcthXHnRqFqfNK6mBSkonqxLS
	-cleos create account eosio afftestacc EOS8HPz1jXupskCpgf8GiRvxzhxWwdjxv6sdTbcft6E3oBPkew7RU
	-cleos push action hoqutoken issue '["merchtestacc", "10000.000000000 HQX", "money for test"]' -p hoqutoken@active
	cleos push action hoqutoken approve '["merchtestacc", "hoquplatform", "10000.000000000 HQX"]' -p merchtestacc@active

test_users:
	cleos push action hoquplatform useradd '["nettestacc", "network"]' -p hoquplatform@active
	cleos push action hoquplatform useradd '["merchtestacc", "merchant"]' -p hoquplatform@active
	cleos push action hoquplatform useradd '["afftestacc", "affiliate"]' -p hoquplatform@active

	cleos push action hoquplatform userlogin '["afftestacc", 0x41D3FFD5D8800000]' -p afftestacc@active

test_networks:
	cleos push action hoquplatform networkadd '[55067438, "nettestacc", "HOQU Net"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform networks
	cleos push action hoquplatform networkupd '[55067438, "HOQU Network", "active"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform networks

test_offers:
	cleos push action hoquplatform offeradd '[23874, 55067438, "merchtestacc", "HOQU Offer", "merchtestacc"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offerupd '[23874, "HOQU Offer 2", "merchtestacc", "pending"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offeradd '[23875, 55067438, "merchtestacc", "HOQU Offer", "merchtestacc"]' -p hoquplatform@active
	cleos push action hoquplatform offerupd '[23875, "HOQU Offer 3", "merchtestacc", "active"]' -p hoquplatform@active
	cleos push action hoquplatform offerupd '[23874, "HOQU Offer 2", "merchtestacc", "active"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers

test_offer_tariffs:
	cleos push action hoquplatform offtariffadd '[23874, 1, "callback", "100.500000000 HQX", 0, false, 30]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offtariffupd '[23874, 1, "callback", "100.000000000 HQX", 0, false, 30]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offtariffact '[23874, 1]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offtariffoff '[23874, 1]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offtariffadd '[23874, 2, "callback", "100.650000000 HQX", 0, false, 33.345]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers
	cleos push action hoquplatform offtariffact '[23874, 2]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform offers

test_ads:
	cleos push action hoquplatform adadd '[5363, 23874, "afftestacc"]' -p hoquplatform@active
	cleos push action hoquplatform adadd '[2360, 23874, "afftestacc"]' -p hoquplatform@active
	cleos push action hoquplatform adadd '[3363, 23874, "afftestacc"]' -p hoquplatform@active
	cleos push action hoquplatform adupd '[5363, "active"]' -p hoquplatform@active
	cleos push action hoquplatform adupd '[2360, "active"]' -p hoquplatform@active
	cleos push action hoquplatform adadd '[5364, 23875, "afftestacc"]' -p hoquplatform@active
	cleos push action hoquplatform adupd '[5364, "active"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform ads

test_leads:
	cleos push action hoquplatform leadadd '[1, 5363, 2, "0.000000000 HQX"]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform leads
	cleos push action hoquplatform leadsell '[1]' -p hoquplatform@active
	cleos get table --show-payer hoquplatform hoquplatform leads
	cleos get currency balance hoqutoken merchtestacc HQX
	cleos get currency balance hoqutoken afftestacc HQX
	cleos get currency balance hoqutoken nettestacc HQX
	cleos get currency balance hoqutoken hoquplatform HQX

