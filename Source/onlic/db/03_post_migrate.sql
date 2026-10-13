-- 03_post_migrate.sql : pgloader 두 번 실행 후 (02a → 02b 순서 무관)
-- 1) 190.7 의 IPCS_LOG(2차 폴백 유입분) 를 본 테이블에 병합 (identity 재부여)
INSERT INTO ipcs_log (lic_insert_date, log_data, log_ip, log_type, log_real_ip)
SELECT lic_insert_date, log_data, log_ip, log_type, log_real_ip FROM ipcs_log_from_lic2
ORDER BY lic_insert_date;
DROP TABLE IF EXISTS ipcs_log_from_lic2;

-- 2) identity 시퀀스를 현재 최대값 다음으로
SELECT setval(pg_get_serial_sequence('callbox_log', 'log_index'), COALESCE((SELECT MAX(log_index) FROM callbox_log), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('callcenter_link', 'cc_index'), COALESCE((SELECT MAX(cc_index) FROM callcenter_link), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('callcenter_link_reference', 'ccr_index'), COALESCE((SELECT MAX(ccr_index) FROM callcenter_link_reference), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('enterprise', 'idx'), COALESCE((SELECT MAX(idx) FROM enterprise), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('ipcs_log', 'lic_index'), COALESCE((SELECT MAX(lic_index) FROM ipcs_log), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('lic_access_list', 'lic_ip_index'), COALESCE((SELECT MAX(lic_ip_index) FROM lic_access_list), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('nasa_lic', 'nl_index'), COALESCE((SELECT MAX(nl_index) FROM nasa_lic), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('nasa_lic_log', 'nlog_index'), COALESCE((SELECT MAX(nlog_index) FROM nasa_lic_log), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('on_alarm_user', 'oau_idx'), COALESCE((SELECT MAX(oau_idx) FROM on_alarm_user), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('project', 'idx'), COALESCE((SELECT MAX(idx) FROM project), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('server', 'idx'), COALESCE((SELECT MAX(idx) FROM server), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('site_email_user', 'idx'), COALESCE((SELECT MAX(idx) FROM site_email_user), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('site_sms_basic_user', 'site_sms_index'), COALESCE((SELECT MAX(site_sms_index) FROM site_sms_basic_user), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('site_smsexcludetime', 'sm_index'), COALESCE((SELECT MAX(sm_index) FROM site_smsexcludetime), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('softphone_log', 'log_index'), COALESCE((SELECT MAX(log_index) FROM softphone_log), 0) + 1, false);
SELECT setval(pg_get_serial_sequence('stc_calllist', 'idx'), COALESCE((SELECT MAX(idx) FROM stc_calllist), 0) + 1, false);

-- 3) 통계
ANALYZE;

-- 4) 검증 (이관 당일 원본 건수와 비교. 2026-10-13 기준값: callcenter_link 2049, site_sms_user 25640, site_sms_basic_user 28,
--    site_email_user 8, site_smsexcludetime 4, lic_access_list 859, ipcs_log 326235+1986, nasa_lic 698, nasa_lic_log 1947)
SELECT 'callcenter_link' t, COUNT(*) FROM callcenter_link UNION ALL
SELECT 'site_sms_user', COUNT(*) FROM site_sms_user UNION ALL
SELECT 'site_sms_basic_user', COUNT(*) FROM site_sms_basic_user UNION ALL
SELECT 'site_email_user', COUNT(*) FROM site_email_user UNION ALL
SELECT 'site_smsexcludetime', COUNT(*) FROM site_smsexcludetime UNION ALL
SELECT 'lic_access_list', COUNT(*) FROM lic_access_list UNION ALL
SELECT 'ipcs_log', COUNT(*) FROM ipcs_log UNION ALL
SELECT 'nasa_lic', COUNT(*) FROM nasa_lic UNION ALL
SELECT 'nasa_lic_log', COUNT(*) FROM nasa_lic_log;

-- 5) onlic 이 쓰는 쿼리 1건 점검 (Windows 소스 그대로, 식별자 대소문자 무관)
SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_use='Y' AND cc_link_error='N';
select count(*) from lic_access_list where nasa_con_ip='203.239.190.7' and nasa_use='Y';
