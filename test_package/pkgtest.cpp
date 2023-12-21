#include <sqlite3pp/Database.h>

int main() {
    sqlite3pp::Database db{":memory:"};
    return 0;
}
