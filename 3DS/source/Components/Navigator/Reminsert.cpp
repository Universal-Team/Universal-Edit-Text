/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "Actions.hpp"
#include "Common.hpp"
#include "Reminsert.hpp" // Remove Insert FYI.
#include "StatusMessage.hpp"
#include "PromptMessage.hpp"

void Reminsert::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("REMINSERT_MENU"), 310);

	/* Only display if FileHandler is good. */
	if (FileHandler::Loaded) {
		/* Draw the Buttons. */
		for (uint8_t Idx = 0; Idx < 5; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		};

		Gui::DrawStringCentered(26, this->Menu[0].y + 8, 0.45f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("OFFSET") + "0x" + Common::ToHex<uint32_t>(this->Offset));
		Gui::DrawStringCentered(26, this->Menu[1].y + 8, 0.45f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SIZE") + "0x" + Common::ToHex<uint32_t>(this->Size));
		Gui::DrawStringCentered(26, this->Menu[2].y + 8, 0.45f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("TO_INSERT") + "0x" + Common::ToHex<uint8_t>(this->ValueToInsert));

		/* Draw Remove, then Insert. */
		Gui::DrawStringCentered(-40, this->Menu[3].y + 8, 0.45f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[0]));
		Gui::DrawStringCentered(87, this->Menu[4].y + 8, 0.45f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[1]));
	};
};

void Reminsert::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 6; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};


void Reminsert::SetOffs() {
	if (FileHandler::Loaded) {
		this->Offset = Common::HexPad(Common::GetStr("ENTER_OFFSET_IN_HEX"), this->Offset, 0x0, UniversalEdit::UE->CurrentFile->GetSize(), 10);
	};
};

void Reminsert::SetSize() {
	if (FileHandler::Loaded) {
		this->Size = Common::HexPad(Common::GetStr("ENTER_SIZE_IN_HEX"), this->Size, 0x0, 0xFFFFFFFF, 10);
	};
};

void Reminsert::SetVal() {
	if (FileHandler::Loaded) {
		this->ValueToInsert = Common::HexPad(Common::GetStr("ENTER_VALUE_TO_INSERT_IN_HEX"), this->ValueToInsert, 0x0, 0xFF, 4);
	};
};

void Reminsert::Insert() {
	if (FileHandler::Loaded && this->Size > 0 && this->Offset <= UniversalEdit::UE->CurrentFile->GetSize()) {
		Actions::Insert(this->Offset, this->Size, this->ValueToInsert);
	};
};

void Reminsert::Remove() {
	if (FileHandler::Loaded && this->Size > 0 && this->Offset + this->Size <= UniversalEdit::UE->CurrentFile->GetSize()) {
		Actions::Remove(this->Offset, this->Size);
	};
};

void Reminsert::Back() { Navigation::Mode = Navigation::SubMode::Main; };