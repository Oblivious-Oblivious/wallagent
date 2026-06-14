#include "../libs/cSpec/export/cSpec.h"
#include "get-value/get-value.module.spec.h"

int main(void) {
  cspec_run_suite("all", {T_get_value();});
}
