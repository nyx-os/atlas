#include <atlas/result.hpp>
#include <doctest.h>

using namespace Atlas;

TEST_SUITE("Result") {
  TEST_CASE("Ok") {
    Result<int> some = Ok(1);
    CHECK(some.is_ok());
    CHECK(some.unwrap() == 1);
  }

  TEST_CASE("Err") {
    Result<int> some = Err(Error::Empty);

    CHECK_FALSE(some.is_ok());
    CHECK(some.error() == Error::Empty);
    CHECK(some.unwrap_or(1) == 1);
  }

  TEST_CASE("unwrap_or") {
    Result<int> some = Ok(1);
    CHECK(some.unwrap_or(2) == 1);
    Result<int> none = Err(Error::Empty);
    CHECK(none.unwrap_or(2) == 2);
  }

  TEST_CASE("unwrapping none") {
    Result<int> some = Ok(1);
    CHECK(some.unwrap() == 1);

    Result<int> none = Err(Error::Empty);
    CHECK_THROWS(none.unwrap());
  }

  TEST_CASE("error on ok") {
    Result<int> some = Ok(1);
    CHECK_THROWS(some.error());
  }
}
