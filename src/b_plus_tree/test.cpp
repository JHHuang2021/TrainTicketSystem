#include "include/b_plus_tree.hpp"
huang::DiskManager dm("test_disk_manager");
huang::Replacer<huang::Page*> rep;
huang::BufferPoolManager bpm(512, &dm, &rep);
huang::BPlusTree<int, double> bpt(&bpm);
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
        std::cin >> op;
        int key;
        double value;
        if (op == "insert") {
            std::cin >> key >> value;
            bpt.Insert(key, value);
        } else if (op == "delete") {
            std::cin >> key;
            bpt.Remove(key);
        } else if (op == "find") {
            std::vector<double> ans;
            std::cin >> key;
            auto ret = bpt.GetValue(key);
            if (ret.second)
                std::cout << *(ret.first) << std::endl;
            else
                std::cout << "null" << std::endl;
        }
    }
    // bpt.Insert(1, {1});
    // auto ret=bpt.GetValue(1);


    return 0;
}