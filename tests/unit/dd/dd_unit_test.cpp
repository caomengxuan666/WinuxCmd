
#include <functional>
#include "framework/winuxtest.h"

namespace dd_pipeline {
enum class ReadBlockAction { success, recovered, stop };
ReadBlockAction recover_read_block(
    std::function<bool(char*, std::size_t, std::size_t&)> read,
    std::function<bool(std::size_t)> seek, bool noerror, bool sync_blocks,
    std::size_t request, std::vector<char>& output_buffer,
    std::size_t& input_records, std::vector<char>& input_buffer,
    std::size_t& bytes_read) {
  bytes_read = 0;
  if (read(input_buffer.data(), request, bytes_read)) {
    return ReadBlockAction::success;
  }
  if (!noerror) return ReadBlockAction::stop;

  const bool can_continue = seek(request);
  if (sync_blocks) output_buffer.insert(output_buffer.end(), request, '\0');
  ++input_records;
  return can_continue ? ReadBlockAction::recovered : ReadBlockAction::stop;
}
}  // namespace dd_pipeline

TEST(dd, dd_copies_with_block_size_and_count) {
  TempDir tmp;
  tmp.write("input.bin", "abcdefghijkl");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=4", L"count=2"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), "abcdefgh");
  EXPECT_TRUE(r.stderr_text.find("2 records in") != std::string::npos);
}

TEST(dd, dd_conv_sync_pads_short_input_blocks) {
  TempDir tmp;
  tmp.write("input.bin", "abc");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe",
        {L"if=input.bin", L"of=out.bin", L"bs=4", L"count=1", L"conv=sync"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), std::string("abc\0", 4));
  EXPECT_TRUE(r.stderr_text.find("4 bytes copied") != std::string::npos);
}

TEST(dd, dd_skip_uses_input_block_size) {
  TempDir tmp;
  tmp.write("input.bin", "abcdefghi");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"ibs=3", L"obs=2",
                    L"skip=1", L"count=1"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), "def");
}

TEST(dd, dd_zero_count_skip_and_seek_are_allowed) {
  TempDir tmp;
  tmp.write("input.bin", "abcdef");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=2", L"cbs=4",
                    L"count=0", L"skip=0", L"seek=0"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), "");
  EXPECT_TRUE(r.stderr_text.find("0 records in") != std::string::npos);
  EXPECT_TRUE(r.stderr_text.find("0 records out") != std::string::npos);
}

TEST(dd, dd_seek_and_notrunc_preserve_existing_bytes) {
  TempDir tmp;
  tmp.write("input.bin", "zz");
  tmp.write("out.bin", "ABCDEFGH");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe",
        {L"if=input.bin", L"of=out.bin", L"obs=4", L"seek=1", L"conv=notrunc"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), "ABCDzzGH");
}

TEST(dd, dd_status_none_suppresses_diagnostics) {
  TempDir tmp;
  tmp.write("input.bin", "abc");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=1", L"status=none"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stderr_text.empty());
  EXPECT_EQ(tmp.read("out.bin"), "abc");
}

TEST(dd, dd_status_noxfer_omits_bytes_copied_line) {
  TempDir tmp;
  tmp.write("input.bin", "abc");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=1", L"status=noxfer"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stderr_text.find("records in") != std::string::npos);
  EXPECT_TRUE(r.stderr_text.find("records out") != std::string::npos);
  EXPECT_TRUE(r.stderr_text.find("bytes copied") == std::string::npos);
  EXPECT_EQ(tmp.read("out.bin"), "abc");
}

TEST(dd, dd_rejects_invalid_size_operand) {
  TempDir tmp;
  tmp.write("input.bin", "abc");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=bad"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_TRUE(r.stderr_text.find("invalid bs value") != std::string::npos);
}

TEST(dd, dd_conv_noerror_is_accepted_with_sync) {
  TempDir tmp;
  tmp.write("input.bin", "abc");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"dd.exe", {L"if=input.bin", L"of=out.bin", L"bs=4", L"count=1",
                    L"conv=noerror,sync"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(tmp.read("out.bin"), std::string("abc\0", 4));
}

TEST(dd, noerror_recovery_skips_failed_block_and_syncs) {
  std::vector<char> input(4);
  std::vector<char> output;
  std::size_t records = 0;
  std::size_t bytes_read = 0;
  bool seek_called = false;

  const auto action = dd_pipeline::recover_read_block(
      [](char*, std::size_t, std::size_t&) { return false; },
      [&](std::size_t amount) {
        seek_called = amount == 4;
        return true;
      },
      true, true, 4, output, records, input, bytes_read);

  EXPECT_EQ(static_cast<int>(action),
            static_cast<int>(dd_pipeline::ReadBlockAction::recovered));
  EXPECT_TRUE(seek_called);
  EXPECT_EQ(records, 1u);
  ASSERT_EQ(output.size(), 4u);
  EXPECT_TRUE(std::all_of(output.begin(), output.end(),
                          [](char value) { return value == '\0'; }));
  EXPECT_EQ(bytes_read, 0u);
}
