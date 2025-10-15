init時向kernel註冊device major/minor number

透過insmod在/dev底下建立device file後，user 執行 open/close 對應到file operation中的open/release function



Ex :  ./test  /dev/hello0

