#include "../libs/cSpec/export/cSpec.h"
#include "z/init.module.spec.h"

int main(void) {
  cspec_run_suite("all", { T_Z(); });
}
