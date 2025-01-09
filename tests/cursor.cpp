#include <atlas/array.hpp>
#include <atlas/io/cursor.hpp>
#include <atlas/vec.hpp>
#include <doctest.h>

using namespace atlas;
using namespace atlas::io;

TEST_SUITE("cursor") {
  Vec<uint8_t> vector{1, 2, 3};
  Cursor cursor{vector};

  TEST_CASE("read") {
    Array<uint8_t, 3> buf{};

    auto result = cursor.read(buf.as_slice());
    CHECK(result.is_ok());
    CHECK(result.unwrap() == 3);
    CHECK(buf[0] == 1);
    CHECK(buf[1] == 2);
    CHECK(buf[2] == 3);
  }

  TEST_CASE("read larger than byte") {
    CHECK(cursor.seek(SeekFrom::start(0)).is_ok());

    auto result = cursor.read<uint16_t, Endianness::Little>();

    CHECK(result.is_ok());
    CHECK(result.unwrap() == 0x0201);

    CHECK(cursor.seek(SeekFrom::start(0)).is_ok());

    auto res2 = cursor.read<uint16_t, Endianness::Big>();

    CHECK(res2.is_ok());
    CHECK(res2.unwrap() == 0x0102);
  }

  TEST_CASE("seek") {
    auto result = cursor.seek(SeekFrom::start(1));
    CHECK(result.is_ok());
    CHECK(result.unwrap() == 1);

    Array<uint8_t, 2> buf{};
    auto read_result = cursor.read(buf.as_slice());

    CHECK(read_result.is_ok());
    CHECK(read_result.unwrap() == 2);
    CHECK(buf[0] == 2);
    CHECK(buf[1] == 3);

    CHECK(cursor.seek(SeekFrom::start(0)).is_ok());
  }

  TEST_CASE("write") {
    Array<uint8_t, 3> buf{4, 5, 6};
    auto result = cursor.write(buf.as_slice());

    CHECK(result.is_ok());
    CHECK(result.unwrap() == 3);

    CHECK(vector[0] == 4);
    CHECK(vector[1] == 5);
    CHECK(vector[2] == 6);
  }

  TEST_CASE("write larger than byte") {
    CHECK(cursor.seek(SeekFrom::start(0)).is_ok());

    auto result = cursor.write<uint16_t, Endianness::Little>(0x0405);

    CHECK(result.is_ok());
    CHECK(result.unwrap() == 2);

    CHECK(vector[0] == 5);
    CHECK(vector[1] == 4);

    CHECK(cursor.seek(SeekFrom::start(0)).is_ok());

    auto res2 = cursor.write<uint16_t, Endianness::Big>(0x0102);

    CHECK(res2.is_ok());
    CHECK(res2.unwrap() == 2);

    CHECK(vector[0] == 1);
    CHECK(vector[1] == 2);
  }
}