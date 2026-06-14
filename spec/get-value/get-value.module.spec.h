#ifndef __WALLAGENT_GET_VALUE_MODULE_SPEC_H_
#define __WALLAGENT_GET_VALUE_MODULE_SPEC_H_

#include "../../libs/cSpec/export/cSpec.h"
#include "../../src/get-value/get-value.h"

module(T_get_value, {
  describe("#get_value", {
    it("returns `Hello, World!`", {
      assert_that_charptr(get_value() equals to "Hello, World!");
    });
  });
})

#endif
