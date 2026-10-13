-- 00_create_db.sql : 최초 1회 (postgres 슈퍼유저로)
--   psql -U postgres -f 00_create_db.sql
CREATE ROLE onlic LOGIN PASSWORD 'CHANGE_ME';
CREATE DATABASE nasa_license OWNER onlic ENCODING 'UTF8' LC_COLLATE 'C' LC_CTYPE 'C' TEMPLATE template0;
-- 이후: psql -U onlic -d nasa_license -f 01_nasa_license_schema.sql
-- 서버 인코딩 UTF8 + 클라이언트 EUCKR(onlic 이 set client_encoding) 조합. CP949 확장 한자/기호가 EUC-KR 범위를 벗어나면 변환 오류가 나므로
-- 문제가 되면 DB 를 ENCODING 'EUC_KR' 로 만드는 대안도 가능 (psqlODBC 는 양쪽 모두 지원)
