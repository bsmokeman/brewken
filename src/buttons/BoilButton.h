/*======================================================================================================================
 * buttons/BoilButton.h is part of Brewken, and is copyright the following authors 2024:
 *   • Matt Young <mfsy@yahoo.com>
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
#ifndef BUTTONS_BOILBUTTON_H
#define BUTTONS_BOILBUTTON_H
#pragma once

#include "buttons/RecipeAttributeButton.h"
#include "model/Boil.h"

/*!
 * \class BoilButton
 *
 * \brief This is a view class that displays a named mash
 */
class BoilButton : public RecipeAttributeButton,
                   public RecipeAttributeButtonBase<BoilButton, Boil> {
   Q_OBJECT
   RECIPE_ATTRIBUTE_BUTTON_BASE_DECL(Boil)
};

#endif
