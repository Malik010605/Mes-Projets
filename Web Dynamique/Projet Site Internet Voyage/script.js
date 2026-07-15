document.addEventListener("DOMContentLoaded", function () {
  marquerLienActif();
  verifierFormulairesSimples();
  preparerAffichageMasquage();
  afficherCompteurPanier();
  confirmerReservation();
  confirmerActions();
  animerApparition();
});

function marquerLienActif() {
  const liens = document.querySelectorAll("nav a");
  const pageActuelle = window.location.pathname.split("/").pop();

  liens.forEach(function (lien) {
    const pageLien = lien.getAttribute("href");

    if (pageLien === pageActuelle || (pageActuelle === "" && pageLien === "index.html")) {
      lien.classList.add("active-link");
    }
  });
}

function verifierFormulairesSimples() {
  const formulaires = document.querySelectorAll("form");

  formulaires.forEach(function (formulaire) {
    formulaire.addEventListener("submit", function (event) {
      const champs = formulaire.querySelectorAll("input[required], select[required], textarea[required]");
      let formulaireValide = true;

      champs.forEach(function (champ) {
        if (champ.value === "") {
          formulaireValide = false;
          champ.classList.add("field-error");
        } else {
          champ.classList.remove("field-error");
        }
      });

      if (formulaireValide === false) {
        event.preventDefault();
        alert("Veuillez remplir tous les champs obligatoires.");
      }
    });
  });
}

function preparerAffichageMasquage() {
  const boutons = document.querySelectorAll("[data-target]");

  boutons.forEach(function (bouton) {
    bouton.addEventListener("click", function () {
      const idCible = bouton.getAttribute("data-target");
      const element = document.getElementById(idCible);

      if (element) {
        if (element.style.display === "none") {
          element.style.display = "block";
        } else {
          element.style.display = "none";
        }
      }
    });
  });
}

function afficherCompteurPanier() {
  const compteur = document.getElementById("cartCount");

  if (compteur) {
    const elementsPanier = document.querySelectorAll(".panier-item");
    compteur.textContent = elementsPanier.length;
  }
}

function confirmerReservation() {
  const formulaireReservation = document.getElementById("reservationForm");

  if (formulaireReservation) {
    formulaireReservation.addEventListener("submit", function (event) {
      const confirmation = confirm("Confirmez-vous la validation de ce séjour ?");

      if (confirmation === false) {
        event.preventDefault();
      }
    });
  }
}

function confirmerActions() {
  const boutons = document.querySelectorAll("[data-confirm]");

  boutons.forEach(function (bouton) {
    bouton.addEventListener("click", function (event) {
      const message = bouton.getAttribute("data-confirm");

      if (!confirm(message)) {
        event.preventDefault();
      }
    });
  });
}

function animerApparition() {
  const elements = document.querySelectorAll("main section, article");

  if (!("IntersectionObserver" in window)) {
    elements.forEach(function (element) {
      element.classList.add("visible");
    });
    return;
  }

  const observateur = new IntersectionObserver(function (entrees) {
    entrees.forEach(function (entree) {
      if (entree.isIntersecting) {
        entree.target.classList.add("visible");
        observateur.unobserve(entree.target);
      }
    });
  }, {
    threshold: 0.12
  });

  elements.forEach(function (element) {
    element.classList.add("reveal");
    observateur.observe(element);
  });
}
