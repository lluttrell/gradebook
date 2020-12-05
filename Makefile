all: gradebook

gradebook: gradebook.c
	gcc -fno-stack-protector -o gradebook gradebook.c
	sudo mv gradebook /usr/bin/gradebook
	sudo chown registrar /usr/bin/gradebook
	sudo chgrp registrar /usr/bin/gradebook
	sudo chmod 4555 /usr/bin/gradebook