--------------
====README====
--------------

RoBIOS software Readme

---Contents---
-Section code: Section Title	
-SSP: Software parameters

---SSP: Software parameters
The RoBIOS software parameters are stored in the file robios_params.txt. When the RoBIOS software first starts up it will attempt to read from this file. If it cannot read from the file it will attempt to create the file, setting the parameters to their default values in the process.
The format for each parameter is as follows: parameter value
For example: irCode 786
It is important that the formatting is correct having a semicolon, colon or anything else between the parameter and the value will result in read errors and could cause some strange behaviour.

If the RoBIOS software does not find a particular parameter, whether it is not in the file or incorrectly formatted it will revert to the default value for the parameters not specified in the file.
To edit the parameters in the RoBIOS software head to the settings page located on the Info page in the program
