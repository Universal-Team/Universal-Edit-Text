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

#include "Common.hpp"
#include "FileBrowser.hpp"
#include "Data.hpp"
#include "LabelSelector.hpp"
#include "LUAHelper.hpp"
#include "PromptMessage.hpp"

#define BYTES_PER_LIST 0xD0
#define BYTES_PER_OFFS 0x10
#define LINES 0xD

size_t HexEditor::CursorIdx = 0, HexEditor::OffsIdx = 0, HexEditor::EditorMode = 0;

void HexEditor::DrawHexOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < BYTES_PER_OFFS; Idx++) { // 16 Nums per line.
		if (HexEditor::CursorIdx % BYTES_PER_OFFS == Idx) Gui::DrawString(70 + (Idx * 20), 27, 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowHighlightColor()), Utils::ToHex<uint8_t>(Idx));
		else Gui::DrawString(70 + (Idx * 20), 27, 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowColor()), Utils::ToHex<uint8_t>(Idx));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		if (HexEditor::CursorIdx / BYTES_PER_OFFS == Idx) Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetHighlightColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
		else Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		if (Idx2 == HexEditor::CursorIdx) {
			if (this->IsEditMode()) {
				Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->SelectedByte(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));

			} else {
				Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->UnselectedByte(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));
			};

		} else {
			Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));
		};
	};
};

void HexEditor::DrawTextOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < BYTES_PER_OFFS; Idx++) { // 16 Nums per line.
		if (HexEditor::CursorIdx % BYTES_PER_OFFS == Idx) Gui::DrawString(70 + (Idx * 20), 27, 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowHighlightColor()), Utils::ToHex<uint8_t>(Idx));
		else Gui::DrawString(70 + (Idx * 20), 27, 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowColor()), Utils::ToHex<uint8_t>(Idx));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		if (HexEditor::CursorIdx / BYTES_PER_OFFS == Idx) Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetHighlightColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
		else Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		if (Idx2 == HexEditor::CursorIdx) {
			if (this->IsEditMode()) {
				Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->SelectedByte(), UniversalEdit::UE->CurrentFile->GetChar(Idx));

			} else {
				Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->UnselectedByte(), UniversalEdit::UE->CurrentFile->GetChar(Idx));
			};

		} else {
			Gui::DrawString(this->XPositions[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->GetChar(Idx));
		};
	};
};

void HexEditor::DrawTextAndHex() {
	/* Display the top bytes '00, 04, 08, 0C. */
	for (uint8_t Idx = 0; Idx < 4; Idx++) { // 32 bit sections.
		if (HexEditor::CursorIdx % BYTES_PER_OFFS == Idx * 4) Gui::DrawString(this->XPositionsAlt[Idx * 4], 27, 0.38f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowHighlightColor()), Utils::ToHex<uint8_t>(Idx * 0x4));
		else Gui::DrawString(this->XPositionsAlt[Idx * 4], 27, 0.38f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->RowColor()), Utils::ToHex<uint8_t>(Idx * 0x4));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		if (HexEditor::CursorIdx / BYTES_PER_OFFS == Idx) Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetHighlightColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
		else Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetColor()), Utils::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		if (Idx2 == HexEditor::CursorIdx) {
			if (this->IsEditMode()) {
				Gui::DrawString(this->XPositionsAlt[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->SelectedByte(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));
				Gui::DrawString(this->DecodedPos[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->SelectedByte(), UniversalEdit::UE->CurrentFile->GetChar(Idx));

			} else {
				Gui::DrawString(this->XPositionsAlt[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->UnselectedByte(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));
				Gui::DrawString(this->DecodedPos[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->UnselectedByte(), UniversalEdit::UE->CurrentFile->GetChar(Idx));
			};

		} else {
			Gui::DrawString(this->XPositionsAlt[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->ByteToString(Idx));
			Gui::DrawString(this->DecodedPos[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->GetChar(Idx));
		};
	};
};

void HexEditor::DrawTop() {
	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);

		switch(HexEditor::EditorMode) {
			case 0:
				this->DrawHexOnly();
				break;

			case 1:
				this->DrawTextOnly();
				break;

			case 2:
				this->DrawTextAndHex();
				break;
		};

		Gui::DrawString(5, 27, 0.4f, UniversalEdit::UE->TData->HexRowColor(UniversalEdit::UE->CData->OffsetColor()), Utils::GetStr("OFFSET_H"), 390);
	};
};

void HexEditor::DrawBottom() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr("HEX_EDITOR_MENU"), 310);

	/* Draw Buttons. */
	if (FileHandler::Loaded) {
		for (uint8_t Idx = 0; Idx < 6; Idx++) {
			Gui::Draw_Rect(this->HexMenu[Idx].x - 2, this->HexMenu[Idx].y - 2, this->HexMenu[Idx].w + 4, this->HexMenu[Idx].h + 4, UniversalEdit::UE->TData->ButtonColor());
			Gui::Draw_Rect(this->HexMenu[Idx].x, this->HexMenu[Idx].y, this->HexMenu[Idx].w, this->HexMenu[Idx].h, UniversalEdit::UE->TData->BarColor());
			Gui::DrawString(this->HexMenu[Idx].x + 5, this->HexMenu[Idx].y + 5, 0.4f, UniversalEdit::UE->TData->TextColor(), Utils::GetStr(this->MenuOptions[Idx]));
		};
	};
};


void HexEditor::Handler() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		if (this->IsEditMode()) { // Edit the selected byte.
			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] < 0xFF)
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)]++;
			};

			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] > 0x0)
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)]--;
			};

			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] < 0xF0)
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] += 0x10;
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] > 0xF)
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] -= 0x10;
			};

			if (UniversalEdit::UE->Down & KEY_B) {
				this->EditMode = false;
			};

		} else { // Change the Offset.
			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) < UniversalEdit::UE->CurrentFile->GetSize() - 1) {
					if (HexEditor::CursorIdx < BYTES_PER_LIST - 1) HexEditor::CursorIdx++;
					else {
						HexEditor::CursorIdx = BYTES_PER_LIST - BYTES_PER_OFFS;
						HexEditor::OffsIdx++; // One offset row down.
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) > 0) {
					if (HexEditor::CursorIdx > 0) HexEditor::CursorIdx--;
					else {
						HexEditor::CursorIdx = 0xF; // 0xF.
						HexEditor::OffsIdx--; // One offset row down.
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) < UniversalEdit::UE->CurrentFile->GetSize() - BYTES_PER_OFFS) {
					if (HexEditor::CursorIdx >= BYTES_PER_LIST - BYTES_PER_OFFS) HexEditor::OffsIdx++;
					else HexEditor::CursorIdx += BYTES_PER_OFFS;

					if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) >= UniversalEdit::UE->CurrentFile->GetSize()) {
						if (UniversalEdit::UE->CurrentFile->GetSize() < BYTES_PER_LIST) {
							HexEditor::CursorIdx = UniversalEdit::UE->CurrentFile->GetSize() - 1;

						} else {
							HexEditor::CursorIdx += BYTES_PER_LIST - BYTES_PER_OFFS; // TODO: Set to max.
						};
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) > BYTES_PER_OFFS - 1) {
					if (HexEditor::CursorIdx > 0xF) HexEditor::CursorIdx -= BYTES_PER_OFFS;
					else {
						HexEditor::OffsIdx--; // One offset row up.
					};
				};
			};

			if (UniversalEdit::UE->Down & KEY_A) {
				this->EditMode = true;
			};
		};

		if (UniversalEdit::UE->Down & KEY_R) {
			if (HexEditor::EditorMode < 2) HexEditor::EditorMode++;
		};

		if (UniversalEdit::UE->Down & KEY_L) {
			if (HexEditor::EditorMode > 0) HexEditor::EditorMode--;
		};
	};

	/* Button Clicks. */
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->Down & KEY_TOUCH) {
			for (uint8_t Idx = 0; Idx < 5; Idx++) {
				if (Utils::Touching(UniversalEdit::UE->T, this->HexMenu[Idx])) {
					this->Funcs[Idx]();
					break;
				};
			};
		};
	};
};

void HexEditor::Scripts() {
	if (FileHandler::Loaded) {
		std::unique_ptr<LUAHelper> LH = std::make_unique<LUAHelper>();
		LH->RunScript();
	};
};

void HexEditor::Labels() {
	if (FileHandler::Loaded) {
		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string LBFile = FB->Handler("sdmc:/3ds/Universal-Edit/Hex-Editor/Labels/", true, Utils::GetStr("SELECT_LABEL"), { "json" });

		if (LBFile != "") {
			std::unique_ptr<LabelSelector> Label = std::make_unique<LabelSelector>();
			const int Offs = Label->Handler(LBFile);

			if (Offs != -1 && Offs < (int)UniversalEdit::UE->CurrentFile->GetSize()) {
				if (Offs < BYTES_PER_OFFS) {
					HexEditor::OffsIdx = 0;
					HexEditor::CursorIdx = Offs;

				} else {
					HexEditor::OffsIdx = 1 + ((Offs - BYTES_PER_LIST) / BYTES_PER_OFFS);
					HexEditor::CursorIdx = (BYTES_PER_LIST - 0x10) + (Offs % BYTES_PER_OFFS);
				};
			};
		};
	};
};

void HexEditor::Encoding() {
	if (FileHandler::Loaded) {
		std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
		const bool Res = PMessage->Handler(Utils::GetStr("ENCODING_LOAD"));

		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string EncodingFile = FB->Handler((Res ? "sdmc:/3ds/Universal-Edit/Hex-Editor/Encodings/" : "romfs:/encodings/"), true, "Select the Encoding you like to use.", { "json" });

		if (EncodingFile != "") UniversalEdit::UE->CurrentFile->LoadEncoding(EncodingFile);
	};
};


void HexEditor::Convert() {
	if (FileHandler::Loaded) {

	};
};

void HexEditor::InsertBytes() {
	if (FileHandler::Loaded) {

	};
};

void HexEditor::EraseBytes() {
	if (FileHandler::Loaded) {

	};
};