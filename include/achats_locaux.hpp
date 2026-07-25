#pragma once
#include <string>

struct AchatsLocauxResult {
  double overdueInvoices = 0.0; // LOCAL_OVERDUE_INVOICES
  double pendingInvoices = 0.0; // LOCAL_PENDING_INVOICES
  double total = 0.0;           // LOCAL_TOTAL
};

// Reads "Suivi Gest. Financier" and "Factures reçues en attente" from the
// uploaded workbook and computes the Achats Locaux totals.
AchatsLocauxResult computeAchatsLocaux(const std::string &xlsmPath);
