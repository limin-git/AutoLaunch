@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET process_name=%1%
SET batch_file=%2%
SET /A wait_time=%3% + 1
SET has_error=false

IF "%process_name%" == "" SET has_error=true &SET error_msg_1=process-name not specified.
IF NOT EXIST !batch_file! SET has_error=true &SET error_msg_2=batch-file '!batch_file!' does not exist.

IF "%has_error%" == "true " (
	IF NOT "%error_msg_1%" == "" ECHO error: %error_msg_1%
	IF NOT "%error_msg_2%" == "" ECHO error: %error_msg_2%
	GOTO :EOF
)


:LOOP
	TASKLIST | FIND /I "%process_name%" >NUL
	IF %ERRORLEVEL% EQU 0 GOTO OK
	PING localhost /n 2 >NUL
GOTO LOOP


:OK
PING localhost /n !wait_time! >NUL
CALL %batch_file%
