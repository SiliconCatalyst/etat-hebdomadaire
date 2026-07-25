#include "achats_locaux.hpp"
#include "xlsx_utils.hpp"
#include <OpenXLSX.hpp>
#include <stdexcept>

using namespace OpenXLSX;

AchatsLocauxResult computeAchatsLocaux(const std::string &xlsmPath) {
  AchatsLocauxResult result;

  XLDocument doc;
  doc.open(xlsmPath);

  // --- Suivi Gest. Financier ------------------------------------------
  // Header row is row 3 (rows 1-2 hold merged group labels).
  // Reste echeance not numeric (e.g. "Facture payée", blank) -> not a debt,
  // skip. Reste echeance <= 0 -> overdue (Echu). Reste echeance >= 1 -> pending
  // (Non Echu). Either way the amount added is the Montant column.
  {
    XLWorksheet wks = doc.workbook().worksheet("Suivi Gest. Financier");
    auto headers = mapHeaders(wks, 3);

    if (!headers.count("Reste echeance") || !headers.count("Montant"))
      throw std::runtime_error("Suivi Gest. Financier: 'Reste echeance' or "
                               "'Montant' column not found");

    uint32_t resteCol = headers["Reste echeance"];
    uint32_t montantCol = headers["Montant"];
    uint32_t rowCount = wks.rowCount();

    for (uint32_t r = 4; r <= rowCount; ++r) {
      double reste;
      if (!cellAsNumber(wks, r, resteCol, reste))
        continue; // not a debt

      double montant = 0.0;
      cellAsNumber(wks, r, montantCol, montant);

      if (reste <= 0.0)
        result.overdueInvoices += montant;
      else
        result.pendingInvoices += montant;
    }
  }

  // --- Factures reçues en attente --------------------------------------
  // Header row is row 1. Every row with a Montant value counts as pending debt.
  {
    XLWorksheet wks = doc.workbook().worksheet("Factures reçues en attente");
    auto headers = mapHeaders(wks, 1);

    if (!headers.count("Montant"))
      throw std::runtime_error(
          "Factures reçues en attente: 'Montant' column not found");

    uint32_t montantCol = headers["Montant"];
    uint32_t rowCount = wks.rowCount();

    for (uint32_t r = 2; r <= rowCount; ++r) {
      double montant;
      if (!cellAsNumber(wks, r, montantCol, montant))
        continue; // blank row

      result.pendingInvoices += montant;
    }
  }

  doc.close();

  result.total = result.overdueInvoices + result.pendingInvoices;
  return result;
}
