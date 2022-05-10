#include "include/b_plus_tree.hpp"
struct node {
    char ch[30];
    bool operator==(const node& rhs) const { return strcmp(ch, rhs.ch) == 0; }
    bool operator<(const node& rhs) const { return strcmp(ch, rhs.ch) < 0; }
};
huang::DiskManager dm("test_disk_manager");
huang::Replacer<huang::Page*> rep;
huang::BufferPoolManager bpm(512, &dm, &rep);
huang::BPlusTree<node, int> bpt("string", &bpm);
int main() {
    freopen("test.in", "r", stdin);
    freopen("test.out", "w", stdout);
    int n;
    std::cin >> n;
    node N;
    for (int i = 1; i <= n; i++) {
        std::string op;
        int value;
        std::cin >> op;
        if (op == "insert") {
            std::cin >> N.ch >> value;
            bpt.Insert(N, value);
        } else if (op == "delete") {
            std::cin >> N.ch >> value;
            bpt.Remove(N);
        } else if (op == "find") {
            std::cin >> N.ch;
            auto find = bpt.GetValue(N);
            if (find.second)
                std::cout << *find.first << std::endl;
            else
                std::cout << "null" << std::endl;
        }
    }
    return 0;
}