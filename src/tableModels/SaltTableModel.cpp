/*======================================================================================================================
 * tableModels/SaltTableModel.cpp is part of Brewken, and is copyright the following authors 2009-2024:
 *   • Mattias Måhl <mattias@kejsarsten.com>
 *   • Matt Young <mfsy@yahoo.com>
 *   • Mik Firestone <mikfire@gmail.com>
 *   • Philip Greggory Lee <rocketman768@gmail.com>
 *   • Tim Payne <swstim@gmail.com>
 *
 * Brewken is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Brewken is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 =====================================================================================================================*/
#include "tableModels/SaltTableModel.h"

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QComboBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLineEdit>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QStandardItemModel>
#include <QString>
#include <QStyleOptionViewItem>
#include <QVariant>
#include <QWidget>

#include "database/ObjectStoreWrapper.h"
#include "measurement/Measurement.h"
#include "measurement/Unit.h"
#include "model/Mash.h"
#include "model/MashStep.h"
#include "model/Recipe.h"
#include "PersistentSettings.h"
#include "WaterDialog.h"

SaltTableModel::SaltTableModel(QTableView* parent, bool editable) :
   BtTableModel{
      parent,
      editable,
      {
         // NOTE: Need PropertyNames::Salt::amountWithUnits not PropertyNames::Salt::amount below so we
         //       can handle mass-or-volume generically in TableModelBase.
         //
         // Note too that, for the purposes of these columns, the "name" of a Salt is not its "NamedEntity name" but actually its type
         TABLE_MODEL_HEADER(Salt, Name   , tr("Name"    ), PropertyNames::Salt::type           , EnumInfo{Salt::typeStringMapping, Salt::typeDisplayNames}),
///         TABLE_MODEL_HEADER(Salt, Amount , tr("Amount"  ), PropertyNames::Salt::amountWithUnits),
///         TABLE_MODEL_HEADER(Salt, AddTo  , tr("Added To"), PropertyNames::Salt::whenToAdd      , EnumInfo{Salt::whenToAddStringMapping, Salt::whenToAddDisplayNames}),
         TABLE_MODEL_HEADER(Salt, PctAcid, tr("% Acid"  ), PropertyNames::Salt::percentAcid    ),
         TABLE_MODEL_HEADER(Salt, TotalInventory    , tr("Inventory"  ), PropertyNames::Ingredient::totalInventory, PrecisionInfo{1}),
         TABLE_MODEL_HEADER(Salt, TotalInventoryType, tr("Amount Type"), PropertyNames::Ingredient::totalInventory, Salt::validMeasures),
      }
   },
   TableModelBase<SaltTableModel, Salt>{} {
   setObjectName("saltTable");

   QHeaderView* headerView = m_parentTableWidget->horizontalHeader();
   headerView->setMinimumSectionSize(parent->width()/this->columnCount());

   connect(headerView, &QWidget::customContextMenuRequested, this, &SaltTableModel::contextMenu);
   return;
}

SaltTableModel::~SaltTableModel() = default;

void SaltTableModel::added  ([[maybe_unused]] std::shared_ptr<Salt> item) { return; }
void SaltTableModel::removed([[maybe_unused]] std::shared_ptr<Salt> item) { return; }
void SaltTableModel::updateTotals()                                       { return; }

QVariant SaltTableModel::data(QModelIndex const & index, int role) const {
   if (!this->indexAndRoleOk(index, role)) {
      return QVariant();
   }

   // No special handling required for any of our columns
   return this->readDataFromModel(index, role);
}

Qt::ItemFlags SaltTableModel::flags(const QModelIndex& index) const {
   // Q_UNUSED(index)
   if (index.row() >= this->rows.size() ) {
      return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
   }

   auto const row = this->rows[index.row()];
   if (!row->isAcid() && index.column() == static_cast<int>(SaltTableModel::ColumnIndex::PctAcid))  {
      return Qt::NoItemFlags;
   }
   return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
}

bool SaltTableModel::setData(QModelIndex const & index, QVariant const & value, int role) {
   if (!this->indexAndRoleOk(index, role)) {
      return false;
   }

   // No special handling required for any of our columns...
   bool const retVal = this->writeDataToModel(index, value, role);

   // ...but some other post-modification things we check
   emit dataChanged(index,index);
   QHeaderView* headerView = m_parentTableWidget->horizontalHeader();
   headerView->resizeSections(QHeaderView::ResizeToContents);

   return retVal;
}

///void SaltTableModel::saveAndClose() {
///   // all of the writes should have been instantaneous unless
///   // we've added a new salt. Wonder if this will work?
///   for (auto salt : this->rows) {
///      if (salt->key() < 0 &&
///         salt->whenToAdd() != Salt::WhenToAdd::NEVER) {
///         ObjectStoreWrapper::insert(salt);
///         this->recObs->add(salt);
///      }
///   }
///   return;
///}

// Insert the boiler-plate stuff that we cannot do in TableModelBase
TABLE_MODEL_COMMON_CODE(Salt, salt, PropertyNames::None::none)
//=============================================== CLASS SaltItemDelegate ===============================================

// Insert the boiler-plate stuff that we cannot do in ItemDelegate
ITEM_DELEGATE_COMMON_CODE(Salt)

// .:TBD:. We don't currently replicate the enabled/disabled logic from the fragment of old commented-out code below.
//         AFAICT it's graying out the SPARGE and RATIO options if the observed recipe mash has no sparge.

///   if (columnIndex == SaltTableModel::ColumnIndex::AddTo) {
///      QComboBox *box = new QComboBox(parent);
///
///      ...
///
///      if ( m_mash != nullptr ) {
///         QStandardItemModel* i_model = qobject_cast<QStandardItemModel*>(box->model());
///         if ( ! m_mash->hasSparge() ) {
///            for( int i = 2; i < 5; ++i ) {
///               QStandardItem* entry = i_model->item(i);
///               if ( entry )
///                  entry->setEnabled(false);
///            }
///         }
///      }
///
///      return box;
///   }
