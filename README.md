# nrf24l01
Modified spidev for nrf24l01 linux driver

Build the project and execute the program nrf on the board as instructed below
Note: I have cross complied for am335x (phyBOARD WEGA)
```
root@wega:-# ./nrf t 33 Greetings
```
The above command will make the nrf24L01 connceted to the wega board a transmitter and transmits the message "Greetings" to the channel 33 (00-FF channels are avalible)
after this u may keep sending the messages by directly typing like a chat application streaming on the same channel 
```
root@wega:-# ./nrf r 33
```
The above command will receive any messages being transmitted on the 33 channel.
