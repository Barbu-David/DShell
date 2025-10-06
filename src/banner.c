#include "banner.h"
#include "colors.h"
#include <stdio.h>

void print_banner(const char *color) {

  const char *dshell_banner = 
    "DDDDDDDDDDDDD           SSSSSSSSSSSSSSS HHHHHHHHH     HHHHHHHHHEEEEEEEEEEEEEEEEEEEEEELLLLLLLLLLL             LLLLLLLLLLL             \n"
    "D::::::::::::DDD      SS:::::::::::::::SH:::::::H     H:::::::HE::::::::::::::::::::EL:::::::::L             L:::::::::L             \n"
    "D:::::::::::::::DD   S:::::SSSSSS::::::SH:::::::H     H:::::::HE::::::::::::::::::::EL:::::::::L             L:::::::::L             \n"
    "DDD:::::DDDDD:::::D  S:::::S     SSSSSSSHH::::::H     H::::::HHEE::::::EEEEEEEEE::::ELL:::::::LL             LL:::::::LL             \n"
    "  D:::::D    D:::::D S:::::S              H:::::H     H:::::H    E:::::E       EEEEEE  L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::DS:::::S              H:::::H     H:::::H    E:::::E               L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::D S::::SSSS           H::::::HHHHH::::::H    E::::::EEEEEEEEEE     L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::D  SS::::::SSSSS      H:::::::::::::::::H    E:::::::::::::::E     L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::D    SSS::::::::SS    H:::::::::::::::::H    E:::::::::::::::E     L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::D       SSSSSS::::S   H::::::HHHHH::::::H    E::::::EEEEEEEEEE     L:::::L                 L:::::L               \n"
    "  D:::::D     D:::::D            S:::::S  H:::::H     H:::::H    E:::::E               L:::::L                 L:::::L               \n"
    "  D:::::D    D:::::D             S:::::S  H:::::H     H:::::H    E:::::E       EEEEEE  L:::::L         LLLLLL  L:::::L         LLLLLL\n"
    "DDD:::::DDDDD:::::D  SSSSSSS     S:::::SHH::::::H     H::::::HHEE::::::EEEEEEEE:::::ELL:::::::LLLLLLLLL:::::LLL:::::::LLLLLLLLL:::::L\n"
    "D:::::::::::::::DD   S::::::SSSSSS:::::SH:::::::H     H:::::::HE::::::::::::::::::::EL::::::::::::::::::::::LL::::::::::::::::::::::L\n"
    "D::::::::::::DDD     S:::::::::::::::SS H:::::::H     H:::::::HE::::::::::::::::::::EL::::::::::::::::::::::LL::::::::::::::::::::::L\n"
    "DDDDDDDDDDDDD         SSSSSSSSSSSSSSS   HHHHHHHHH     HHHHHHHHHEEEEEEEEEEEEEEEEEEEEEELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n";

  printf("%s%s%s\n", color, dshell_banner, RESET_COLOR);
  fflush(stdout);
}
