eol-keyboard-remapper: remapper.c
	clang -O3 -o eol-keyboard-remapper remapper.c

.PHONY: install
install: eol-keyboard-remapper udevmon.yaml udevmon.service
    cp ./eol-keyboard-remapper /usr/local/bin/
	cp ./udevmon.yaml /etc/
	cp ./udevmon.service /etc/systemd/system/
	systemctl daemon-reload

.PHONY: uninstall
uninstall:
	rm -rf /usr/local/bin/eol-keyboard-remapper
	rm -rf /etc/udevmon.yaml
	rm -rf /etc/systemd/system/udevmon.service

  
