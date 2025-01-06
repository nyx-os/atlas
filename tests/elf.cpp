#include <atlas/formats/elf.hpp>
#include <doctest.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

using namespace Atlas;
using namespace Atlas::Formats;

TEST_SUITE("Elf file") {
  const char *file_path = "tests/test.elf";

  // Read file in buffer
  std::ifstream file(file_path, std::ios::binary);
  std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});

  Slice<uint8_t> file_data(buffer.data(), buffer.size());

  auto elf = Elf::File::deserialize(file_data).unwrap();

  TEST_CASE("machine") { CHECK(elf.header().machine == Elf::Machine::Amd64); }
  TEST_CASE("entry") { CHECK(elf.header().entry == 0x401000); }
  TEST_CASE("type") { CHECK(elf.header().type == Elf::Type::Executable); }
  TEST_CASE("phnum") { CHECK(elf.header().phnum == 3); }

  TEST_CASE("phdrs") {
    elf.phdrs().for_each([](auto phdr) {
      CHECK(phdr.type == Elf::ProgramHeader::Type::Load);
      CHECK(phdr.flags.read == true);
    });
  }

  TEST_CASE("shnum") { CHECK(elf.header().shnum == 6); }

  TEST_CASE("sections") {
    Array<StringView, 6> expected_names{"",        ".text",   ".rodata",
                                        ".symtab", ".strtab", ".shstrtab"};

    Array<Elf::SectionHeader::Type, 6> expected_types{
        Elf::SectionHeader::Type::Null,     Elf::SectionHeader::Type::Progbits,
        Elf::SectionHeader::Type::Progbits, Elf::SectionHeader::Type::Symtab,
        Elf::SectionHeader::Type::Strtab,   Elf::SectionHeader::Type::Strtab};

    elf.sections().enumerate().for_each(
        [&](Cons<size_t, Elf::SectionHeader> a) {
          auto [i, shdr] = a;
          CHECK(elf.get_string(shdr.name, elf.header().shstrndx) ==
                expected_names[i]);
          CHECK(shdr.type == expected_types[i]);
        });
  }

  TEST_CASE("symbols") {
    auto symtab = elf.sections().find([](auto shdr) {
      return shdr.type == Elf::SectionHeader::Type::Symtab;
    });

    CHECK(symtab.is_some());

    Array<StringView, 8> expected_names{"",       "test.asm", "msg",
                                        "msglen", "_start",   "__bss_start",
                                        "_edata", "_end"};

    elf.symbols(*symtab).enumerate().for_each([&](Cons<size_t, Elf::Symbol> a) {
      auto [i, sym] = a;
      CHECK(elf.get_string(sym.name, symtab->link) == expected_names[i]);
    });
  }
}