#include "../../libs/cSpec/export/cSpec.h"
#include "../../src/z/init.h"

module(T_Z, {
  describe("#init", {
    it("returns `Wall Agent`", {
      assert_that_charptr(init() equals to "Wall Agent");
    });
  });
})
