#include <sqlite_modern_cpp.h>

using namespace std;

struct User {
    long long             id;
    std::optional<int>    age;
    std::optional<string> name;
    std::optional<double> weight;
};

int main() {
    User user;
    user.name = "bob";
    user.age  = 32;

    // Same database as above
    sqlite::database db("dbfile.db");

    db << "create table if not exists user ("
          "   id integer primary key autoincrement not null,"
          "   age int,"
          "   name text,"
          "   weight real"
          ");";

    // Here, age and weight will be inserted as NULL in the database.
    db << "insert into user (age,name,weight) values (?,?,?);" << user.age << user.name << user.weight;
    user.id = db.last_insert_rowid();

    // Here, the User instance will retain the NULL value(s) from the database.
    db << "select id,age,name,weight from user where age > 18;" >>
        [](long long id, std::optional<int> age, std::optional<string> name, std::optional<double> weight) {
            cout << "id=" << id;
            cout << " age=" << (age ? to_string(*age) : "NULL");
            cout << " name=" << (name ? *name : "NULL");
            cout << " weight=" << (weight ? to_string(*weight) : "NULL") << endl;
        };
}
