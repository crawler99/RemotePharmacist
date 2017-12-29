:RESTART
tasklist /FI "username eq jtao" | find /C "SessionServer.exe" > temp.txt
set /p num= < temp.txt
del /F temp.txt
echo %num%
if "%num%" == "0" start /D "D:\Development\GitHub\RemotePharmacist_New\SessionServer\Release\" SessionServer.exe
ping -n 10 -w 1000 0.0.0.1 > temp.txt
del /F temp.txt
goto RESTART
