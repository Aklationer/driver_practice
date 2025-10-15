init時向kernel註冊device major/minor number

透過insmod在/dev底下建立device file後，user 執行 cat/echo 對應到file operation中的 read/write function


雖然沒有額外定義 open/release 但還是可運行  VFS有通用流程

