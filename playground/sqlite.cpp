#include <sqlite_modern_cpp.h>

using namespace std;

struct User {
    int64_t               id=-1;
    std::optional<int>    age;
    std::optional<string> name;
    std::optional<double> weight;
};

int main() {
    User user;
    user.name = "bob";
    user.age  = 32;

    sqlite::database db("dbfile.db");

    db << "create table if not exists user ("
          "  id integer primary key autoincrement not null,"
          "  t timestamp default (datetime('now')),"
          "  age,"
          "  name not null,"
          "  weight);";

    db << "insert into user (age,name,weight) values (?,?,?);" << user.age << user.name << user.weight;
    user.id = db.last_insert_rowid();

    db << "select id,age,name,weight from user where age > 18;" >>
        [](int64_t id, std::optional<int> age, string name, std::optional<double> weight) {
            cout << "id=" << id;
            cout << " age=" << (age ? to_string(*age) : "NULL");
            cout << " name=" << name;
            cout << " weight=" << (weight ? to_string(*weight) : "NULL") << endl;
        };
}
