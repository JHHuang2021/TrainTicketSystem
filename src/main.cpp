#include "user.h"
// #include "train.h"
// #include "order.h"
#include "command_parser.h"

int main() {
  lin::UserManager user_manager;
  lin::CommandParser command_parser(&user_manager);
  command_parser.Run();
  return 0;
}