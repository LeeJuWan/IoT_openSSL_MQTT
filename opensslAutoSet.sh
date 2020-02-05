# !/bin/sh

# Open SSL CA,SERVER,CLIENT .key, .csr, .crt Create&Update

if [ "$EUID" -ne 0 ]
	then echo "please root access ex) su root"
	exit
	
	else
		echo "root access & Open SSL creat & update start......"
fi

sleep 1

# step_1 Craete CA.key
expect << EOL
spawn openssl genrsa -out ca.key 2048
expect eof
EOL
echo "CREATE CA.KEY SUCCESS...."
echo "================================================"

sleep 1
# step_2 Create CA.crt
expect << EOL
spawn openssl req -new -x509 -days 360 -key ca.key -out ca.crt
expect -re "Country Name*"
send "KR\n"
expect -re "State or Province Name*"
send "Chungnam\n"
expect -re "Locality Name*"
send "Chonan\n"
expect -re "Organization Name*"
send "KongjuUniv\n"
expect -re "Organization Unit Name*"
send "Comp\n"
expect -re "Common Name*"
send "SONG\n"
expect -re "Email Address*"
send "\n"
expect eof
EOL
echo "CREATE CA.CRT SUCCESS...."
echo "================================================"

sleep 1
# step_3 Create Servar.key
expect << EOL
spawn openssl genrsa -out server.key 2048
expect eof
EOL
echo "CREATE SERVER.KEY SUCCESS....."
echo "================================================"

sleep 1
# step_4 Create Server.csr
expect << EOL
spawn openssl req -new -out server.csr -key server.key
expect -re "Country Name*"
send "KR\n"
expect -re "State or Province Name*"
send "Chungnam\n"
expect -re "Locality Name*"
send "Chonan\n"
expect -re "Organization Name*"
send "KongjuUniv\n"
expect -re "Organizational Unit Name*"
send "Server\n"
expect -re "Common Name*"
send "SONG\n"
expect -re "Email Address*"
send "\n"
expect -re "A challenge password*"
send "test\n"
expect -re "An optional company name*"
send "\n"
expect eof
EOL
echo "Create SERVER.CSR SUCCESS"
echo "================================================"

sleep 1
# step_5 Create Server.crt
expect << EOL
spawn openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 360
expect eof
EOL
echo "CREATE SERVER.CRT SUCCESS"
echo "================================================"
echo "Server.crt Verifying...."
sleep 1
# step_6 Verify Server.crt
if [ "`openssl verify -CAfile ca.crt server.crt`" = "server.crt: OK" ]
	then
		echo "SERVER.CRT VERIFY OK"
	else
		echo "SERVER.CRT VERIFY NOT OK"
fi

sleep 1
# step_7 Create Client.key
expect << EOL
spawn openssl genrsa -out client.key 2048
expect eof
EOL
echo "CREATE CLIENT.KEY SUCCESS"
echo "================================================"
sleep 1
# step_8 Create Client.csr
expect << EOL
spawn openssl req -new -out client.csr -key client.key
expect -re "Country Name*"
send "KR\n"
expect -re "State or Province Name*"
send "Chungnam\n"
expect -re "Locality Name*"
send "Chonan\n"
expect -re "Organization Name*"
send "KongjuUniv\n"
expect -re "Organizational Unit Name*"
send "Client\n"
expect -re "Common Name*"
send "SONG\n"
expect -re "Email Address*"
send "\n"
expect -re "A challenge password*"
send "test\n"
expect -re "An optional company name*"
send "\n"
expect eof
EOL
echo "CREATE CLIENT.CSR SUCCESS"
echo "================================================="

sleep 1
# step_9 Create Client.crt
expect << EOL
spawn openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 360
expect eof
EOL
echo "CREATE CLIENT.CRT SUCCESS"

echo "Client.crt Verifying...."
sleep 1
# step_10 Veirfy Client.crt
if [ "`openssl verify -CAfile ca.crt client.crt`" = "client.crt: OK" ]
	then
		echo "CLIENT.CRT VERIFY OK "
	else
		echo "CLIENT.CRT VERIFT NOT OK"
fi

echo "=======================Finish OpenSSL SHELL================="

# step_10 send key data esp8266
expect << EOL
spawn openssl x509 -in client.crt -outform der -out clientesp.der
spawn openssl rsa -in client.key -outform der -out espkey.der
expect eof
EOL
`openssl x509 -noout -in server.crt -fingerprint | awk -F = '{print$2}' > hash_file`
echo "CREATE DER&HASH SUCCESS"
