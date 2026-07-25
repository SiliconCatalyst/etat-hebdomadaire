#pragma once

#include <OpenXLSX.hpp>
#include <string>
#include <unordered_map>

// Scans headerRow of wks and returns a map of header text -> 1-based column
// index. Lets calculation code look up columns by name instead of relying on
// fixed positions.
inline std::unordered_map<std::string, uint32_t>
mapHeaders(OpenXLSX::XLWorksheet &wks, uint32_t headerRow) {
  std::unordered_map<std::string, uint32_t> headers;

  auto row = wks.row(headerRow);
  uint32_t colCount = row.cellCount();

  for (uint32_t c = 1; c <= colCount; ++c) {
    auto cell = wks.cell(headerRow, c);
    if (cell.value().type() == OpenXLSX::XLValueType::String) {
      std::string name = cell.value().get<std::string>();
      if (!name.empty())
        headers[name] = c;
    }
  }
  return headers;
}

// Writes the numeric value of wks(row, col) to out and returns true,
// or returns false if the cell isn't numeric (e.g. text like "Facture payée",
// or blank).
inline bool cellAsNumber(OpenXLSX::XLWorksheet &wks, uint32_t row, uint32_t col,
                         double &out) {
  auto cell = wks.cell(row, col);
  auto type = cell.value().type();

  if (type == OpenXLSX::XLValueType::Integer ||
      type == OpenXLSX::XLValueType::Float) {
    out = cell.value().get<double>();
    return true;
  }
  return false;
}
