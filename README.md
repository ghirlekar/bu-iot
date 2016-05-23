Boston University - School of Public Health
===========================================

About
-----

Boston University School of Public Health project for wireless sensor networks to monitor indoor conditions.


Getting Started
---------------

I highly recommend setting up the local toolchain with the Particle github repository [here](https://github.com/spark/firmware) instead of using the cloud toolchain. I've had consistency issues with the latter especially when the Photon pops in and out of deep sleep mode. It is far faster and easier to compile and flash via USB. 

Next, clone this git repository into spark_repo_path/user/applications as described [here](https://github.com/spark/firmware/blob/develop/docs/build.md#building-an-application). 

Then run `make clean all PLATFORM=photon APP=bu-iot program-dfu` with spark_repo_path/main as the working directory. 

The [application.cpp](../master/application.cpp) contains the application code to run on the Photon.

Contact
-------

- Dr. Thomas Little
- Gaurav Hirlekar (gauravh@bu.edu)
