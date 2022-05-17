#include "include/b_plus_tree.hpp"
huang::DiskManager dm("test_disk_manager");
huang::Replacer<huang::Page*> rep;
huang::BufferPoolManager bpm(512, &dm, &rep);
huang::BPlusTree<int, int> bpt(&bpm);
int main() {
    freopen("test0.in", "r", stdin);
    freopen("test.out", "w", stdout);
    int n;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        // std::cout << "Case :" << i << std::endl;
        // bpt.Debug();
        // std::cout << std::endl;
        std::string op;
        int key, value;
        std::cin >> op;
        if (op == "insert") {
            std::cin >> key >> value;
            bpt.Insert(key, value);
        } else if (op == "delete") {
            std::cin >> key;
            bpt.Remove(key);
        } else if (op == "find") {
            std::cin >> key;
            auto find = bpt.GetValue(key);
            if (find.second)
                std::cout << *find.first << std::endl;
            else
                std::cout << "null" << std::endl;
        }
    }
    // bpt.Debug();
    return 0;
}