.PHONY: clean
clean:
	@echo "cleaning .o files in build and binary files in bin."
	@for file in ../*/build/*.o; do rm -rf $$file; echo Delete Object file: $$file; done
	@for file in ../build/*.o; do rm -rf $$file; echo Delete Object file: $$file; done
	@for file in ../*/bin/*; do rm -rf $$file; echo Delete binary file: $$file; done
	@for file in ../*/*/bin/*; do rm -rf $$file; echo Delete Object file: $$file; done
	@for file in ../*/*/build/*.o; do rm -rf $$file; echo Delete Object file: $$file; done
	@for file in ../*/*/*/build/*.o; do rm -rf $$file; echo Delete Object file: $$file; done
	@for file in ./*.log; do rm -rf $$file; echo Delete log file: $$file; done
	@echo "finished"
