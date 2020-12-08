all: gradebook studentgrades

gradebook: gradebook.c
	gcc -fno-stack-protector -o gradebook gradebook.c
	sudo mv gradebook /usr/bin/gradebook
	sudo chown registrar /usr/bin/gradebook
	sudo chgrp faculty /usr/bin/gradebook
	sudo chmod 4755 /usr/bin/gradebook
	sudo cp ./help.txt /home/registrar/help.txt
	sudo chmod 744 /home/registrar/help.txt

studentgrades:
	sudo rm -r /home/registrar/studentgrades
	sudo mkdir -m750 /home/registrar/studentgrades
	sudo chmod g+s /home/registrar/studentgrades
	sudo chown registrar /home/registrar/studentgrades
	sudo chgrp faculty /home/registrar/studentgrades
