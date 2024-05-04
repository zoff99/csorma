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

### What is does NOT do
* NOT optimized for speed
* NOT optimized for small memory footprint
* NO complex DB operations like JOIN or UNION etc.
* NO multi column primary keys
* NO non ASCII characters in table and column names
* table and column names MUST NOT start or end with a `_` or a number
* table and column names MUST only contain `[a-z][_]` (NO uppercase)
* if table or column name starts with (or contains) `public` or `static` there could be issues(*)

### Usage
#### Create your first C project with csorma
create a directory for your project:
```bash
mkdir -p ./mysuperstuff/
```

create one file for each database table that you need.
<br>
create file for db table `Person` as `./mysuperstuff/_csorma_Person.java`
```Java
@Table
public class Person
{
    @PrimaryKey(autoincrement = true)
    public long id;
    @Column
    public String name;
    @Column
    public String address;
    @Column
    public int social_number;
}
```

now create the sources with the generator. you need at least java 17.<br>
```bash
javac csorma_generator.java && java csorma_generator ./mysuperstuff/
```

your project is now ready to start.<br>
enter the project directory:
```bash
cd ./mysuperstuff/gen/
```

now build your C project stub and run it:
```bash
make csorma_stub
./csorma_stub
```

the output should look (something) like this:
```
STUB: CSORMA version: 0.99.0
STUB: CSORMA SQLite version: 3.45.3
TEST: creating table: Person
TEST: res1: 0

STUB: all OK
```

you now have your working project stub.<br>

#### Add some more SQL stuff to your new project

open your C project stub in your favorite C Code Editor or IDE:
```
vim csorma_stub.c
```

now let's add commands<br>
(after the `"OrmaDatabase_run_multi_sql()"` line)<br>
to insert a row:
```C
{ // HINT: using blocks here to have `p` be a local var
Person *p = orma_new_Person(o->db);
p->name = csb("Larry Wilson");
p->address = csb("1 Larry Drive, Sunset Town");
p->social_number = 381739;
int64_t inserted_id = orma_insertIntoPerson(p);
printf("STUB: inserted id: %lld\n", (long long)inserted_id);
} // HINT: using blocks here to have `p` be a local var
```

now lets count how many Persons have the social number `381739`:
```C
{
Person *p = orma_selectFromPerson(o->db);
printf("STUB: count: %d\n", (int)p->social_numberEq(p, 381739)->count(p));
}
```

now do the same but use the `less than` operator `Lt()`
```C
{
Person *p = orma_selectFromPerson(o->db);
printf("STUB: count: %d\n", (int)p->social_numberLt(p, 400000)->count(p));
}
```

now insert another Person:
```C
{
Person *p = orma_new_Person(o->db);
p->name = csb("Martha Liebowitz");
p->address = csb("2035 Morning Road, Big City");
p->social_number = 139807;
int64_t inserted_id = orma_insertIntoPerson(p);
printf("STUB: inserted id: %lld\n", (long long)inserted_id);
}
```

#### Stub C code, some functions explained

include the header file:
```C
#include "csorma_runtime.h"
```

initialize the database:
```C
const char *db_dir = "./";
const char *db_filename = "stub.db";
OrmaDatabase *o = OrmaDatabase_init(
    (uint8_t*)db_dir, strlen(db_dir),
    (uint8_t*)db_filename, strlen(db_filename)
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

shutdown the database:
```C
OrmaDatabase_shutdown(o);
```

<br>
Any use of this project's code by GitHub Copilot, past or present, is done
without our permission.  We do not consent to GitHub's use of this project's
code in Copilot.
