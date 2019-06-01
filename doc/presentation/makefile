.PHONY: all
all: presentation.html

presentation.html: embedded.md
	landslide embedded.md

.PHONY: show
show: presentation.html
	firefox presentation.html

.PHONY: install_landslide
install_landslide:
	sudo apt-get install python-landslide
