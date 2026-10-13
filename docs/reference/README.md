# reference/

포팅 호환성 검증용 외부 참조 자료. 이 폴더의 소스는 **이 프로젝트의 빌드 대상이 아님**.

| 파일 | 출처 | 용도 |
|---|---|---|
| `client_license_check.cpp` | IVR 클라이언트 ShareLib (사용자 제공, 2026-10-13) | TCP 6321 라이선스 체크 클라이언트 측 구현. `WriteServerLog`(요청/응답 처리), `CheckLicense`/`WriteLicense`(로컬 .lic 검증). 프로토콜명세 §3.5의 근거이며, 포팅 후 테스트 클라이언트 작성 시 모사 대상 |
| (외부 경로) `F:\AI_Pro\_proc\onms\source_onms\ms_260424` + `ncmsivr` | IVR 앱(ncms)과 SipStack 라이브러리 (복사하지 않음) | `ms_260424/source/ncms/SystemLoad.cpp:906,1106-1118`(설정·호출), `ms_260424/source/ncms/ConfigLoad.cpp:387-392`(`USE_CHECK_LICENSE` 기본 1), `ncmsivr/source/SipStack/SipMsg_Lib.cpp:967-971`(체크 분기), `ncmsivr/source/ShareLib/wrappthread.cpp:444-559`(IVR 타입 구현, `client_license_check.cpp`와 동일 내용). 프로토콜명세 §3.7 |
| (외부 경로) `F:\AI_Pro\_proc\onmw\onmw_source\onmw_3.10.79` + `F:\AI_Pro\_proc\onmw\docs\analysis\*` | CTI 미들웨어 onmw 소스와 기존 분석 문서 — **Linux 포팅 구조 참조(라이브러리·패턴)** | 공용 라이브러리 `include/{ShareLib,LogObject,TimerObject,ListObject,UrlEnDe}`, 앱 패턴 `source/ncmw/{ncmwmain,SystemLoad,ConfigLoad,DirectODBC,SocketThread,RunAgtSocketThread,SockProcAgt,RecvAgtWaitStack,SQLWaitStack,MwTimer}.cpp`, 빌드 `linux/Makefile_64bit*`. 기존 분석: `docs/analysis/agt_socket_analysis.md`, `db_pointer_thread_safety_analysis.md`, `CLAUDE.md`. `docs/리눅스참조구조.md` |
| (외부 경로) `F:\AI_Pro\_proc\ondemon\OnDemon` | 감시 데몬 OnDemon 소스 (복사하지 않음) — **UDP 5363 주 송신자** | `source/AppMonitor/ProcManager.cpp:484-520`(초기화·DNS·UDP 20001 바인드), `:860-912`(LINK_CHECK·이벤트 송신), `:461-482`(ACK 수신→큐 제거), `:403-436`(타이머·임계값, nLinkTime=300 고정), `include/AppMonitor/app_struct.h:12-30`(nType enum), `cfg/ondemon.ini`(설정 예). 프로토콜명세 §5.10 |
| (외부 경로) `F:\AI_Pro\_proc\_onbk\bk_source` | 녹취 백업 ncbk 소스 (복사하지 않음) | UDP 5363 송신 클라이언트. `source/ncbk/SystemLoad.cpp:1263-1397`(`SmsEventSend` 프레임·sendto), `:389-399`(lic2.onpia.co.kr DNS), `:433-438`(CallCenterEnable DB 판정), `include/ncbk/struct_def.h:206-221`(nType 301~308, 포트), `BkFtpUpLoad.cpp:455-515`·`BkCapacityProcThread.cpp:108-161`(발생 조건). 프로토콜명세 §5.9 |
| (외부 경로) `F:\AI_Pro\_proc\onrec\onrec_source\ncir-2.10.22` | 녹취 서버 ncir 소스 (복사하지 않음) | 호출 흐름 확인 근거. `source/ncir/SystemLoad.cpp:267-274`(호출부), `source/ncir/ConfigLoad.cpp:188-191`(`USE_CHECK_LICENSE` 기본 1), `source/ObjectLib/WrapThread.cpp:283-355, 497-600`(VRS 타입 구현), `bin/VRS6cc217657ae1.lic`(샘플 파일). 프로토콜명세 §3.6 |
