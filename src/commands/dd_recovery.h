#pragma once

#include <cstddef>
#include <vector>

namespace dd_pipeline {

enum class ReadBlockAction { success, recovered, stop };

// Keeps noerror recovery deterministic and independently testable.
template <typename ReadFn, typename SeekFn>
ReadBlockAction recover_read_block(ReadFn&& read, SeekFn&& seek, bool noerror,
                                   bool sync_blocks, std::size_t request,
                                   std::vector<char>& output_buffer,
                                   std::size_t& input_records,
                                   std::vector<char>& input_buffer,
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
