@echo off
rem ============================================================
rem  db_export.bat - 절체용 운영 MSSQL -^> PostgreSQL INSERT 파일 추출
rem  사용:  db_export.bat            (두 서버 전부)
rem         db_export.bat --dry-run  (원본 건수만 확인)
rem         db_export.bat --source 44
rem         db_export.bat --out D:\cutover
rem  접속정보: 이 폴더의 db_export.ini (없으면 db_export.ini.sample 을 복사해 비밀번호를 채우세요)
rem  원본 DB 에는 SELECT(bcp) 만 수행합니다
rem ============================================================
setlocal
chcp 949 >nul
cd /d "%~dp0"

if not exist "db_export.ini" (
    echo [오류] db_export.ini 가 없습니다. db_export.ini.sample 을 db_export.ini 로 복사한 뒤 password 를 채우세요.
    pause
    exit /b 1
)

set PYEXE=
where py >nul 2>nul && set PYEXE=py -3
if "%PYEXE%"=="" where python >nul 2>nul && set PYEXE=python
if "%PYEXE%"=="" (
    echo [오류] Python 3 를 찾을 수 없습니다.
    pause
    exit /b 1
)

echo ============================================================
echo  MSSQL -^> PostgreSQL 데이터 추출 시작  %date% %time%
echo ============================================================
set PYTHONIOENCODING=utf-8
%PYEXE% -I "%~dp0db_export_mssql_to_pg.py" %*
set RC=%ERRORLEVEL%

echo.
if "%RC%"=="0" (
    echo [완료] 건수 일치. 산출물: %~dp0^<YYYYMMDD_HHMMSS^>\  ^(_report.txt 확인^)
) else (
    echo [주의] 종료코드 %RC% - 건수 불일치 또는 오류. 위 로그와 _report.txt 를 확인하세요.
)
echo.
pause
exit /b %RC%
