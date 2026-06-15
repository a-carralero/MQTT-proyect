.PHONY: all clean libs libs-clean

all:
	$(MAKE) -C libs
	$(MAKE) -C publisher
	$(MAKE) -C subscriber_metrics
	$(MAKE) -C subscriber_camera


clean:
	$(MAKE) -C publisher clean
	$(MAKE) -C subscriber_metrics clean
	$(MAKE) -C subscriber_camera clean
	$(MAKE) -C libs clean

libs:
	$(MAKE) -C libs

libs-clean:
	$(MAKE) -C libs clean


	
