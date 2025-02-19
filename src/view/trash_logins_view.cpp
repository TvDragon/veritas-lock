#include "trash_logins_view.h"

TrashLoginsView::TrashLoginsView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	:ptrDataContext(ptrDataContext)
	, ptrDataHandler(ptrDataHandler)
{

}

TrashLoginsView::~TrashLoginsView() {}

void TrashLoginsView::DisplayTrashLoginsView() {
	// Position the window directly below the menu bar
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));

	// Set window size to take up the entire application window below the menu bar
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - ImGui::GetFrameHeight()));

	// Add padding inside the window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10));  // Adjust padding as needed

	ImGui::Begin("Trash Logins", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	
	ImGui::PopStyleVar();	// Restore previous padding style
	
	ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize("Trash Logins");
    ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f); // Center horizontally
	ImGui::Text("Trash Logins");

	std::vector<std::string> colNames = ptrDataHandler->GetTrashColumnNames();
	std::vector<std::map<std::string, std::string>> trashLogins = ptrDataHandler->GetTrashLogins();
	int numColumns = colNames.size();
	int numTrashLogins = trashLogins.size();

	ImGui::BeginTable("trashTable", numColumns + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable);

	ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImColor(169, 180, 194).Value);
	for (int i = 0; i < numColumns; i++) {
		ImGui::TableSetupColumn(colNames[i].c_str());
	}
	ImGui::TableSetupColumn("Restore");
	ImGui::TableSetupColumn("Delete");
	ImGui::TableHeadersRow();
	ImGui::PopStyleColor();
	std::string uniqueLabel;

	for (int row = 0; row < numTrashLogins; row++) {
		ImGui::TableNextRow();
		int column = 0;
		for (; column < numColumns; column++) {
            ImGui::TableSetColumnIndex(column);
			uniqueLabel = trashLogins[row][colNames[column]] + "##" + std::to_string(row) + "-" + std::to_string(column);
			if (ImGui::Selectable(uniqueLabel.c_str())) {
				ImGui::SetClipboardText(trashLogins[row][colNames[column]].c_str());
			}
		}
        ImGui::TableSetColumnIndex(column);
		uniqueLabel = "Restore##" + std::to_string(row) + "-" + std::to_string(column);
		if (ImGui::Button(uniqueLabel.c_str())) {
			ImGui::OpenPopup("Restore Login Confirmation");
			selectedRow = row;
		}
		column++;
		uniqueLabel = "Delete##" + std::to_string(row) + "-" + std::to_string(column);
        ImGui::TableSetColumnIndex(column);
		if (ImGui::Button(uniqueLabel.c_str())) {
			ImGui::OpenPopup("Delete Login Permanently Confirmation");
			selectedRow = row;
		}
		if (selectedRow == row) {
			if (ImGui::BeginPopupModal("Restore Login Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				std::stringstream ss;
				ss << "Do you wish to restore your login with\nID: " +
						trashLogins[selectedRow]["id"] + " for Account: ";
				ss << trashLogins[selectedRow]["account"];
				ImGui::Text(ss.str().c_str());

				float modalWidth = ImGui::GetContentRegionAvail().x;
				float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

				if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
					ptrDataHandler->AddLogin(trashLogins[selectedRow]["account"],
											trashLogins[selectedRow]["type"],
											trashLogins[selectedRow]["username"],
											trashLogins[selectedRow]["email"],
											trashLogins[selectedRow]["password"],
											trashLogins[selectedRow]["notes"],
											trashLogins[selectedRow]["website"],
											trashLogins[selectedRow]["user"]);
					ptrDataHandler->DeleteFromTrash(std::stoi(trashLogins[selectedRow]["id"]));
					selectedRow = -1;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
					selectedRow = -1;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			} else if (ImGui::BeginPopupModal("Delete Login Permanently Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				std::stringstream ss;
				ss << "Do you wish to permanently delete login with\nID: " +
						trashLogins[selectedRow]["id"] + " for Account: ";
				ss << trashLogins[selectedRow]["account"];
				ImGui::Text(ss.str().c_str());

				float modalWidth = ImGui::GetContentRegionAvail().x;
				float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

				if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
					ptrDataHandler->DeleteFromTrash(std::stoi(trashLogins[selectedRow]["id"]));
					selectedRow = -1;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
					selectedRow = -1;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}

	ImGui::EndTable();
	
	ImGui::End();
}