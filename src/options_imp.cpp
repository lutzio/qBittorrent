/*
 * Bittorrent Client using Qt4 and libtorrent.
 * Copyright (C) 2006  Christophe Dumez
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Contact : chris@qbittorrent.org
 */
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QSettings>
#include <QPlastiqueStyle>
#include "qgnomelook.h"
#include <QMotifStyle>
#include <QCDEStyle>
#include <QDialogButtonBox>
#include <QCloseEvent>
#ifdef Q_WS_WIN
  #include <QWindowsXPStyle>
#endif

#ifdef Q_WS_MAC
  #include <QMacStyle>
#endif

// P2B Stuff
#include <arpa/inet.h>
// End of P2B stuff

#include "options_imp.h"
#include "misc.h"

// Constructor
options_imp::options_imp(QWidget *parent):QDialog(parent){
  qDebug("-> Constructing Options");
  QString savePath;
  setupUi(this);
  // Get apply button in button box
  QList<QAbstractButton *> buttons = buttonBox->buttons();
  QAbstractButton *button;
  foreach(button, buttons){
    if(buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole){
      applyButton = button;
      break;
    }
  }
  connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(applySettings(QAbstractButton*)));
#ifdef Q_WS_WIN
  comboStyle->addItem("Windows XP Style (Windows Only)");
#endif
#ifdef Q_WS_MAC
  comboStyle->addItem("MacOS Style (MacOS Only)");
#endif
  // Languages supported
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/united_kingdom.png"))), QString::fromUtf8("English"));
  locales << "en_GB";
  comboI18n->setCurrentIndex(0);
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/france.png"))), QString::fromUtf8("Français"));
  locales << "fr_FR";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/germany.png"))), QString::fromUtf8("Deutsch"));
  locales << "de_DE";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/hungary.png"))), QString::fromUtf8("Magyar"));
  locales << "hu_HU";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/italy.png"))), QString::fromUtf8("Italiano"));
  locales << "it_IT";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/netherlands.png"))), QString::fromUtf8("Nederlands"));
  locales << "nl_NL";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/spain.png"))), QString::fromUtf8("Español"));
  locales << "es_ES";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/spain_catalunya.png"))), QString::fromUtf8("Català"));
  locales << "ca_ES";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/portugal.png"))), QString::fromUtf8("Português"));
  locales << "pt_PT";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/brazil.png"))), QString::fromUtf8("Português brasileiro"));
  locales << "pt_BR";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/poland.png"))), QString::fromUtf8("Polski"));
  locales << "pl_PL";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/slovakia.png"))), QString::fromUtf8("Slovenčina"));
  locales << "sk_SK";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/romania.png"))), QString::fromUtf8("Română"));
  locales << "ro_RO";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/turkey.png"))), QString::fromUtf8("Türkçe"));
  locales << "tr_TR";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/greece.png"))), QString::fromUtf8("Ελληνικά"));
  locales << "el_GR";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/sweden.png"))), QString::fromUtf8("Svenska"));
  locales << "sv_SE";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/finland.png"))), QString::fromUtf8("Suomi"));
  locales << "fi_FI";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/norway.png"))), QString::fromUtf8("Norsk"));
  locales << "nb_NO";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/denmark.png"))), QString::fromUtf8("Dansk"));
  locales << "da_DK";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/bulgaria.png"))), QString::fromUtf8("Български"));
  locales << "bg_BG";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/ukraine.png"))), QString::fromUtf8("Українська"));
  locales << "uk_UA";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/russia.png"))), QString::fromUtf8("Русский"));
  locales << "ru_RU";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/japan.png"))), QString::fromUtf8("日本語"));
  locales << "ja_JP";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/china.png"))), QString::fromUtf8("中文 (简体)"));
  locales << "zh_CN";
  comboI18n->addItem((QIcon(QString::fromUtf8(":/Icons/flags/south_korea.png"))), QString::fromUtf8("한글"));
  locales << "ko_KR";

  // Load options
  loadOptions();
  // Disable systray integration if it is not supported by the system
  if(!QSystemTrayIcon::isSystemTrayAvailable()){
    checkNoSystray->setEnabled(false);
  }
  // Connect signals / slots
  // General tab
  connect(checkNoSystray, SIGNAL(stateChanged(int)), this, SLOT(setSystrayOptionsState(int)));
  // Downloads tab
  connect(checkScanDir, SIGNAL(stateChanged(int)), this, SLOT(enableDirScan(int)));
  connect(actionTorrentDlOnDblClBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  connect(actionTorrentFnOnDblClBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  // Connection tab
  connect(checkUploadLimit, SIGNAL(stateChanged(int)), this, SLOT(enableUploadLimit(int)));
  connect(checkDownloadLimit,  SIGNAL(stateChanged(int)), this, SLOT(enableDownloadLimit(int)));
  connect(comboProxyType, SIGNAL(currentIndexChanged(int)),this, SLOT(enableProxy(int)));
  connect(checkProxyAuth,  SIGNAL(stateChanged(int)), this, SLOT(enableProxyAuth(int)));
  // Bittorrent tab
  connect(checkMaxConnecs,  SIGNAL(stateChanged(int)), this, SLOT(enableMaxConnecsLimit(int)));
  connect(checkMaxConnecsPerTorrent,  SIGNAL(stateChanged(int)), this, SLOT(enableMaxConnecsLimitPerTorrent(int)));
  connect(checkMaxUploadsPerTorrent,  SIGNAL(stateChanged(int)), this, SLOT(enableMaxUploadsLimitPerTorrent(int)));
  connect(checkRatioLimit,  SIGNAL(stateChanged(int)), this, SLOT(enableShareRatio(int)));
  connect(checkRatioRemove,  SIGNAL(stateChanged(int)), this, SLOT(enableDeleteRatio(int)));
  // IP Filter tab
  connect(checkIPFilter,  SIGNAL(stateChanged(int)), this, SLOT(enableFilter(int)));
  // Web UI tab
  connect(checkWebUi,  SIGNAL(toggled(bool)), this, SLOT(enableWebUi(bool)));

  // Apply button is activated when a value is changed
  // General tab
  connect(comboI18n, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  connect(comboStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkConfirmExit, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkSpeedInTitle, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(spinRefreshInterval, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkNoSystray, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkCloseToSystray, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkMinimizeToSysTray, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkStartMinimized, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkSystrayBalloons, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  // Downloads tab
  connect(textSavePath, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkPreallocateAll, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkAdditionDialog, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkStartPaused, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkScanDir, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(textScanDir, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  // Connection tab
  connect(spinPortMin, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinPortMax, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkUPnP, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkNATPMP, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkUploadLimit, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkDownloadLimit, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(spinUploadLimit, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinDownloadLimit, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(comboProxyType, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  connect(textProxyIP, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinProxyPort, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkProxyAuth, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(textProxyUsername, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(textProxyPassword, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkProxyTrackers, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkProxyPeers, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkProxyWebseeds, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkProxyDHT, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  // Bittorrent tab
  connect(checkMaxConnecs, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkMaxConnecsPerTorrent, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkMaxUploadsPerTorrent, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(spinMaxConnec, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinMaxConnecPerTorrent, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinMaxUploadsPerTorrent, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(checkDHT, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkPeX, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkLSD, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(comboEncryption, SIGNAL(currentIndexChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkRatioLimit, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(checkRatioRemove, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(spinRatio, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinMaxRatio, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  // Misc tab
  connect(checkIPFilter, SIGNAL(stateChanged(int)), this, SLOT(enableApplyButton()));
  connect(textFilterPath, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinRSSRefresh, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  connect(spinRSSMaxArticlesPerFeed, SIGNAL(valueChanged(QString)), this, SLOT(enableApplyButton()));
  // Web UI tab
  connect(checkWebUi, SIGNAL(toggled(bool)), this, SLOT(enableApplyButton()));
  connect(spinWebUiPort, SIGNAL(valueChanged(int)), this, SLOT(enableApplyButton()));
  connect(textWebUiUsername, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  connect(textWebUiPassword, SIGNAL(textChanged(QString)), this, SLOT(enableApplyButton()));
  // Disable apply Button
  applyButton->setEnabled(false);
  if(!QSystemTrayIcon::supportsMessages()){
    // Mac OS X doesn't support it yet
    checkSystrayBalloons->setChecked(false);
    checkSystrayBalloons->setEnabled(false);
  }
}

// Main destructor
options_imp::~options_imp(){
  qDebug("-> destructing Options");
}

void options_imp::useStyle(){
  int style = getStyle();
  switch(style) {
    case 1:
      QApplication::setStyle(new QPlastiqueStyle());
      break;
    case 2:
      QApplication::setStyle(new QGnomeLookStyle());
      break;
    case 3:
      QApplication::setStyle(new QMotifStyle());
      break;
    case 4:
      QApplication::setStyle(new QCDEStyle());
      break;
#ifdef Q_WS_MAC
    case 5:
    QApplication::setStyle(new QMacStyle());
    break;
#endif
#ifdef Q_WS_WIN
    case 6:
    QApplication::setStyle(new QWindowsXPStyle());
    break;
#endif
  }
}

void options_imp::saveOptions(){
  applyButton->setEnabled(false);
  QSettings settings("qBittorrent", "qBittorrent");
  // Apply style
  useStyle();
  // Check if min port < max port
  checkPortsLogic();
  settings.beginGroup("Preferences");
  // General preferences
  settings.beginGroup("General");
  settings.setValue(QString::fromUtf8("Locale"), getLocale());
  settings.setValue(QString::fromUtf8("Style"), getStyle());
  settings.setValue(QString::fromUtf8("ExitConfirm"), confirmOnExit());
  settings.setValue(QString::fromUtf8("SpeedInTitleBar"), speedInTitleBar());
  settings.setValue(QString::fromUtf8("RefreshInterval"), getRefreshInterval());
  settings.setValue(QString::fromUtf8("SystrayEnabled"), systrayIntegration());
  settings.setValue(QString::fromUtf8("CloseToTray"), closeToTray());
  settings.setValue(QString::fromUtf8("MinimizeToTray"), minimizeToTray());
  settings.setValue(QString::fromUtf8("StartMinimized"), startMinimized());
  settings.setValue(QString::fromUtf8("NotificationBaloons"), OSDEnabled());
  // End General preferences
  settings.endGroup();
  // Downloads preferences
  settings.beginGroup("Downloads");
  settings.setValue(QString::fromUtf8("SavePath"), getSavePath());
  settings.setValue(QString::fromUtf8("PreAllocation"), preAllocateAllFiles());
  settings.setValue(QString::fromUtf8("AdditionDialog"), useAdditionDialog());
  settings.setValue(QString::fromUtf8("StartInPause"), addTorrentsInPause());
  settings.setValue(QString::fromUtf8("ScanDir"), getScanDir());
  settings.setValue(QString::fromUtf8("DblClOnTorDl"), getActionOnDblClOnTorrentDl());
  settings.setValue(QString::fromUtf8("DblClOnTorFn"), getActionOnDblClOnTorrentFn());
  // End Downloads preferences
  settings.endGroup();
  // Connection preferences
  settings.beginGroup("Connection");
  settings.setValue(QString::fromUtf8("PortRangeMin"), getPorts().first);
  settings.setValue(QString::fromUtf8("PortRangeMax"), getPorts().second);
  settings.setValue(QString::fromUtf8("UPnP"), isUPnPEnabled());
  settings.setValue(QString::fromUtf8("NAT-PMP"), isNATPMPEnabled());
  settings.setValue(QString::fromUtf8("GlobalDLLimit"), getGlobalBandwidthLimits().first);
  settings.setValue(QString::fromUtf8("GlobalUPLimit"), getGlobalBandwidthLimits().second);
  settings.setValue(QString::fromUtf8("ProxyType"), getProxyType());
  if(isProxyEnabled()) {
    settings.beginGroup("Proxy");
    // Proxy is enabled, save settings
    settings.setValue(QString::fromUtf8("IP"), getProxyIp());
    settings.setValue(QString::fromUtf8("Port"), getProxyPort());
    settings.setValue(QString::fromUtf8("Authentication"), isProxyAuthEnabled());
    if(isProxyAuthEnabled()) {
      // Credentials
      settings.setValue(QString::fromUtf8("Username"), getProxyUsername());
      settings.setValue(QString::fromUtf8("Password"), getProxyPassword());
    }
    // Affected connections
    settings.setValue(QString::fromUtf8("AffectTrackers"), useProxyForTrackers());
    settings.setValue(QString::fromUtf8("AffectPeers"), useProxyForPeers());
    settings.setValue(QString::fromUtf8("AffectWebSeeds"), useProxyForWebseeds());
    settings.setValue(QString::fromUtf8("AffectDHT"), useProxyForDHT());
    settings.endGroup(); // End Proxy
  }
  // End Connection preferences
  settings.endGroup();
  // Bittorrent preferences
  settings.beginGroup("Bittorrent");
  settings.setValue(QString::fromUtf8("MaxConnecs"), getMaxConnecs());
  settings.setValue(QString::fromUtf8("MaxConnecsPerTorrent"), getMaxConnecsPerTorrent());
  settings.setValue(QString::fromUtf8("MaxUploadsPerTorrent"), getMaxUploadsPerTorrent());
  settings.setValue(QString::fromUtf8("DHT"), isDHTEnabled());
  settings.setValue(QString::fromUtf8("PeX"), isPeXEnabled());
  settings.setValue(QString::fromUtf8("LSD"), isLSDEnabled());
  settings.setValue(QString::fromUtf8("Encryption"), getEncryptionSetting());
  settings.setValue(QString::fromUtf8("DesiredRatio"), getDesiredRatio());
  settings.setValue(QString::fromUtf8("MaxRatio"), getDeleteRatio());
  // End Bittorrent preferences
  settings.endGroup();
  // Misc preferences
  // * IPFilter
  settings.beginGroup("IPFilter");
  settings.setValue(QString::fromUtf8("Enabled"), isFilteringEnabled());
  if(isFilteringEnabled()){
    settings.setValue(QString::fromUtf8("File"), textFilterPath->text());
  }
  // End IPFilter preferences
  settings.endGroup();
  // * RSS
  settings.beginGroup("RSS");
  settings.setValue(QString::fromUtf8("RSSRefresh"), spinRSSRefresh->value());
  settings.setValue(QString::fromUtf8("RSSMaxArticlesPerFeed"), spinRSSMaxArticlesPerFeed->value());
  // End RSS preferences
  settings.endGroup();
  // Web UI
  settings.beginGroup("WebUI");
  settings.setValue("Enabled", isWebUiEnabled());
  if(isWebUiEnabled())
  {
    settings.setValue("Port", webUiPort());
    settings.setValue("Username", webUiUsername());
    settings.setValue("Password", webUiPassword());
  }
  // End Web UI
  settings.endGroup();
  // End preferences
  settings.endGroup();
}

bool options_imp::isFilteringEnabled() const{
  return checkIPFilter->isChecked();
}

int options_imp::getProxyType() const{
  if(comboProxyType->currentIndex() == HTTP){
    if(isProxyAuthEnabled()){
      return HTTP_PW;
    }else{
      return HTTP;
    }
  }else{
    if(comboProxyType->currentIndex() == SOCKS5){
      if(isProxyAuthEnabled()){
        return SOCKS5_PW;
      }else{
        return SOCKS5;
      }
    }
  }
  return -1; // disabled
}

bool options_imp::useProxyForTrackers() const{
  return checkProxyTrackers->isChecked();
}

bool options_imp::useProxyForPeers() const{
  return checkProxyPeers->isChecked();
}

bool options_imp::useProxyForWebseeds() const{
  return checkProxyWebseeds->isChecked();
}

bool options_imp::useProxyForDHT() const{
  return checkProxyDHT->isChecked();
}

int options_imp::getStyle() const{
  return comboStyle->currentIndex();
}

void options_imp::setStyle(int style){
  if(style >= comboStyle->count() || style < 0)
    style = 0;
  comboStyle->setCurrentIndex(style);
}

void options_imp::loadOptions(){
  int intValue;
  float floatValue;
  QString strValue;
  QSettings settings("qBittorrent", "qBittorrent");
  settings.beginGroup("Preferences");
  // General preferences
  settings.beginGroup("General");
  setLocale(settings.value(QString::fromUtf8("Locale"), "en_GB").toString());
  setStyle(settings.value(QString::fromUtf8("Style"), 0).toInt());
  checkConfirmExit->setChecked(settings.value(QString::fromUtf8("ExitConfirm"), true).toBool());
  checkSpeedInTitle->setChecked(settings.value(QString::fromUtf8("SpeedInTitleBar"), false).toBool());
  spinRefreshInterval->setValue(settings.value(QString::fromUtf8("RefreshInterval"), 1500).toInt());
  checkNoSystray->setChecked(!settings.value(QString::fromUtf8("SystrayEnabled"), true).toBool());
  if(!systrayIntegration()) {
    disableSystrayOptions();
  } else {
    enableSystrayOptions();
    checkCloseToSystray->setChecked(settings.value(QString::fromUtf8("CloseToTray"), false).toBool());
    checkMinimizeToSysTray->setChecked(settings.value(QString::fromUtf8("MinimizeToTray"), false).toBool());
    checkStartMinimized->setChecked(settings.value(QString::fromUtf8("StartMinimized"), false).toBool());
    checkSystrayBalloons->setChecked(settings.value(QString::fromUtf8("NotificationBaloons"), true).toBool());
  }
  // End General preferences
  settings.endGroup();
  // Downloads preferences
  settings.beginGroup("Downloads");
  QString home = QDir::homePath();
  if(home[home.length()-1] != QDir::separator()){
    home += QDir::separator();
  }
  textSavePath->setText(settings.value(QString::fromUtf8("SavePath"), home+"qBT_dir").toString());
  checkPreallocateAll->setChecked(settings.value(QString::fromUtf8("PreAllocation"), false).toBool());
  checkAdditionDialog->setChecked(settings.value(QString::fromUtf8("AdditionDialog"), true).toBool());
  checkStartPaused->setChecked(settings.value(QString::fromUtf8("StartInPause"), false).toBool());
  strValue = settings.value(QString::fromUtf8("ScanDir"), QString()).toString();
  if(strValue.isEmpty()) {
    // Disable
    checkScanDir->setChecked(false);
    enableDirScan(0);
  } else {
    // enable
    checkScanDir->setChecked(true);
    textScanDir->setText(strValue);
    enableDirScan(2);
  }
  actionTorrentDlOnDblClBox->setCurrentIndex(settings.value(QString::fromUtf8("DblClOnTorDl"), 0).toInt());
  actionTorrentFnOnDblClBox->setCurrentIndex(settings.value(QString::fromUtf8("DblClOnTorFn"), 0).toInt());
  intValue = settings.value(QString::fromUtf8("DblClOnTorFn"), 1).toInt();
  // End Downloads preferences
  settings.endGroup();
  // Connection preferences
  settings.beginGroup("Connection");
  spinPortMin->setValue(settings.value(QString::fromUtf8("PortRangeMin"), 6881).toInt());
  spinPortMax->setValue(settings.value(QString::fromUtf8("PortRangeMax"), 6889).toInt());
  // Check if min port < max port
  checkPortsLogic();
  checkUPnP->setChecked(settings.value(QString::fromUtf8("UPnP"), true).toBool());
  checkNATPMP->setChecked(settings.value(QString::fromUtf8("NAT-PMP"), true).toBool());
  intValue = settings.value(QString::fromUtf8("GlobalDLLimit"), -1).toInt();
  if(intValue > 0) {
    // Enabled
    checkDownloadLimit->setChecked(true);
    spinDownloadLimit->setEnabled(true);
    spinDownloadLimit->setValue(intValue);
  } else {
    // Disabled
    checkDownloadLimit->setChecked(false);
    spinDownloadLimit->setEnabled(false);
  }
  intValue = settings.value(QString::fromUtf8("GlobalUPLimit"), 50).toInt();
  if(intValue != -1) {
    // Enabled
    checkUploadLimit->setChecked(true);
    spinUploadLimit->setEnabled(true);
    spinUploadLimit->setValue(intValue);
  } else {
    // Disabled
    checkUploadLimit->setChecked(false);
    spinUploadLimit->setEnabled(false);
  }
  intValue = settings.value(QString::fromUtf8("ProxyType"), 0).toInt();
  if(intValue <= 0) {
    intValue = 0;
  } else {
    if(intValue%2 == 0) {
      intValue = 2;
    }else {
      intValue = 1;
    }
  }
  comboProxyType->setCurrentIndex(intValue);
  enableProxy(intValue);
  if(isProxyEnabled()) {
    settings.beginGroup("Proxy");
    // Proxy is enabled, save settings
    textProxyIP->setText(settings.value(QString::fromUtf8("IP"), "0.0.0.0").toString());
    spinProxyPort->setValue(settings.value(QString::fromUtf8("Port"), 8080).toInt());
    checkProxyAuth->setChecked(settings.value(QString::fromUtf8("Authentication"), false).toBool());
    if(isProxyAuthEnabled()) {
      enableProxyAuth(2); // Enable
      // Credentials
      textProxyUsername->setText(settings.value(QString::fromUtf8("Username"), QString()).toString());
      textProxyPassword->setText(settings.value(QString::fromUtf8("Password"), QString()).toString());
    } else {
      enableProxyAuth(0); // Disable
    }
    // Affected connections
    checkProxyTrackers->setChecked(settings.value(QString::fromUtf8("AffectTrackers"), true).toBool());
    checkProxyPeers->setChecked(settings.value(QString::fromUtf8("AffectPeers"), true).toBool());
    checkProxyWebseeds->setChecked(settings.value(QString::fromUtf8("AffectWebSeeds"), true).toBool());
    checkProxyDHT->setChecked(settings.value(QString::fromUtf8("AffectDHT"), true).toBool());
    settings.endGroup(); // End Proxy
  }
  // End Connection preferences
  settings.endGroup();
  // Bittorrent preferences
  settings.beginGroup("Bittorrent");
  intValue = settings.value(QString::fromUtf8("MaxConnecs"), 500).toInt();
  if(intValue != -1) {
    // enable
    checkMaxConnecs->setChecked(true);
    spinMaxConnec->setEnabled(true);
    spinMaxConnec->setValue(intValue);
  } else {
    // disable
    checkMaxConnecs->setChecked(false);
    spinMaxConnec->setEnabled(false);
  }
  intValue = settings.value(QString::fromUtf8("MaxConnecsPerTorrent"), 100).toInt();
  if(intValue != -1) {
    // enable
    checkMaxConnecsPerTorrent->setChecked(true);
    spinMaxConnecPerTorrent->setEnabled(true);
    spinMaxConnecPerTorrent->setValue(intValue);
  } else {
    // disable
    checkMaxConnecsPerTorrent->setChecked(false);
    spinMaxConnecPerTorrent->setEnabled(false);
  }
  intValue = settings.value(QString::fromUtf8("MaxUploadsPerTorrent"), 4).toInt();
  if(intValue != -1) {
    // enable
    checkMaxUploadsPerTorrent->setChecked(true);
    spinMaxUploadsPerTorrent->setEnabled(true);
    spinMaxUploadsPerTorrent->setValue(intValue);
  } else {
    // disable
    checkMaxUploadsPerTorrent->setChecked(false);
    spinMaxUploadsPerTorrent->setEnabled(false);
  }
  checkDHT->setChecked(settings.value(QString::fromUtf8("DHT"), true).toBool());
  checkPeX->setChecked(settings.value(QString::fromUtf8("PeX"), true).toBool());
  checkLSD->setChecked(settings.value(QString::fromUtf8("LSD"), true).toBool());
  comboEncryption->setCurrentIndex(settings.value(QString::fromUtf8("Encryption"), 0).toInt());
  floatValue = settings.value(QString::fromUtf8("DesiredRatio"), -1).toDouble();
  if(floatValue >= 1.) {
    // Enable
    checkRatioLimit->setChecked(true);
    spinRatio->setEnabled(true);
    spinRatio->setValue(floatValue);
  } else {
    // Disable
    checkRatioLimit->setChecked(false);
    spinRatio->setEnabled(false);
  }
  floatValue = settings.value(QString::fromUtf8("MaxRatio"), -1).toDouble();
  if(floatValue >= 1.) {
    // Enable
    checkRatioRemove->setChecked(true);
    spinMaxRatio->setEnabled(true);
    spinMaxRatio->setValue(floatValue);
  } else {
    // Disable
    checkRatioRemove->setChecked(false);
    spinMaxRatio->setEnabled(false);
  }
  // End Bittorrent preferences
  settings.endGroup();
  // Misc preferences
  // * IP Filter
  settings.beginGroup("IPFilter");
  checkIPFilter->setChecked(settings.value(QString::fromUtf8("Enabled"), false).toBool());
  if(isFilteringEnabled()) {
    enableFilter(2); // Enable
    textFilterPath->setText(settings.value(QString::fromUtf8("File"), QString()).toString());
    processFilterFile(textFilterPath->text());
  } else {
    enableFilter(0); // Disable
  }
  // End IP Filter
  settings.endGroup();
  // * RSS
  settings.beginGroup("RSS");
  spinRSSRefresh->setValue(settings.value(QString::fromUtf8("RSSRefresh"), 5).toInt());
  spinRSSMaxArticlesPerFeed->setValue(settings.value(QString::fromUtf8("RSSMaxArticlesPerFeed"), 50).toInt());
  // End RSS preferences
  settings.endGroup();
  // Web UI
  settings.beginGroup("WebUI");
  checkWebUi->setChecked(settings.value("Enabled", false).toBool());
  enableWebUi(isWebUiEnabled());
  spinWebUiPort->setValue(settings.value("Port", 8080).toInt());
  textWebUiUsername->setText(settings.value("Username", "user").toString());
  textWebUiPassword->setText(settings.value("Password", "").toString());
  // End Web UI
  settings.endGroup();
}

// return min & max ports
// [min, max]
std::pair<unsigned short, unsigned short> options_imp::getPorts() const{
  return std::make_pair(spinPortMin->value(), spinPortMax->value());
}

int options_imp::getEncryptionSetting() const{
  return comboEncryption->currentIndex();
}

bool options_imp::minimizeToTray() const{
  if(checkNoSystray->isChecked()) return false;
  return checkMinimizeToSysTray->isChecked();
}

bool options_imp::closeToTray() const{
  if(checkNoSystray->isChecked()) return false;
  return checkCloseToSystray->isChecked();
}

unsigned int options_imp::getRefreshInterval() const {
  return spinRefreshInterval->value();
}

bool options_imp::confirmOnExit() const{
  return checkConfirmExit->isChecked();
}

bool options_imp::isDirScanEnabled() const {
  return checkScanDir->isChecked();
}

bool options_imp::isDHTEnabled() const{
  return checkDHT->isChecked();
}

bool options_imp::isPeXEnabled() const{
  return checkPeX->isChecked();
}

bool options_imp::isLSDEnabled() const{
  return checkLSD->isChecked();
}

bool options_imp::isUPnPEnabled() const{
  return checkUPnP->isChecked();
}

bool options_imp::isNATPMPEnabled() const{
  return checkNATPMP->isChecked();
}

// Return Download & Upload limits in kbps
// [download,upload]
QPair<int,int> options_imp::getGlobalBandwidthLimits() const{
  int DL = -1, UP = -1;
  if(checkDownloadLimit->isChecked()){
    DL = spinDownloadLimit->value();
  }
  if(checkUploadLimit->isChecked()){
    UP = spinUploadLimit->value();
  }
  return qMakePair(DL, UP);
}

bool options_imp::OSDEnabled() const {
  if(checkNoSystray->isChecked()) return false;
  return checkSystrayBalloons->isChecked();
}

bool options_imp::startMinimized() const {
  if(checkStartMinimized->isChecked()) return true;
  return checkStartMinimized->isChecked();
}

bool options_imp::systrayIntegration() const{
  if (!QSystemTrayIcon::isSystemTrayAvailable()) return false;
  return (!checkNoSystray->isChecked());
}

// Return Share ratio
float options_imp::getDesiredRatio() const{
  if(checkRatioLimit->isChecked()){
    return spinRatio->value();
  }
  return -1;
}

// Return Share ratio
float options_imp::getDeleteRatio() const{
  if(checkRatioRemove->isChecked()){
    return spinMaxRatio->value();
  }
  return -1;
}

// Return Save Path
QString options_imp::getSavePath() const{
  QString home = QDir::homePath();
  if(home[home.length()-1] != QDir::separator()){
    home += QDir::separator();
  }
  if(textSavePath->text().trimmed().isEmpty()){
    textSavePath->setText(home+QString::fromUtf8("qBT_dir"));
  }
  return textSavePath->text();
}

// Return max connections number
int options_imp::getMaxConnecs() const{
  if(!checkMaxConnecs->isChecked()){
    return -1;
  }else{
    return spinMaxConnec->value();
  }
}

int options_imp::getMaxConnecsPerTorrent() const{
  if(!checkMaxConnecsPerTorrent->isChecked()){
    return -1;
  }else{
    return spinMaxConnecPerTorrent->value();
  }
}

int options_imp::getMaxUploadsPerTorrent() const{
  if(!checkMaxUploadsPerTorrent->isChecked()){
    return -1;
  }else{
    return spinMaxUploadsPerTorrent->value();
  }
}

void options_imp::on_buttonBox_accepted(){
  if(applyButton->isEnabled()){
    saveOptions();
    applyButton->setEnabled(false);
    // set infobar text
    this->hide();
    emit status_changed(tr("Options were saved successfully."), true);
  }else{
    setAttribute(Qt::WA_DeleteOnClose);
    accept();
  }
}

void options_imp::applySettings(QAbstractButton* button) {
  if(button == applyButton){
    saveOptions();
    emit status_changed(tr("Options were saved successfully."), false);
  }
}

void options_imp::closeEvent(QCloseEvent *e){
  setAttribute(Qt::WA_DeleteOnClose);
  e->accept();
}

void options_imp::on_buttonBox_rejected(){
  setAttribute(Qt::WA_DeleteOnClose);
  reject();
}

void options_imp::enableDownloadLimit(int checkBoxValue){
  if(checkBoxValue!=2){
    //Disable
    spinDownloadLimit->setEnabled(false);
  }else{
    //enable
    spinDownloadLimit->setEnabled(true);
  }
}

bool options_imp::useAdditionDialog() const{
  return checkAdditionDialog->isChecked();
}

void options_imp::enableMaxConnecsLimit(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinMaxConnec->setEnabled(false);
  }else{
    //enable
    spinMaxConnec->setEnabled(true);
  }
}

void options_imp::enableMaxConnecsLimitPerTorrent(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinMaxConnecPerTorrent->setEnabled(false);
  }else{
    //enable
    spinMaxConnecPerTorrent->setEnabled(true);
  }
}

void options_imp::enableSystrayOptions() {
  checkCloseToSystray->setEnabled(true);
  checkMinimizeToSysTray->setEnabled(true);
  checkSystrayBalloons->setEnabled(true);
}

void options_imp::disableSystrayOptions() {
  checkCloseToSystray->setEnabled(false);
  checkMinimizeToSysTray->setEnabled(false);
  checkSystrayBalloons->setEnabled(false);
}

void options_imp::setSystrayOptionsState(int checkBoxValue) {
  if(checkBoxValue == 2) {
    disableSystrayOptions();
  } else {
    enableSystrayOptions();
  }
}

void options_imp::enableMaxUploadsLimitPerTorrent(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinMaxUploadsPerTorrent->setEnabled(false);
  }else{
    //enable
    spinMaxUploadsPerTorrent->setEnabled(true);
  }
}

void options_imp::enableFilter(int checkBoxValue){
  if(checkBoxValue!=2){
    //Disable
    lblFilterPath->setEnabled(false);
    textFilterPath->setEnabled(false);
    browseFilterButton->setEnabled(false);
  }else{
    //enable
    lblFilterPath->setEnabled(true);
    textFilterPath->setEnabled(true);
    browseFilterButton->setEnabled(true);
  }
}

void options_imp::enableUploadLimit(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinUploadLimit->setEnabled(false);
  }else{
    //enable
    spinUploadLimit->setEnabled(true);
  }
}

void options_imp::enableApplyButton(){
  if(!applyButton->isEnabled()){
    applyButton->setEnabled(true);
  }
}

void options_imp::enableShareRatio(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinRatio->setEnabled(false);
  }else{
    //enable
    spinRatio->setEnabled(true);
  }
}

void options_imp::enableDeleteRatio(int checkBoxValue){
  if(checkBoxValue != 2){
    //Disable
    spinMaxRatio->setEnabled(false);
  }else{
    //enable
    spinMaxRatio->setEnabled(true);
  }
}

void options_imp::enableProxy(int index){
  if(index){
    //enable
    lblProxyIP->setEnabled(true);
    textProxyIP->setEnabled(true);
    lblProxyPort->setEnabled(true);
    spinProxyPort->setEnabled(true);
    checkProxyAuth->setEnabled(true);
    ProxyConnecsBox->setEnabled(true);
  }else{
    //disable
    lblProxyIP->setEnabled(false);
    textProxyIP->setEnabled(false);
    lblProxyPort->setEnabled(false);
    spinProxyPort->setEnabled(false);
    checkProxyAuth->setChecked(false);
    checkProxyAuth->setEnabled(false);
    ProxyConnecsBox->setEnabled(false);
  }
}

void options_imp::enableProxyAuth(int checkBoxValue){
  if(checkBoxValue==2){
    //enable
    lblProxyUsername->setEnabled(true);
    lblProxyPassword->setEnabled(true);
    textProxyUsername->setEnabled(true);
    textProxyPassword->setEnabled(true);
  }else{
    //disable
    lblProxyUsername->setEnabled(false);
    lblProxyPassword->setEnabled(false);
    textProxyUsername->setEnabled(false);
    textProxyPassword->setEnabled(false);
  }
}

void options_imp::enableDirScan(int checkBoxValue){
  if(checkBoxValue==2){
    //enable
    textScanDir->setEnabled(true);
    browseScanDirButton->setEnabled(true);
  }else{
    //disable
    textScanDir->setEnabled(false);
    browseScanDirButton->setEnabled(false);
  }
}

bool options_imp::speedInTitleBar() const {
  return checkSpeedInTitle->isChecked();
}

bool options_imp::preAllocateAllFiles() const {
  return checkPreallocateAll->isChecked();
}

bool options_imp::addTorrentsInPause() const {
  return checkStartPaused->isChecked();
}

// Proxy settings
bool options_imp::isProxyEnabled() const{
  return comboProxyType->currentIndex();
}

bool options_imp::isProxyAuthEnabled() const{
  return checkProxyAuth->isChecked();
}

QString options_imp::getProxyIp() const{
  QString ip = textProxyIP->text();
  ip = ip.trimmed();
  return ip;
}

unsigned short options_imp::getProxyPort() const{
  return spinProxyPort->value();
}

QString options_imp::getProxyUsername() const{
  QString username = textProxyUsername->text();
  username = username.trimmed();
  return username;
}

QString options_imp::getProxyPassword() const{
  QString password = textProxyPassword->text();
  password = password.trimmed();
  return password;
}

// Locale Settings
QString options_imp::getLocale() const{
  return locales.at(comboI18n->currentIndex());
}

void options_imp::setLocale(QString locale){
  int indexLocales=locales.indexOf(QRegExp(locale));
  if(indexLocales != -1){
    comboI18n->setCurrentIndex(indexLocales);
  }
}

// Is called before saving to check if minPort < maxPort
void options_imp::checkPortsLogic(){
  int maxValue = spinPortMax->value();
  if(spinPortMin->value() > spinPortMax->value()){
    spinPortMax->setValue(spinPortMin->value());
    spinPortMin->setValue(maxValue);
  }
}

// Return scan dir set in options
QString options_imp::getScanDir() const {
  if(checkScanDir->isChecked()){
    QString scanDir = textScanDir->text();
    scanDir = scanDir.trimmed();
    return scanDir;
  }else{
    return QString();
  }
}

// Return action on double-click on a downloading torrent set in options
int options_imp::getActionOnDblClOnTorrentDl() const {
  if(actionTorrentDlOnDblClBox->currentIndex()<1)
    return 0;
  return actionTorrentDlOnDblClBox->currentIndex();
}

// Return action on double-click on a finished torrent set in options
int options_imp::getActionOnDblClOnTorrentFn() const {
  if(actionTorrentFnOnDblClBox->currentIndex()<1)
    return 0;
  return actionTorrentFnOnDblClBox->currentIndex();
}

// Display dialog to choose scan dir
void options_imp::on_browseScanDirButton_clicked() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Choose scan directory"), QDir::homePath());
  if(!dir.isNull()){
    textScanDir->setText(dir);
  }
}

void options_imp::on_browseFilterButton_clicked() {
  QString ipfilter = QFileDialog::getOpenFileName(this, tr("Choose an ip filter file"), QDir::homePath(), tr("Filters (*.dat *.p2p *.p2b)"));
  if(!ipfilter.isNull()){
    textFilterPath->setText(ipfilter);
    processFilterFile(ipfilter);
  }
}

// Display dialog to choose save dir
void options_imp::on_browseSaveDirButton_clicked(){
  QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a save directory"), QDir::homePath());
  if(!dir.isNull()){
    textSavePath->setText(dir);
  }
}

// Parser for eMule ip filter in DAT format
void options_imp::parseDATFilterFile(QString filePath) {
  const QRegExp is_ipv6(QString::fromUtf8("^[0-9a-f]{4}(:[0-9a-f]{4}){7}$"), Qt::CaseInsensitive, QRegExp::RegExp);
  const QRegExp is_ipv4(QString::fromUtf8("^(([0-1]?[0-9]?[0-9])|(2[0-4][0-9])|(25[0-5]))(\\.(([0-1]?[0-9]?[0-9])|(2[0-4][0-9])|(25[0-5]))){3}$"), Qt::CaseInsensitive, QRegExp::RegExp);
  QString strStartIP, strEndIP;
  bool IPv4 = true;
  QFile file(filePath);
  if (file.exists()){
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("Couldn't open %1 in read mode.").arg(filePath));
      return;
    }
    unsigned int nbLine = 0;
    while (!file.atEnd()) {
      ++nbLine;
      QByteArray line = file.readLine();
      // Ignoring empty lines
      line = line.trimmed();
      if(line.isEmpty()) continue;
      // Ignoring commented lines
      if(line.startsWith('#') || line.startsWith("//")) continue;
      // Line is not commented
      QList<QByteArray> partsList = line.split(',');
      unsigned int nbElem = partsList.size();
      // IP Range can be splitted by a dash or a comma...
      // Check if there is a dash in first part
      QByteArray firstPart = partsList.at(0);
      int nbAccess = 0;
      if(firstPart.contains('-')) {
        // Range is splitted by a dash
        QList<QByteArray> IPs = firstPart.split('-');
        if(IPs.size() != 2) {
          qDebug("Ipfilter.dat: line %d is malformed.", nbLine);
          continue;
        }
        strStartIP = IPs.at(0).trimmed();
        strEndIP = IPs.at(1).trimmed();
        // Check if IPs are correct
        if(strStartIP.contains(is_ipv4) && strEndIP.contains(is_ipv4)) {
          IPv4 = true;
        } else {
          if(strStartIP.contains(is_ipv6) && strEndIP.contains(is_ipv6)) {
            IPv4 = false;
          } else {
            // Could not determine IP format
            qDebug("Ipfilter.dat: line %d is malformed.", nbLine);
            continue;
          }
        }
        // Check if there is an access value (apparently not mandatory)
        if(nbElem > 1) {
          // There is possibly one
          bool ok;
          nbAccess = partsList.at(1).trimmed().toInt(&ok);
          if(!ok){
            nbAccess = 0;
          }
        }
      } else {
        // Range is probably splitted by a comma
        unsigned int nbElem = partsList.size();
        if(nbElem > 1) {
          strStartIP = firstPart.trimmed();
          strEndIP = partsList.at(1).trimmed();
          // Check if IPs are correct
          if(strStartIP.contains(is_ipv4) && strEndIP.contains(is_ipv4)) {
            IPv4 = true;
          } else {
            if(strStartIP.contains(is_ipv6) && strEndIP.contains(is_ipv6)) {
              IPv4 = false;
            } else {
              // Could not determine IP format
              qDebug("Ipfilter.dat: line %d is malformed.", nbLine);
              continue;
            }
          }
          // Check if there is an access value (apparently not mandatory)
          if(nbElem > 2) {
            // There is possibly one
            bool ok;
            nbAccess = partsList.at(2).trimmed().toInt(&ok);
            if(!ok){
              nbAccess = 0;
            }
          }
        }
      }
      if(nbAccess > 127) {
        // Ignoring this rule because access value is too high
        continue;
      }
      // Now Add to the filter
      QStringList IP;
      if(IPv4) {
        //IPv4 addresses
        IP = strStartIP.split('.');
        address_v4 start((IP.at(0).toInt() << 24) + (IP.at(1).toInt() << 16) + (IP.at(2).toInt() << 8) + IP.at(3).toInt());
        IP = strEndIP.split('.');
        address_v4 last((IP.at(0).toInt() << 24) + (IP.at(1).toInt() << 16) + (IP.at(2).toInt() << 8) + IP.at(3).toInt());
        // Apply to bittorrent session
        filter.add_rule(start, last, ip_filter::blocked);
      } else {
        // IPv6, ex :   1fff:0000:0a88:85a3:0000:0000:ac1f:8001
        IP = strStartIP.split(':');
        address_v6 start = address_v6::from_string(strStartIP.remove(':', 0).toUtf8().data());
        IP = strEndIP.split(':');
        address_v6 last = address_v6::from_string(strEndIP.remove(':', 0).toUtf8().data());
        // Apply to bittorrent session
        filter.add_rule(start, last, ip_filter::blocked);
      }
    }
    file.close();
  }
}

// Parser for PeerGuardian ip filter in p2p format
void options_imp::parseP2PFilterFile(QString filePath) {
  const QRegExp is_ipv4(QString::fromUtf8("^(([0-1]?[0-9]?[0-9])|(2[0-4][0-9])|(25[0-5]))(\\.(([0-1]?[0-9]?[0-9])|(2[0-4][0-9])|(25[0-5]))){3}$"), Qt::CaseInsensitive, QRegExp::RegExp);
  QFile file(filePath);
  QStringList IP;
  if (file.exists()){
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("Couldn't open %1 in read mode.").arg(filePath));
      return;
    }
    unsigned int nbLine = 0;
    while (!file.atEnd()) {
      ++nbLine;
      QByteArray line = file.readLine();
      // Ignoring empty lines
      line = line.trimmed();
      if(line.isEmpty()) continue;
      // Ignoring commented lines
      if(line.startsWith('#') || line.startsWith("//")) continue;
      // Line is not commented
      QList<QByteArray> partsList = line.split(':');
      if(partsList.size() < 2){
        qDebug("p2p file: line %d is malformed.", nbLine);
        continue;
      }
      // Get IP range
      QList<QByteArray> IPs = partsList.at(1).split('-');
      if(IPs.size() != 2) {
        qDebug("p2p file: line %d is malformed.", nbLine);
        continue;
      }
      QString strStartIP = IPs.at(0).trimmed();
      QString strEndIP = IPs.at(1).trimmed();
      // Check IPs format (IPv4 only)
      if(strStartIP.contains(is_ipv4) && strEndIP.contains(is_ipv4)) {
        // IPv4
        IP = strStartIP.split('.');
        address_v4 start((IP.at(0).toInt() << 24) + (IP.at(1).toInt() << 16) + (IP.at(2).toInt() << 8) + IP.at(3).toInt());
        IP = strEndIP.split('.');
        address_v4 last((IP.at(0).toInt() << 24) + (IP.at(1).toInt() << 16) + (IP.at(2).toInt() << 8) + IP.at(3).toInt());
        // Apply to bittorrent session
        filter.add_rule(start, last, ip_filter::blocked);
      } else {
          qDebug("p2p file: line %d is malformed.", nbLine);
          continue;
      }
    }
    file.close();
  }
}

int options_imp::getlineInStream(QDataStream& stream, string& name, char delim) {
  char c;
  int total_read = 0;
  do {
    int read = stream.readRawData(&c, 1);
    total_read += read;
    if(read > 0) {
      if(c != delim) {
        name += c;
      } else {
        // Delim found
        return total_read;
      }
    }
  } while(read > 0);
  return total_read;
}

// Parser for PeerGuardian ip filter in p2p format
void options_imp::parseP2BFilterFile(QString filePath) {
  QFile file(filePath);
  if (file.exists()){
    if(!file.open(QIODevice::ReadOnly)){
      QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("Couldn't open %1 in read mode.").arg(filePath));
      return;
    }
    QDataStream stream(&file);
    // Read header
    char buf[7];
    unsigned char version;
    if(
            !stream.readRawData(buf, sizeof(buf)) ||
            memcmp(buf, "\xFF\xFF\xFF\xFFP2B", 7) ||
            !stream.readRawData((char*)&version, sizeof(version))
    ) {
      QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
      return;
    }

    if(version==1 || version==2) {
      qDebug ("p2b version 1 or 2");
      unsigned int start, end;
      
      string name;
      while(getlineInStream(stream, name, '\0')) {
        if(
          !stream.readRawData((char*)&start, sizeof(start)) ||
          !stream.readRawData((char*)&end, sizeof(end))
        ) {
          QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
          return;
        }
        // Network byte order to Host byte order
        // asio address_v4 contructor expects it
        // that way
        address_v4 first(ntohl(start));
        address_v4 last(ntohl(end));
        // Apply to bittorrent session
        filter.add_rule(first, last, ip_filter::blocked);
      }
    }
    else if(version==3) {
      qDebug ("p2b version 3");
      unsigned int namecount;
      if(!stream.readRawData((char*)&namecount, sizeof(namecount))) {
        QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
        return;
      }
      namecount=ntohl(namecount);
      // Reading names although, we don't really care about them
      for(unsigned int i=0; i<namecount; i++) {
        string name;
        if(!getlineInStream(stream, name, '\0')) {
          QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
          return;
        }
      }
      // Reading the ranges
      unsigned int rangecount;
      if(!stream.readRawData((char*)&rangecount, sizeof(rangecount))) {
        QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
        return;
      }
      rangecount=ntohl(rangecount);

      unsigned int name, start, end;

      for(unsigned int i=0; i<rangecount; i++) {
        if(
          !stream.readRawData((char*)&name, sizeof(name)) ||
          !stream.readRawData((char*)&start, sizeof(start)) ||
          !stream.readRawData((char*)&end, sizeof(end))
        ) {
          QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
          return;
        }
        // Network byte order to Host byte order
        // asio address_v4 contructor expects it
        // that way
        address_v4 first(ntohl(start));
        address_v4 last(ntohl(end));
        // Apply to bittorrent session
        filter.add_rule(first, last, ip_filter::blocked);
      }
    } else {
      QMessageBox::critical(0, tr("I/O Error", "Input/Output Error"), tr("%1 is not a valid PeerGuardian P2B file.").arg(filePath));
      return;
    }
    file.close();
  }
}

// Process ip filter file
// Supported formats:
//  * eMule IP list (DAT): http://wiki.phoenixlabs.org/wiki/DAT_Format
//  * PeerGuardian Text (P2P): http://wiki.phoenixlabs.org/wiki/P2P_Format
//  * PeerGuardian Binary (P2B): http://wiki.phoenixlabs.org/wiki/P2B_Format
void options_imp::processFilterFile(QString filePath){
  qDebug("Processing filter files");
  if(filePath.endsWith(".dat", Qt::CaseInsensitive)) {
    // eMule DAT file
    parseDATFilterFile(filePath);
  } else {
    if(filePath.endsWith(".p2p", Qt::CaseInsensitive)) {
      // PeerGuardian p2p file
      parseP2PFilterFile(filePath);
    } else {
        if(filePath.endsWith(".p2p", Qt::CaseInsensitive)) {
          // PeerGuardian p2p file
          parseP2BFilterFile(filePath);
        } else {
          // Default: eMule DAT format
          parseDATFilterFile(filePath);
        }
    }
  }
}

// Return Filter object to apply to BT session
ip_filter options_imp::getFilter() const{
  return filter;
}

// Web UI

void options_imp::enableWebUi(bool checkBoxValue){
  groupWebUiServer->setEnabled(checkBoxValue);
  groupWebUiAuth->setEnabled(checkBoxValue);
}

bool options_imp::isWebUiEnabled() const
{
  return checkWebUi->isChecked();
}

quint16 options_imp::webUiPort() const
{
  return spinWebUiPort->value();
}

QString options_imp::webUiUsername() const
{
  return textWebUiUsername->text();
}

QString options_imp::webUiPassword() const
{
  return textWebUiPassword->text();
}
