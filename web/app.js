// ---------------------------------------------------------------------------
// DOM refs
// ---------------------------------------------------------------------------
const fileInput = document.getElementById("file");
const calculateBtn = document.getElementById("calculateBtn");
const resultsCard = document.getElementById("resultsCard");
const errorMsg = document.getElementById("errorMsg");

const kpiOverdue = document.getElementById("kpiOverdue");
const kpiPending = document.getElementById("kpiPending");
const kpiTotal = document.getElementById("kpiTotal");

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
function showError(msg) {
  errorMsg.textContent = msg;
  errorMsg.classList.remove("hidden");
}

function clearError() {
  errorMsg.textContent = "";
  errorMsg.classList.add("hidden");
}

function formatAmount(n) {
  return Number(n).toLocaleString("fr-FR", { maximumFractionDigits: 2 });
}

// ---------------------------------------------------------------------------
// Calculate: POST the file, then GET the computed result
// ---------------------------------------------------------------------------
calculateBtn.addEventListener("click", async () => {
  clearError();

  if (!fileInput.files[0]) {
    showError("Veuillez sélectionner un fichier Excel.");
    return;
  }

  calculateBtn.disabled = true;
  calculateBtn.textContent = "Calcul en cours…";

  const form = new FormData();
  form.append("file", fileInput.files[0]);

  try {
    const uploadRes = await fetch("/api/achats-locaux/upload", {
      method: "POST",
      body: form,
    });
    const uploadData = await uploadRes.json();

    if (!uploadRes.ok || uploadData.error) {
      showError(uploadData.error ?? "Le calcul a échoué.");
      return;
    }

    const resultRes = await fetch("/api/achats-locaux");
    const result = await resultRes.json();

    if (!resultRes.ok || result.error) {
      showError(result.error ?? "Impossible de récupérer le résultat.");
      return;
    }

    kpiOverdue.textContent = formatAmount(result.LOCAL_OVERDUE_INVOICES);
    kpiPending.textContent = formatAmount(result.LOCAL_PENDING_INVOICES);
    kpiTotal.textContent = formatAmount(result.LOCAL_TOTAL);

    resultsCard.classList.remove("hidden");
  } catch (err) {
    showError("Impossible de contacter le serveur local.");
  } finally {
    calculateBtn.disabled = false;
    calculateBtn.textContent = "Calculer";
  }
});
