create type email;


CREATE FUNCTION email_in(cstring)
RETURNS email
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION email_out(email)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;




CREATE FUNCTION email_recv(internal)
RETURNS email
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION email_send(email)
RETURNS bytea
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION email_equal(email,email)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION email_notequal(email,email)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

-- está comentareada pues aun está un poco primaria su implementacion en C
/*CREATE FUNCTION email_hash(email)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;*/


CREATE FUNCTION getuser(email)
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION getdomain(email)
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE email (
        INTERNALLENGTH = -1,
        INPUT = email_in,
        OUTPUT = email_out,
        RECEIVE = email_recv,
        SEND = email_send,
        STORAGE = extended


);


CREATE  or REPLACE FUNCTION get_user(mail email) RETURNS character varying AS
$BODY$
declare
usr character varying;
begin
usr:= substring(mail::character varying from 1 for position('@' in mail::character varying)-1);
return usr;
end;
$BODY$
LANGUAGE plpgsql VOLATILE;

CREATE  or REPLACE FUNCTION get_domain(mail email) RETURNS character varying AS
$BODY$
declare
usr character varying;
begin
usr:= substring(mail::character varying from position('@' in mail::character varying)+1 for length(mail::character varying));
return usr;
end;
$BODY$
LANGUAGE plpgsql VOLATILE;


--está temporalmente hasta que se mejore la implementacion del hash en C
CREATE OR REPLACE FUNCTION email_hash(doc1 email)
  RETURNS integer AS
$BODY$
declare
valor character varying ;
suma integer=0;
i integer=1;
begin
  valor=md5($1::character varying);

  i := length(valor);
  loop
    suma:=suma+ascii(substring(valor from i for 1));
    i:=i-1;
    if i=0 then
      exit;
    end if;
  end loop;


return suma* ascii(substring(valor from 1 for 1))*ascii(substring(valor from 2 for 1));


end;

$BODY$
  LANGUAGE plpgsql VOLATILE;

CREATE OPERATOR = (
leftarg = email,
rightarg = email,
procedure = email_equal,
 NEGATOR = <>

);

CREATE OPERATOR <> (
leftarg = email,
rightarg = email,
procedure = email_notequal,
 NEGATOR = =

);

CREATE OPERATOR CLASS email_equal_ops
DEFAULT FOR TYPE email USING hash AS
OPERATOR 1   = ,

FUNCTION 1 email_hash(email);

--CREATE INDEX test_index_email ON prueba USING hash(campo email_equal_ops);



