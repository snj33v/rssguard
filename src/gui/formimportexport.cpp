// This file is part of RSS Guard.
//
// Copyright (C) 2011-2014 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "gui/formimportexport.h"

#include "core/feedsimportexportmodel.h"
#include "core/feedsmodel.h"
#include "miscellaneous/application.h"
#include "gui/feedmessageviewer.h"
#include "gui/formmain.h"
#include "gui/feedsview.h"

#include <QFileDialog>
#include <QTextStream>


FormImportExport::FormImportExport(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormImportExport), m_mode(Import) {
  m_ui->setupUi(this);
  m_model = new FeedsImportExportModel(m_ui->m_treeFeeds);

  m_ui->m_lblSelectFile->setStatus(WidgetWithStatus::Error, tr("No file is selected."), tr("No file is selected."));
  m_ui->m_buttonBox->button(QDialogButtonBox::Ok)->disconnect();
  m_ui->m_lblResult->setStatus(WidgetWithStatus::Warning, tr("No operation executed yet."), tr("No operation executed yet."));

  connect(m_ui->m_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(performAction()));
  connect(m_ui->m_btnSelectFile, SIGNAL(clicked()), this, SLOT(selectFile()));
}

FormImportExport::~FormImportExport() {
  delete m_ui;
}

FormImportExport::Mode FormImportExport::mode() const {
  return m_mode;
}

void FormImportExport::setMode(const Mode &mode) {
  m_mode = mode;

  switch (m_mode) {
    case Export: {
      m_model->setRootItem(qApp->mainForm()->tabWidget()->feedMessageViewer()->feedsView()->sourceModel()->rootItem());
      m_ui->m_treeFeeds->setModel(m_model);
      m_ui->m_treeFeeds->expandAll();
      setWindowTitle(tr("Export feeds"));
      setWindowIcon(qApp->icons()->fromTheme("document-export"));
      m_ui->m_groupFile->setTitle(tr("Destination file"));
      m_ui->m_groupFeeds->setTitle(tr("Source feeds && categories"));
      break;
    }

    case Import: {
      m_ui->m_groupFile->setTitle(tr("Source file"));
      m_ui->m_groupFeeds->setTitle(tr("Target feeds && categories"));
      m_ui->m_treeFeeds->setDisabled(true);
      setWindowTitle(tr("Import feeds"));
      setWindowIcon(qApp->icons()->fromTheme("document-import"));
      break;
    }

    default:
      break;
  }
}

void FormImportExport::selectFile() {
  switch (m_mode) {
    case Import:
      selectImportFile();
      break;

    case Export: {
      selectExportFile();
      break;
    }

    default:
      break;
  }
}

void FormImportExport::selectExportFile() {
  QString filter_opml20 = tr("OPML 2.0 files (*.opml)");

  QString filter;
  QString selected_filter;

  // Add more filters here.
  filter += filter_opml20;

  QString selected_file = QFileDialog::getSaveFileName(this, tr("Select file for feeds export"),
                                                       QDir::homePath(), filter, &selected_filter);



  if (!selected_file.isEmpty()) {
    if (selected_filter == filter_opml20) {
      m_conversionType = OPML20;
    }
    // NOTE: Add other types here.

    m_ui->m_lblSelectFile->setStatus(WidgetWithStatus::Ok, QDir::toNativeSeparators(selected_file), tr("File is selected."));
  }
  else {
    m_ui->m_lblSelectFile->setStatus(WidgetWithStatus::Error, tr("No file is selected."), tr("No file is selected."));
  }

  m_ui->m_buttonBox->button(QDialogButtonBox::Ok)->setDisabled(selected_file.isEmpty());
}

void FormImportExport::selectImportFile() {
  // TODO: vyber soubor a kdyz je vybranej, tak rozparsovat a vytvorit dle nej strukturu
  // itemů a narvat do modelu.
}

void FormImportExport::performAction() {
  switch (m_mode) {
    case Import:
      importFeeds();
      break;

    case Export:
      exportFeeds();
      break;

    default:
      break;
  }
}

void FormImportExport::exportFeeds() {
  switch (m_conversionType) {
    case OPML20: {
      QByteArray result_data;
      bool result_export = m_model->exportToOMPL20(result_data);

      if (result_export) {
        // Save exported data.
        QFile output_file(m_ui->m_lblSelectFile->label()->text());

        if (output_file.open(QIODevice::Unbuffered | QIODevice::Truncate | QIODevice::WriteOnly)) {
          QTextStream stream(&output_file);

          stream.setCodec("UTF-8");
          stream << QString::fromUtf8(result_data);
          output_file.flush();
          output_file.close();

          m_ui->m_lblResult->setStatus(WidgetWithStatus::Ok, tr("Feeds were exported successfully."),
                                       tr("Feeds were exported successfully."));
        }
        else {
          m_ui->m_lblResult->setStatus(WidgetWithStatus::Error, tr("Cannot write into destination file."),
                                       tr("Cannot write into destination file."));
        }
      }
      else {
        m_ui->m_lblResult->setStatus(WidgetWithStatus::Error, tr("Critical error occurred."), tr("Critical error occurred."));
      }
    }

    default:
      break;
  }
}

void FormImportExport::importFeeds() {

}