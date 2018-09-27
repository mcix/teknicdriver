Created by Arnoud van der Heijde

This DLL is used to control the teknic motors from Java, using JNI

To build the DLL:
- install Visual C++ build tools visualcppbuildtools.exe
- start cmd using the Visual C++ 2-15 MSBuild Command Prompt
- navigate to the project/jni folder and run:
    - buildjava.bat
    - build.bat
    
There is an example app located in DeltaProtoDriver.java, use run.bat to start this example app. It might be required to change the COM port in the run.bat file.