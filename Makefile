include nemu/Makefile.git

default:
	@echo "Please run 'make' under any subprojects to compile."
 
clean:
	-$(MAKE) -C nemu clean
	-$(MAKE) -C nexus-am clean
	-$(MAKE) -C nanos-lite clean
	-$(MAKE) -C navy-apps clean

submit: clean
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s http://moon.nju.edu.cn/~jyy/teach/submit.sh)"

git:
	@git add .
	@git commit --allow-empty
.PHONY: default clean submit git
