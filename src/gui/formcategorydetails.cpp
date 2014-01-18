#include "gui/formcategorydetails.h"

#include "core/defs.h"
#include "core/feedsmodelrootitem.h"
#include "core/feedsmodelcategory.h"
#include "core/feedsmodelstandardcategory.h"
#include "core/feedsmodel.h"
#include "gui/iconthemefactory.h"
#include "gui/feedsview.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QToolButton>
#include "QComboBox"


FormCategoryDetails::FormCategoryDetails(FeedsModel *model, QWidget *parent)
  : QDialog(parent),
    m_editableCategory(NULL),
    m_feedsModel(model)  {
  initialize();
  loadCategories(model->allCategories().values(),
                 model->rootItem());

  connect(m_ui->m_buttonBox, SIGNAL(accepted()),
          this, SLOT(apply()));
}

FormCategoryDetails::~FormCategoryDetails() {
  qDebug("Destroying FormCategoryDetails instance.");
}

void FormCategoryDetails::setEditableCategory(FeedsModelCategory *editable_category) {
  m_editableCategory = editable_category;

  // TODO: Setup the dialog according to the category.
  // so remove this category from category combobox!!
  m_ui->m_txtTitle->setText(editable_category->title());
  m_ui->m_txtDescription->setText(editable_category->description());
  m_ui->m_btnIcon->setIcon(editable_category->icon());
}

FormCategoryDetailsAnswer FormCategoryDetails::exec(FeedsModelCategory *input_category) {
  FormCategoryDetailsAnswer answer;

  if (input_category == NULL) {
    // User is adding new category.
    setWindowTitle(tr("Add new category"));
  }
  else {
    // User is editing existing category.
    setWindowTitle(tr("Edit existing category"));
    setEditableCategory(input_category);
  }

  answer.m_dialogCode = QDialog::exec();

  return answer;
}

void FormCategoryDetails::apply() {
  if (m_editableCategory == NULL) {
    // add category
    FeedsModelRootItem *parent = static_cast<FeedsModelRootItem*>(m_ui->m_cmbParentCategory->itemData(m_ui->m_cmbParentCategory->currentIndex()).value<void*>());
    FeedsModelStandardCategory *cat = new FeedsModelStandardCategory();

    cat->setTitle(m_ui->m_txtTitle->text());
    cat->setCreationDate(QDateTime::currentDateTime());
    cat->setDescription(m_ui->m_txtDescription->toPlainText());
    cat->setIcon(m_ui->m_btnIcon->icon());
    cat->setType(FeedsModelCategory::Standard);

    if (m_feedsModel->addItem(cat, parent)) {
      accept();
    }
    else {
      // TODO: hlasit chybu
    }
  }
  else {
    // edit category
  }
}

void FormCategoryDetails::initialize() {
  m_ui = new Ui::FormCategoryDetails();
  m_ui->setupUi(this);

  // Set flags and attributes.
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);
  setWindowIcon(IconThemeFactory::instance()->fromTheme("document-new"));
}

void FormCategoryDetails::loadCategories(const QList<FeedsModelCategory *> categories,
                                         FeedsModelRootItem *root_item) {
  m_ui->m_cmbParentCategory->addItem(root_item->icon(),
                                     root_item->title(),
                                     QVariant::fromValue((void*) root_item));
  // pro ziskani root_item static_cast<FeedsModelRootItem*>(itemData(i).value<void*>())
  // a stejně dole ve foreachi


  foreach (FeedsModelCategory *category, categories) {
    m_ui->m_cmbParentCategory->addItem(category->data(FDS_MODEL_TITLE_INDEX,
                                                      Qt::DecorationRole).value<QIcon>(),
                                       category->title(),
                                       QVariant::fromValue((void*) category));
  }
}
