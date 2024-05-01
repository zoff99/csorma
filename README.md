# C Simple ORM (Android?)

### Oh nooo, Why?
I was looking around for something very simple and easy to use in pure C and there was nothing around like that.
<br>
It is based on the wonderful Android-Orma by FUJI Goro
<br>
and on my (not so wonderful) sorma<sup>2</sup>
<br>
https://github.com/maskarade/Android-Orma
<br>
https://github.com/gfx
<br>
https://github.com/zoff99/sorma2

### Features
* pure C
* Thread safe
* safe Strings (UTF-8 or even broken UTF-8 or just random bytes)
* easy to use (for most common SQL operations)
* no dependencies (other than SQLite3 amalagamtion soure file)
* works with ASAN

### What is does NOT
* NOT optimized for speed
* NOT optimized for small memory footprint
* NO complex DB operations like JOIN or UNION etc.
* NO multi column primary keys
* NO non ASCII characters in table and column names
* table and column names can NOT start or end with a `_`
* table and column names MUST only contain `[a-z][_]` (NO uppercase)

### Usage

include the header file:
```C
#include "csorma_runtime.h"
```

initialize the database:
```C
const char *db_dir = "./";
const char *db_filename = "example.db";
OrmaDatabase *o = OrmaDatabase_init(
        db_dir,
        strlen(db_dir),
        db_filename,
        strlen(db_filename)
);
```

run a freehand SQL:
```C
char *sql1 = "CREATE TABLE IF NOT EXISTS Message ("
        "message_id	INTEGER NOT NULL,"
        "read	BOOLEAN,"
        "direction	INTEGER ,"
        "text	TEXT,"
        "id	INTEGER,"
        "PRIMARY KEY(\"id\" AUTOINCREMENT)"
        ");"
        "insert into message(message_id,text) values('123','test message');";
CSORMA_GENERIC_RESULT res1 = OrmaDatabase_run_multi_sql(o, sql1);
 
```

insert a row:
```C
csorma_s *str2 = csb("abc!?%_\\");
Message *m = orma_new_Message(o->db);
m->text = str2;
m->message_id = 200;
int64_t rowid = orma_insertIntoMessage(m);
orma_free_Message(m);
```

update rows:

```C
Message *m_u = orma_updateMessage(o->db);
m_u->message_idEq(m_u, 200)->textSet(m_u, csb("test123"))->execute(m_u);
```
get count of rows:
```C
Message *m_c = orma_selectFromMessage(o->db);
printf("count: %ld\n", m_c->message_idEq(m_c, 200)->textEq(m_c, csb("test123"))->count(m_c));
```

select rows and iterate through the result:
```C
Message *m_s = orma_selectFromMessage(o->db);
MessageList *ml_s = m_s->message_idEq(m_s, 200)->toList(m_s);
printf("items: %ld\n", ml_s->items);
Message **iter = ml_s->ml;
for(int i=0;i<ml_s->items;i++)
{
    printf("id=%ld\n", (*iter)->id);
    printf("mid=%ld\n", (*iter)->message_id);
    printf("text=\"%s\"\n", (*iter)->text->s);
    iter++;
}
orma_free_MessageList(ml_s);
```

delete rows:
```C
Message *m_d = orma_deleteFromMessage(o->db);
m_d->message_idEq(m_d, 200)->execute(m_d);
```

drop table:
```C
char *sql_drop = "DROP TABLE Message;";
CSORMA_GENERIC_RESULT res3 = OrmaDatabase_run_multi_sql(o, sql_drop);
```

shutdown the database:
```C
OrmaDatabase_shutdown(o);
```

### Full example program

```C
#include "csorma_runtime.h"
#include <stdio.h>
#include <string.h>

int main()
{
  const char *db_dir = "./";
  const char *db_filename = "example.db";
  OrmaDatabase *o = OrmaDatabase_init(
        db_dir,
        strlen(db_dir),
        db_filename,
        strlen(db_filename)
  );

  char *sql1 = "CREATE TABLE IF NOT EXISTS Message ("
        "message_id	INTEGER NOT NULL,"
        "read	BOOLEAN,"
        "direction	INTEGER ,"
        "text	TEXT,"
        "id	INTEGER,"
        "PRIMARY KEY(\"id\" AUTOINCREMENT)"
        ");"
        "insert into message(message_id,text) values('123','test message');";
  CSORMA_GENERIC_RESULT res1 = OrmaDatabase_run_multi_sql(o, sql1);

  csorma_s *str2 = csb("abc!?%_\\");
  Message *m = orma_new_Message(o->db);
  m->text = str2;
  m->message_id = 200;
  int64_t rowid = orma_insertIntoMessage(m);
  orma_free_Message(m);

  Message *m_u = orma_updateMessage(o->db);
  m_u->message_idEq(m_u, 200)->textSet(m_u, csb("test123"))->execute(m_u);

  Message *m_c = orma_selectFromMessage(o->db);
  printf("count: %ld\n", m_c->message_idEq(m_c, 200)->textEq(m_c, csb("test123"))->count(m_c));

  Message *m_s = orma_selectFromMessage(o->db);
  MessageList *ml_s = m_s->message_idEq(m_s, 200)->toList(m_s);
  printf("items: %ld\n", ml_s->items);
  Message **iter = ml_s->ml;
  for(int i=0;i<ml_s->items;i++)
  {
      printf("id=%ld\n", (*iter)->id);
      printf("mid=%ld\n", (*iter)->message_id);
      printf("text=\"%s\"\n", (*iter)->text->s);
      iter++;
  }
  orma_free_MessageList(ml_s);

  Message *m_d = orma_deleteFromMessage(o->db);
  m_d->message_idEq(m_d, 200)->execute(m_d);

  char *sql_drop = "DROP TABLE Message;";
  CSORMA_GENERIC_RESULT res3 = OrmaDatabase_run_multi_sql(o, sql_drop);

  OrmaDatabase_shutdown(o);

  return 0;
}
```

<br>
Any use of this project's code by GitHub Copilot, past or present, is done
without our permission.  We do not consent to GitHub's use of this project's
code in Copilot.
