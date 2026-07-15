"""
SCRAPER LOOPNET - VERSION ANTI-CLOUDFLARE
Utilise playwright-stealth pour contourner la détection Cloudflare
"""

import asyncio
from playwright.async_api import async_playwright, TimeoutError as PlaywrightTimeout
from bs4 import BeautifulSoup
import re
import pandas as pd
from datetime import datetime
import os
import csv
import math
import random
import logging
from typing import List, Dict, Optional

# Configuration du logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('loopnet_scraper.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Configuration
BASE_URL = "https://www.loopnet.com"
SEARCH_PATH = "/search/commercial-real-estate/france/for-sale/"
RESULTS_PER_PAGE = 20
MAX_PAGES = 10  # Limiter pour éviter le bannissement
PAGE_LOAD_TIMEOUT = 60000
RESULTS_FOLDER = "resultats"

# LOOPNET CREDENTIALS (optionnel - peut aider mais pas obligatoire)
LOOPNET_EMAIL = "arslane.saadi183@gmail.com"  # Laisser vide pour scraper sans connexion
LOOPNET_PASSWORD = "PlaceToBeAI1"
USE_LOGIN = True  # Mettre True si vous avez des identifiants

# IMPORTANT: Installation requise
# npm install playwright-extra playwright-extra-plugin-stealth
# Ou utiliser le mode manuel (ouvrir le navigateur vous-même)
USE_STEALTH_MODE = False  # Mettre True si vous avez installé playwright-extra


class LoopNetScraper:
    def __init__(self):
        self.all_data = []
        self.browser = None
        self.context = None
        self.page = None

    async def setup_browser_manual(self):
        """
        Mode MANUEL - Le navigateur s'ouvre et VOUS devez:
        1. Résoudre le challenge Cloudflare manuellement
        2. Naviguer vers la page de recherche
        3. Le script continue automatiquement après
        """
        try:
            logger.info("🚀 MODE MANUEL ACTIVÉ")
            logger.info("=" * 70)
            logger.info("INSTRUCTIONS:")
            logger.info("1. Le navigateur va s'ouvrir")
            logger.info("2. Résolvez le challenge Cloudflare si présent")
            logger.info("3. Attendez que la page de résultats charge")
            logger.info("4. Le scraping commencera automatiquement après 15 secondes")
            logger.info("=" * 70)

            p = await async_playwright().start()

            # Utiliser un profil persistant pour garder les cookies
            user_data_dir = os.path.join(os.getcwd(), "browser_data")

            self.browser = await p.chromium.launch_persistent_context(
                user_data_dir,
                headless=False,
                args=[
                    '--disable-blink-features=AutomationControlled',
                    '--disable-dev-shm-usage',
                    '--no-sandbox',
                    '--disable-web-security',
                ],
                viewport={'width': 1920, 'height': 1080},
                user_agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
                locale='en-US',
                timezone_id='America/New_York'
            )

            self.page = self.browser.pages[0] if self.browser.pages else await self.browser.new_page()

            # Masquer webdriver
            await self.page.add_init_script("""
                Object.defineProperty(navigator, 'webdriver', {
                    get: () => undefined
                });

                window.chrome = {
                    runtime: {}
                };

                Object.defineProperty(navigator, 'plugins', {
                    get: () => [1, 2, 3, 4, 5]
                });
            """)

            logger.info("✓ Navigateur configuré en mode manuel")

        except Exception as e:
            logger.error(f"Erreur lors de la configuration du navigateur: {e}")
            raise

    async def wait_for_manual_resolution(self, target_url: str):
        """Attend que l'utilisateur résolve Cloudflare manuellement"""
        try:
            logger.info(f"Navigation vers: {target_url}")
            await self.page.goto(target_url, wait_until='domcontentloaded', timeout=PAGE_LOAD_TIMEOUT)

            # Attendre 15 secondes pour que l'utilisateur résolve le challenge
            logger.info("⏳ Attente de 15 secondes pour résolution manuelle du challenge...")
            logger.info("(Si vous voyez les résultats, tout est OK!)")

            for i in range(15, 0, -1):
                await asyncio.sleep(1)
                logger.info(f"   {i} secondes restantes...")

            # Vérifier si on est sur une page de résultats
            content = await self.page.content()
            if 'access denied' in content.lower() or 'reference #' in content.lower():
                logger.error("❌ Toujours bloqué par Cloudflare")
                logger.error("Veuillez résoudre le challenge dans le navigateur qui est ouvert")
                logger.info("Appuyez sur CTRL+C pour arrêter ou attendez...")
                await asyncio.sleep(30)  # Donner plus de temps
                return False

            logger.info("✅ Page chargée avec succès!")
            return True

        except Exception as e:
            logger.error(f"Erreur: {e}")
            return False

    async def handle_cookies(self):
        """Gère les popups de cookies"""
        try:
            await self.page.wait_for_selector('button#onetrust-accept-btn-handler', timeout=5000)
            await self.page.click('button#onetrust-accept-btn-handler')
            logger.info("Cookie banner accepté")
            await asyncio.sleep(1)
        except:
            pass

    async def get_total_pages(self) -> int:
        """Récupère le nombre total de pages de résultats"""
        try:
            await self.page.wait_for_selector('.result-count, .total-results-digits, .results-count', timeout=10000)
            await asyncio.sleep(2)

            content = await self.page.content()
            soup = BeautifulSoup(content, 'html.parser')

            selectors = [
                'span.total-results-digits',
                'span.result-count',
                'div.results-count',
            ]

            total_listings = None
            for selector in selectors:
                tag = soup.select_one(selector)
                if tag:
                    text = tag.get_text(strip=True)
                    numbers = re.sub(r'[^\d]', '', text)
                    if numbers:
                        total_listings = int(numbers)
                        break

            if total_listings:
                total_pages = math.ceil(total_listings / RESULTS_PER_PAGE)
                logger.info(f"Total annonces: {total_listings} | Pages: {total_pages}")
                return min(total_pages, MAX_PAGES)

            return 1

        except Exception as e:
            logger.error(f"Erreur récupération nombre de pages: {e}")
            return 1

    def parse_price(self, price_text: str) -> Optional[int]:
        """Parse le prix"""
        try:
            price_cleaned = re.sub(r'[^\d]', '', price_text)
            return int(price_cleaned) if price_cleaned else None
        except:
            return None

    def parse_area(self, area_text: str) -> Optional[int]:
        """Parse la superficie"""
        try:
            area_match = re.search(r'([\d,\s]+)', area_text)
            if area_match:
                area_str = area_match.group(1).replace(',', '').replace(' ', '').replace('\u202f', '')
                if area_str.isdigit():
                    if 'sf' in area_text.lower() or 'sq' in area_text.lower():
                        return int(int(area_str) * 0.092903)
                    return int(area_str)
            return None
        except:
            return None

    async def scrape_current_page(self) -> List[Dict]:
        """Scrape les annonces de la page actuelle"""
        listings = []

        try:
            await self.page.wait_for_selector('article.placard, div.property-card, [data-automation-id="PropertyCard"]',
                                              timeout=15000)

            # Scroll progressif
            await self.page.evaluate("window.scrollTo(0, document.body.scrollHeight / 2)")
            await asyncio.sleep(random.uniform(1, 2))
            await self.page.evaluate("window.scrollTo(0, document.body.scrollHeight)")
            await asyncio.sleep(random.uniform(1, 2))

            content = await self.page.content()
            soup = BeautifulSoup(content, 'html.parser')

            cards = soup.find_all('article', class_='placard')
            if not cards:
                cards = soup.find_all('div', class_='property-card')
            if not cards:
                cards = soup.find_all(attrs={'data-automation-id': 'PropertyCard'})

            logger.info(f"Cartes trouvées: {len(cards)}")

            for idx, card in enumerate(cards):
                try:
                    listing = self.parse_listing_card(card)
                    if listing and listing.get('url'):
                        listings.append(listing)
                except Exception as e:
                    logger.warning(f"Erreur parsing carte {idx + 1}: {e}")
                    continue

        except PlaywrightTimeout:
            logger.error("Timeout chargement des annonces")
            await self.page.screenshot(path=f"timeout_{datetime.now().strftime('%Y%m%d_%H%M%S')}.png")
        except Exception as e:
            logger.error(f"Erreur scraping page: {e}")

        return listings

    def parse_listing_card(self, card) -> Optional[Dict]:
        """Parse une carte d'annonce"""
        try:
            title_tag = card.find('h4') or card.find('h3') or card.find('a', class_='property-title')
            if not title_tag:
                return None

            link_tag = title_tag.find('a') if title_tag.name != 'a' else title_tag
            if not link_tag:
                return None

            url = link_tag.get('href', '')
            if url and not url.startswith('http'):
                url = BASE_URL + url

            title = link_tag.get_text(strip=True)

            # Prix
            price = None
            price_tag = card.find('li', attrs={'name': 'Price'}) or card.find(class_='price')
            if price_tag:
                price = self.parse_price(price_tag.get_text())

            # Superficie
            area = None
            area_tag = card.find('li', attrs={'name': 'Area'})
            if not area_tag:
                data_points = card.find('ul', class_='data-points-2c')
                if data_points:
                    for li in data_points.find_all('li'):
                        text = li.get_text()
                        if 'sf' in text.lower() or 'm²' in text.lower() or 'sq' in text.lower():
                            area = self.parse_area(text)
                            break
            else:
                area = self.parse_area(area_tag.get_text())

            # Localisation
            location = None
            location_tag = card.find('div', class_='subtitle-beta') or card.find(class_='property-address')
            if location_tag:
                location = location_tag.get_text(strip=True)
            else:
                header = card.find('header')
                if header:
                    divs = header.find_all('div')
                    if divs:
                        location = divs[-1].get_text(strip=True)

            # Type de propriété
            property_type = None
            type_tag = card.find('li', attrs={'name': 'PropertyType'})
            if type_tag:
                property_type = type_tag.get_text(strip=True)

            return {
                'titre': title,
                'prix': price,
                'superficie_m2': area,
                'localisation': location,
                'type_propriete': property_type,
                'url': url,
                'source': 'LoopNet',
                'date_scraping': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            }

        except Exception as e:
            return None

    async def navigate_to_next_page(self) -> bool:
        """Navigue vers la page suivante"""
        try:
            next_selectors = [
                "a[data-automation-id='NextPage']",
                "a.next-page",
                "a[aria-label='Next Page']",
            ]

            for selector in next_selectors:
                try:
                    next_button = self.page.locator(selector).first
                    if await next_button.count() > 0:
                        is_disabled = await next_button.get_attribute('disabled')
                        if is_disabled:
                            return False

                        # Scroll vers le bouton
                        await next_button.scroll_into_view_if_needed()
                        await asyncio.sleep(1)

                        href = await next_button.get_attribute('href')
                        if href:
                            if not href.startswith('http'):
                                href = BASE_URL + href
                            await self.page.goto(href, wait_until='domcontentloaded', timeout=PAGE_LOAD_TIMEOUT)
                        else:
                            await next_button.click()
                            await self.page.wait_for_load_state('networkidle', timeout=PAGE_LOAD_TIMEOUT)

                        # Délai humain
                        await asyncio.sleep(random.uniform(4, 7))
                        return True

                except:
                    continue

            return False

        except Exception as e:
            logger.error(f"Erreur navigation page suivante: {e}")
            return False

    async def run(self):
        """Lance le scraping complet"""
        try:
            logger.info("=== SCRAPER LOOPNET - MODE ANTI-CLOUDFLARE ===")
            logger.info("")

            # Setup en mode manuel
            await self.setup_browser_manual()

            # Navigation et attente résolution manuelle
            start_url = f"{BASE_URL}{SEARCH_PATH}?sk=fa32b6e2c97f8b3ef7089fe8a9e71a05&view=map"
            success = await self.wait_for_manual_resolution(start_url)

            if not success:
                logger.error("Impossible d'accéder à la page")
                return

            # Gérer cookies
            await self.handle_cookies()

            # Obtenir nombre de pages
            total_pages = await self.get_total_pages()

            # Scraper chaque page
            current_page = 1
            while current_page <= total_pages:
                logger.info(f"--- Page {current_page}/{total_pages} ---")

                page_listings = await self.scrape_current_page()
                self.all_data.extend(page_listings)
                logger.info(f"✓ {len(page_listings)} annonces récupérées (Total: {len(self.all_data)})")

                if current_page < total_pages:
                    success = await self.navigate_to_next_page()
                    if not success:
                        logger.info("Fin de pagination")
                        break
                    current_page += 1
                else:
                    break

            # Sauvegarder
            self.save_results()

            logger.info("\n✅ SCRAPING TERMINÉ - Vous pouvez fermer le navigateur")
            logger.info("Appuyez sur CTRL+C ou fermez cette fenêtre")
            await asyncio.sleep(10)  # Garder ouvert 10s

        except Exception as e:
            logger.error(f"Erreur critique: {e}")
            if self.page:
                await self.page.screenshot(path=f"error_{datetime.now().strftime('%Y%m%d_%H%M%S')}.png")

        finally:
            if self.browser:
                await self.browser.close()

    def save_results(self):
        """Sauvegarde les résultats"""
        try:
            if not os.path.exists(RESULTS_FOLDER):
                os.makedirs(RESULTS_FOLDER)

            if not self.all_data:
                logger.warning("Aucune donnée à sauvegarder")
                return

            timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
            filename = os.path.join(RESULTS_FOLDER, f"loopnet_france_{timestamp}.csv")

            df = pd.DataFrame(self.all_data)

            columns_order = ['titre', 'prix', 'superficie_m2', 'type_propriete', 'localisation', 'url', 'source',
                             'date_scraping']
            df = df[[col for col in columns_order if col in df.columns]]

            df.to_csv(filename, index=False, encoding='utf-8-sig', quoting=csv.QUOTE_ALL)

            logger.info(f"\n{'=' * 70}")
            logger.info(f"✅ {len(self.all_data)} annonces sauvegardées dans '{filename}'")
            if 'prix' in df.columns and df['prix'].notna().any():
                logger.info(f"💰 Prix moyen: {df['prix'].mean():,.0f} €")
            if 'superficie_m2' in df.columns and df['superficie_m2'].notna().any():
                logger.info(f"📐 Superficie moyenne: {df['superficie_m2'].mean():.0f} m²")
            logger.info(f"{'=' * 70}")

        except Exception as e:
            logger.error(f"Erreur sauvegarde: {e}")


async def main():
    scraper = LoopNetScraper()
    await scraper.run()


if __name__ == '__main__':
    print("""
╔══════════════════════════════════════════════════════════════╗
║         SCRAPER LOOPNET - MODE MANUEL ANTI-CLOUDFLARE        ║
╚══════════════════════════════════════════════════════════════╝

Ce scraper utilise le MODE MANUEL pour contourner Cloudflare:

1. Un navigateur Chrome va s'ouvrir automatiquement
2. Vous devez résoudre le challenge Cloudflare (si présent)
3. Attendez 15 secondes que le script détecte la résolution
4. Le scraping commence automatiquement

ASTUCE: Le navigateur garde les cookies entre les sessions.
Après la première fois, Cloudflare vous bloquera moins souvent!

═══════════════════════════════════════════════════════════════
Appuyez sur ENTRÉE pour commencer...
""")
    input()
    asyncio.run(main())
