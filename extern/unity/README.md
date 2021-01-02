# Unity

This is a stripped directory to hold the Unity testing framework located at https://github.com/ThrowTheSwitch/Unity and http://www.throwtheswitch.org/

The source has been modified to invalidate the setup and teardown functionality. These elements are not necessary for SCIPACK and it simplifies the build process.

The makefile is triggered recursively, and should not be used directly.