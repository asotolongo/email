/*  Anthony Sotolongo Leon (asotolongo@uci.cu)
 * emial es una extension que agrega un tipo de datos email,
 * permite insertar direcciones de correo electrónico
 * tiene 2 funciones que permiten obetner el usuario y el dominio de determianda
 * direccion de correo, tambien cuenta con operadores = y <>
 *
 * gracias a la colaboracion de Daymel Bonne, Adrian Quintero  y Frank Alberto
 * */


/*
 * email is a extension to add email data type for postgresql,
 * it allows to insert data in email format
 * it has 2 function that allows to get  email user and domain ,
 *  also it has 2  operators = y <>
 *
 *  Special thanks  for Daymel Bonne, Adrian Quintero  and Frank Alberto
 * */

#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "string.h"
#include <regex.h>
#include "catalog/pg_type.h"
#include "utils/builtins.h"



/*
 * modulo mágico necesario para este tipo de programación
 * declaracion de las funciones necesaria desde la version V1
 */
 
 PG_MODULE_MAGIC;
Datum		email_in(PG_FUNCTION_ARGS);
Datum		email_out(PG_FUNCTION_ARGS);
Datum		email_recv(PG_FUNCTION_ARGS);
Datum		email_send(PG_FUNCTION_ARGS);
Datum		email_equal(PG_FUNCTION_ARGS);
Datum		email_notequal(PG_FUNCTION_ARGS);
Datum		email_hash(PG_FUNCTION_ARGS);
Datum		getuser(PG_FUNCTION_ARGS);
Datum		getdomain(PG_FUNCTION_ARGS);


static int * hashvalue(const char * mail);



PG_FUNCTION_INFO_V1(email_in);

Datum
email_in(PG_FUNCTION_ARGS)
{
	char	*correct;
	char	   *mail = PG_GETARG_CSTRING(0);

	char      *reg_exp_mail = "^[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,4}$";

	regex_t regex;
	int pre;

        pre = regcomp(&regex,reg_exp_mail,1);
        pre = regexec(&regex, mail, 0, NULL, 0);
                if( !pre ){
                	correct = mail;

                }
                else {
                	ereport(ERROR,(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                	                				   				 errmsg("invalid input syntax for user mail :\"%s\"", mail)));
                }

        regfree (&regex);
	
	PG_RETURN_TEXT_P(cstring_to_text(correct));
}

PG_FUNCTION_INFO_V1(email_out);

Datum
email_out(PG_FUNCTION_ARGS)
{
	/*
	  esta es para devolver el dato en el formato de texto
	*/
	Datum		mail = PG_GETARG_DATUM(0);
        
	PG_RETURN_CSTRING(TextDatumGetCString(mail));
}

PG_FUNCTION_INFO_V1(email_recv);
Datum
email_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);


	BpChar	   *result;
	char	   *str;
	int			nbytes;

	str = pq_getmsgtext(buf, buf->len - buf->cursor, &nbytes);
	memcpy(result, str, nbytes);
	pfree(str);
	PG_RETURN_BPCHAR_P(result);
}
PG_FUNCTION_INFO_V1(email_send);
Datum
email_send(PG_FUNCTION_ARGS)
{
	char		*s = PG_GETARG_CHAR(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendtext(&buf, s, strlen(s));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}



PG_FUNCTION_INFO_V1(getuser);
Datum
getuser(PG_FUNCTION_ARGS)
{
	Datum		mail = TextDatumGetCString(PG_GETARG_DATUM(0));

	int size,i;
	size=strlen(mail);
	text *result;
	char *mailtemp =mail;

	for (i = 0; i < size; i++)
		{
			if (mailtemp[i]=='@')
			{

				break;
			}

		}
	result =palloc(i+1+VARHDRSZ);
	SET_VARSIZE(result, i + VARHDRSZ);
	memcpy(VARDATA(result), mailtemp, i);

   PG_RETURN_TEXT_P(result);

}

PG_FUNCTION_INFO_V1(getdomain);
Datum
getdomain(PG_FUNCTION_ARGS)
{
	Datum		mail = TextDatumGetCString(PG_GETARG_DATUM(0));

	int size,i;
	size=strlen(mail);
	text *result;
	char * mailtemp =mail;
	char * substring;

	substring= strtok (mailtemp,"@");
	substring= strtok (NULL,"@");

	i = strlen(substring);
	result =palloc(i+1+VARHDRSZ);
	SET_VARSIZE(result, i + VARHDRSZ);
	memcpy(VARDATA(result), substring, i);

	PG_RETURN_TEXT_P(result);

}


PG_FUNCTION_INFO_V1(email_equal);
Datum
email_equal(PG_FUNCTION_ARGS)
{
	Datum		mail1 = TextDatumGetCString(PG_GETARG_DATUM(0));
	Datum		mail2 = TextDatumGetCString(PG_GETARG_DATUM(1));

	int size1,size2,i;

    size1=strlen(mail1);
    size2=strlen(mail2);
    if (size1!=size2)
    {
    	//flag=1;
    	PG_RETURN_BOOL(0);
    }
    if( strcmp( mail1, mail2 ) != 0 )
    {
        PG_RETURN_BOOL(0);
    }
    else
        PG_RETURN_BOOL(1);


}

PG_FUNCTION_INFO_V1(email_notequal);
Datum
email_notequal(PG_FUNCTION_ARGS)
{
	Datum		mail1 = TextDatumGetCString(PG_GETARG_DATUM(0));
	Datum		mail2 = TextDatumGetCString(PG_GETARG_DATUM(1));

	int size1,size2,i;

    size1=strlen(mail1);
    size2=strlen(mail2);
    if (size1!=size2)
    {
    	PG_RETURN_BOOL(1);
    }
    if( strcmp( mail1, mail2 ) != 0 )
    {
        PG_RETURN_BOOL(1);
    }
    else
        PG_RETURN_BOOL(0);

}


PG_FUNCTION_INFO_V1(email_hash);
Datum
email_hash(PG_FUNCTION_ARGS)
{
	char		*mail = PG_GETARG_CSTRING(0);


	PG_RETURN_INT32(hashvalue(mail));
}


/*funcion para producir un hash par ael indice pero está aun muyyy primaria*/
int * hashvalue(const char * mail)
{

	int size,sum,i ;
    sum=0;

	size =strlen(cstring_to_text(mail));
	for (i = 0; i < size; i++)
		{
		sum=sum+ (int) mail[i];

		}

	return sum;

}


