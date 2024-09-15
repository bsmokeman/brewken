/*======================================================================================================================
 * tableModels/FermentableTableModel.cpp is part of Brewken, and is copyright the following authors 2009-2024:
 *   • Brian Rower <brian.rower@gmail.com>
 *   • Daniel Pettersson <pettson81@gmail.com>
 *   • Mattias Måhl <mattias@kejsarsten.com>
 *   • Matt Young <mfsy@yahoo.com>
 *   • Mik Firestone <mikfire@gmail.com>
 *   • Philip Greggory Lee <rocketman768@gmail.com>
 *   • Samuel Östling <MrOstling@gmail.com>
 *   • Tim Payne <swstim@gmail.com>
 *   • Théophane Martin <theophane.m@gmail.com>
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
#include "tableModels/FermentableTableModel.h"

#include <array>

#include <QAbstractItemModel>
#include <QDebug>
#include <QHeaderView>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QWidget>

#include "database/ObjectStoreWrapper.h"
#include "MainWindow.h"
#include "measurement/Measurement.h"
#include "measurement/Unit.h"
#include "model/Inventory.h"
#include "model/Recipe.h"
#include "tableModels/ItemDelegate.h"
#include "utils/BtStringConst.h"
#include "widgets/BtComboBox.h"

//=====================CLASS FermentableTableModel==============================
FermentableTableModel::FermentableTableModel(QTableView* parent, bool editable) :
   BtTableModel{
      parent,
      editable,
      {
         // NOTE: Need PropertyNames::Fermentable::amountWithUnits not PropertyNames::Fermentable::amount below so we
         //       can handle mass-or-volume generically in TableModelBase.  Same for inventoryWithUnits.
         TABLE_MODEL_HEADER(Fermentable, Name              , tr("Name"          ), PropertyNames::NamedEntity::name              ),
         TABLE_MODEL_HEADER(Fermentable, Type              , tr("Type"          ), PropertyNames::Fermentable::type              , EnumInfo{Fermentable::typeStringMapping,
                                                                                                                                            Fermentable::typeDisplayNames}),
         TABLE_MODEL_HEADER(Fermentable, Yield             , tr("Yield (DBFG) %"), PropertyNames::Fermentable::fineGrindYield_pct, PrecisionInfo{1}),
         TABLE_MODEL_HEADER(Fermentable, Color             , tr("Color"         ), PropertyNames::Fermentable::color_srm         , PrecisionInfo{1}),
         TABLE_MODEL_HEADER(Fermentable, TotalInventory    , tr("Inventory"     ), PropertyNames::Ingredient::totalInventory     , PrecisionInfo{1}),
         TABLE_MODEL_HEADER(Fermentable, TotalInventoryType, tr("Amount Type"   ), PropertyNames::Ingredient::totalInventory     , Fermentable::validMeasures),

      }
   },
   TableModelBase<FermentableTableModel, Fermentable>{} {

   // for units and scales
   setObjectName("fermentableTable");

   QHeaderView* headerView = m_parentTableWidget->horizontalHeader();
   connect(headerView, &QWidget::customContextMenuRequested, this, &FermentableTableModel::contextMenu);
   connect(&ObjectStoreTyped<InventoryFermentable>::getInstance(), &ObjectStoreTyped<InventoryFermentable>::signalPropertyChanged, this, &FermentableTableModel::changedInventory);
   return;
}

FermentableTableModel::~FermentableTableModel() = default;

void FermentableTableModel::added  ([[maybe_unused]] std::shared_ptr<Fermentable> item) { return; }
void FermentableTableModel::removed([[maybe_unused]] std::shared_ptr<Fermentable> item) { return; }
void FermentableTableModel::updateTotals()                                              { return; }

QVariant FermentableTableModel::data(QModelIndex const & index, int role) const {
   if (!this->indexAndRoleOk(index, role)) {
      return QVariant();
   }

   // No special handling required for any of our columns
   return this->readDataFromModel(index, role);
}

Qt::ItemFlags FermentableTableModel::flags(QModelIndex const & index) const {
   Qt::ItemFlags constexpr defaults = Qt::ItemIsEnabled;
   auto row = this->rows[index.row()];

   auto const columnIndex = static_cast<FermentableTableModel::ColumnIndex>(index.column());
   switch (columnIndex) {
      case FermentableTableModel::ColumnIndex::Name:
         return (defaults | Qt::ItemIsSelectable);
      case FermentableTableModel::ColumnIndex::TotalInventory:
         return Qt::ItemIsEnabled | Qt::ItemIsEditable;
      default:
         return (defaults | Qt::ItemIsSelectable | (m_editable ? Qt::ItemIsEditable : Qt::NoItemFlags) );
   }
}


bool FermentableTableModel::setData(QModelIndex const & index,
                                    QVariant const & value,
                                    int role) {
   if (!this->indexAndRoleOk(index, role)) {
      return false;
   }

   // No special handling required for any of our columns
   return this->writeDataToModel(index, value, role);
}

// Insert the boiler-plate stuff that we cannot do in TableModelBase
TABLE_MODEL_COMMON_CODE(Fermentable, fermentable, PropertyNames::None::none)
//=========================================== CLASS FermentableItemDelegate ============================================

// Insert the boiler-plate stuff that we cannot do in ItemDelegate
ITEM_DELEGATE_COMMON_CODE(Fermentable)
