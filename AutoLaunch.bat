@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET process_name=%1%
SET batch_file=%2%
SET /A wait_time=%3% + 1

IF "!process_name!" == "" (
	ECHO error: process name not specified.
	GOTO :EOF
)


IF NOT EXIST !batch_file! (
	ECHO error: batch file '!batch_file!' does not exist.
	GOTO :EOF
)


:loop
	TASKLIST | FIND /I "!process_name!" >NUL
	IF %ERRORLEVEL% EQU 0 GOTO ok
	PING localhost /n 2 >NUL
GOTO loop


:ok
PING localhost /n !wait_time! >NUL
CALL !batch_file!
