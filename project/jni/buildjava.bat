javac DeltaProtoDriver.java
robocopy . nl\bytesoflife\ *.class
javah -jni nl.bytesoflife.DeltaProtoDriver
copy /Y nl_bytesoflife_DeltaProtoDriver.h DeltaProtoDriver.h