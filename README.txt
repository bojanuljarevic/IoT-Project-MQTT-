1. run the build script by typing ./build in terminal in the working directory
   add the corresponding command line argument for compiling a single source file (e.g. ./build light)
   
2. run the simulators by activating them in the terminal (e.g. ./run light)

3. Observe the results on the ESP32 board (for the supported subsystems)

4. test the output of the simulators by running mosquitto_sub -v -t 'simulator_topic_name' in the broker terminal

5. Use the UserApp to turn subsystems on and off

